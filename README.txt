Curses (Fake) Disk Formatter Banner using PAM script
====================================================


Install:

buildInstallPamScript.sh
  --- builds and copies pam_script.so into `/lib/security/`

buildWarner.sh
  --- builds and copies warner into `configs/etc/security/`

installAll.sh 
  --- calls buildInstallPamScript.sh and buildWarner.sh, 
      then copies all configs over to root directories.



Test:

Attempt a login or sudo and type in wrong credentials
