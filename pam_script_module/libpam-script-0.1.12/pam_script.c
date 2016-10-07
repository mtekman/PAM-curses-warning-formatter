/* Copyright Izak Burger 2005.
 * Licensed under the GPL. */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <grp.h>

#define PAM_SM_SESSION
#define PAM_SM_AUTH
/* #define PAM_SM_PASSWORD */

#define EXPOSE_AUTHTOK (1)
#define EXPOSE_KRB5CCNAME (2)
#define EXPOSE_RHOST (4)
#define EXPOSE_USER (8)
#define EXPOSE_HOME (16)

#include <security/_pam_macros.h>
#include <security/pam_modules.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define LOCAL_LOG_PREFIX "PAM-script: "

struct pamscript_options {
	struct passwd *pw;
	char cmd[256];
	char *realuser;
	char *service;
	int cmdoverride;
	int expose;
	int initgroups;
};

static void _pam_log(int err, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vsyslog(LOG_AUTH | err, format, args);
	va_end(args);
}

/* Stolen and adapted from pam_ldap */
static int _get_authtok (pam_handle_t *pamh){
	int rc;
	char *p;
	struct pam_message msg[1], *pmsg[1];
	struct pam_response *resp; 
	struct pam_conv *conv;

	pmsg[0] = &msg[0];
	msg[0].msg_style = PAM_PROMPT_ECHO_OFF;
	msg[0].msg = "Password: ";
	resp = NULL; 
	rc = pam_get_item(pamh, PAM_CONV, (const void **)&conv);
	if (rc == PAM_SUCCESS){
		rc = conv->conv (1,
				(const struct pam_message **) pmsg,
				&resp, conv->appdata_ptr);
	} else {
		return rc;
	}

	if (resp != NULL){
		p = resp[0].resp;
		/* leak if resp[0].resp is malloced. */
		resp[0].resp = NULL;
	} else {
		return PAM_CONV_ERR;
	}

	free (resp);
	pam_set_item (pamh, PAM_AUTHTOK, p);

	return PAM_SUCCESS;
}


static int parse_options(pam_handle_t *pamh,
		int argc, const char **argv,
		const char *execname, struct pamscript_options *options){
	char *user;
	char myuser[256];
	int i;

	pam_get_item(pamh, PAM_SERVICE, (const void**)&(options->service));

	memset(options->cmd, '\0', sizeof(options->cmd));
	snprintf(options->cmd, sizeof(options->cmd), "/etc/security/%s", execname);
	options->cmdoverride = 0;
	options->expose = 0;

	/* Get the username of the person logging in right now. */
	pam_get_user(pamh, (const char**)&user, NULL);
	if(user == NULL){
		_pam_log(LOG_ERR, LOCAL_LOG_PREFIX "Cannot determine name of user");
		return 0;
	}
	options->realuser = user;
	options->expose = 0;
	options->initgroups = 0;

	/* Handle arguments */
	for (i=0; i < argc; i++) {
		char buf[1024];
		char *opt;
		char *val;
		memset(buf, '\0', sizeof(buf));
		strncpy(buf, argv[i], sizeof(buf)-1);
		opt = buf;
		val = strchr(buf, '=');
		if(val){
			val[0] = '\0'; /* Split the string */
			val++;
			if(strcmp(opt, execname)==0){
				memset(options->cmd, '\0', sizeof(options->cmd));
				strncpy(options->cmd, val, sizeof(options->cmd) - 1);
				options->cmdoverride = 1;
			} else if(strcmp(opt, "runas")==0){
				memset(myuser, '\0', sizeof(myuser));
				strncpy(myuser, val, sizeof(myuser) - 1);
				user = myuser;
			} else if(strcmp(opt, "expose")==0) {
				if(strcmp(val, "authtok")==0){
					options->expose |= EXPOSE_AUTHTOK;
				} else if(strcmp(val, "krb5ccname")==0){
					options->expose |= EXPOSE_KRB5CCNAME;
				} else if(strcmp(val, "rhost")==0){
					options->expose |= EXPOSE_RHOST;
				} else if(strcmp(val, "user")==0){
					options->expose |= EXPOSE_USER;
				} else if(strcmp(val, "home")==0){
					options->expose |= EXPOSE_HOME;
				}
			} else if(strcmp(opt, "initgroups")==0){
				options->initgroups = 1;
			} else {
				_pam_log(LOG_ERR, LOCAL_LOG_PREFIX "Bad option: \"%s\"",
				    opt);
			}
		}
	}

	options->pw = getpwnam(user);
	/* Unable to get password info, return session error */
	if (options->pw==NULL){
		_pam_log(LOG_ERR, LOCAL_LOG_PREFIX "Unable to get passwd entry for %s", user);
		return 0;
	}

	return 1;
}

static int session_script(struct pamscript_options options, int numenv, ...)
{
	pid_t pid;
	sigset_t newmask, oldmask;
	int retcode = 0;

	_pam_log(LOG_DEBUG, LOCAL_LOG_PREFIX "Real User is: %s", options.realuser);
	_pam_log(LOG_DEBUG, LOCAL_LOG_PREFIX "Command is:   %s", options.cmd);
	_pam_log(LOG_DEBUG, LOCAL_LOG_PREFIX "Executing uid:gid is: %d:%d", options.pw->pw_uid, options.pw->pw_gid);

	/* Block SIGCHLD */
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGCHLD);
	sigprocmask(SIG_BLOCK, &newmask, &oldmask);

	/* Time to fork, we check for existence and executability of the script later, because we
	 * need to do that as the user executing the script, as it is possible that the script is
	 * executable by root but not by the user */
	pid = fork();
	if(pid<0){
		_pam_log(LOG_ERR, LOCAL_LOG_PREFIX "Unable to fork");
		/* Restore signal mask */
		sigprocmask(SIG_SETMASK, &oldmask, NULL);
		return 0;
	} else if(pid==0) {
		/* ======== Child ========= */
		int i;
		/* Order of setregid and setreuid is important, first group, then user */
		if (
			setgroups(0, NULL)==0 &&
			(options.initgroups?(
				initgroups(options.pw->pw_name, options.pw->pw_gid)==0):1) &&
			setregid(options.pw->pw_gid, options.pw->pw_gid)==0 && 
			setreuid(options.pw->pw_uid, options.pw->pw_uid)==0
		){
			struct stat st;
			char *arge[4];
			va_list env;
			char *key;
			char *value;
			if (stat(options.cmd, &st) || !(S_ISREG(st.st_mode)) || !(st.st_mode&(S_IXUSR|S_IXGRP|S_IXOTH))) {
				_pam_log(LOG_ERR, LOCAL_LOG_PREFIX "%s does not exist or is not executable", options.cmd);
				/* If the arguments supplied us with a script to run and it is not runable, return
				 * non-zero as this is an error.  Otherwise, return zero since it simply means the
				 * default doesn't exist, ie it is unconfigured and that is okay in our book */
				exit(options.cmdoverride);
			}
			/* Set up environment */
			va_start(env, numenv);
			for(i=0; i<numenv; i++){
				key   = va_arg(env, char*);
				value = va_arg(env, char*);
				if(value != NULL){
					setenv(key, value, 0);
				}
			}
			va_end(env);

			arge[0] = options.cmd;
			arge[1] = options.realuser;
			arge[2] = options.service; /* Pass service as second argument (thanks Hanno!) */
			arge[3] = '\0';
			execvp(arge[0], arge);
			/* If we reach this, execvp failed.  Exit with an error */
			_pam_log(LOG_ERR, LOCAL_LOG_PREFIX "execvp error: %d", errno);
			exit(1);
		} else {
			_pam_log(LOG_ERR, LOCAL_LOG_PREFIX "Unable to run as uid %d", options.pw->pw_uid);
		}
		/* ============= Child Ends ======== */
	}
	/* if pid <=0, we would not be here, so pid>0, which means we are the parent */
	/* wait for the child to return */
	while(waitpid(pid, &retcode, 0)==-1){
		if(errno != EINTR){
			_pam_log(LOG_ERR, LOCAL_LOG_PREFIX "waitpid error: %d", errno);
			/* Restore signal mask */
			sigprocmask(SIG_SETMASK, &oldmask, NULL);
			return 0;
		}
	}
	if (!WIFEXITED(retcode)) {/* terminated abnormally */
		_pam_log(LOG_ERR,LOCAL_LOG_PREFIX "Executing file: \"%s\" as uid %d returned abnormally", options.cmd, options.pw->pw_uid);
		/* Restore signal mask */
		sigprocmask(SIG_SETMASK, &oldmask, NULL);
		return 0;
	}
	if(WEXITSTATUS(retcode)!=0){ /* Terminated normally, but non-zero */
		_pam_log(LOG_ERR,LOCAL_LOG_PREFIX "Executing file: \"%s\" as uid %d returned %d", options.cmd, options.pw->pw_uid, WEXITSTATUS(retcode));
		/* Restore signal mask */
		sigprocmask(SIG_SETMASK, &oldmask, NULL);
		return 0;
	}

	/* SUCCESS */
	/* Restore signal mask */
	sigprocmask(SIG_SETMASK, &oldmask, NULL);

	return 1;
}

/* Session management stuff */
PAM_EXTERN int pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
	struct pamscript_options options;
	if(parse_options(pamh, argc, argv, "onsessionopen", &options) && session_script(options, 0)){
		return PAM_SUCCESS;
	}
	return PAM_SESSION_ERR;
}

PAM_EXTERN int pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
	struct pamscript_options options;
	if(parse_options(pamh, argc, argv, "onsessionclose", &options) && session_script(options, 0)){
		return PAM_SUCCESS;
	}
	return PAM_SESSION_ERR;
}

/* Authentication stuff */
PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
	struct pamscript_options options;
	char *password;
	char *krb5ccname;
	char *rhost;
	char* user;
	char* home;

	if(!parse_options(pamh, argc, argv, "onauth", &options)){
		return PAM_AUTHINFO_UNAVAIL;
	}

	user = NULL;
	password = NULL;
	krb5ccname = NULL;
	rhost = NULL;
	home = NULL;

	if(options.expose & EXPOSE_AUTHTOK){
		/* Find password */
		pam_get_item(pamh, PAM_AUTHTOK, (const void **)&password);
		/* If none, prompt user for one */
		if(password == NULL){
			int rc;
			rc = _get_authtok(pamh);
			if(rc != PAM_SUCCESS){
				return rc;
			}
			pam_get_item(pamh, PAM_AUTHTOK, (const void **)&password);
		}
	}

	if(options.expose & EXPOSE_KRB5CCNAME){
		krb5ccname = (char*)pam_getenv(pamh, "KRB5CCNAME");
	}

	if(options.expose & EXPOSE_RHOST){
		pam_get_item(pamh, PAM_RHOST, (const void **)&rhost);
	}

	if (options.expose & EXPOSE_USER) {
		user = options.realuser;
	}

	if (options.expose & EXPOSE_HOME) {
		home = options.pw->pw_dir;
	}

	if(session_script(options, 5,
		"PAM_AUTHTOK", password,
		"KRB5CCNAME", krb5ccname,
		"PAM_RHOST", rhost,
		"PAM_USER", user,
		"HOME", home)){
		return PAM_SUCCESS;
	}

	return PAM_AUTH_ERR;
}

PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
	/* No additional credentials to set */
	return PAM_IGNORE;
}
