# PAM Curses Warner

An interactive curses application that threatens to format your disks upon the three failed login attempts at boot via [PAM](https://en.wikipedia.org/wiki/Pluggable_authentication_module), complete with customizable fail messages.

![pic](https://user-images.githubusercontent.com/20641402/30215500-6b031260-94b0-11e7-974b-2f1d78bad4ad.png)

#### Configure

Change `messages.txt` to your own custom messages prompted at each incorrect login attempt. Emphasis is given by surrounding words in brackets, and newlines are inserted at each adjacent string.

 e.g   "[This] is" "YOUR FINAL WARNING" "BURN ALL YOUR [CLASSIFIED DOCUMENTS]"  

will resolve to:

> **This** is  
 YOUR FINAL WARNING  
 BURN ALL YOUR **CLASSIFIED DOCUMENTS**  

****

### Install:

Run `install.sh`

which calls *buildInstallPamScript.sh* and *buildWarner.sh*, then copies all configs over to root directories.

***Specifically:***

 * `buildInstallPamScript.sh` builds and copies pam_script.so into */lib/security/*

and

 * `buildWarner.sh` builds and copies the interactive warner into *configs/etc/security/*


### Run:

Attempt to login from a tty (*not* vtty) and input incorrect credentials 3 times in a row.

****  
**NOTE:** It does not actually format your machine, merely frightens unsuspecting users.
