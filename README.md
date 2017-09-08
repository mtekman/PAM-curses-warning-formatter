# PAM Curses Warner

An interactive curses application that threatens to format your disks upon the three failed login attempts at boot via [PAM](https://en.wikipedia.org/wiki/Pluggable_authentication_module), complete with customizable fail messages.

![pic](https://user-images.githubusercontent.com/20641402/30215500-6b031260-94b0-11e7-974b-2f1d78bad4ad.png)

#### Configure

Edit *custom_fail_messages.txt* to your own preferred text produced in order of each incorrect login attempt. Emphasis is given by surrounding words in brackets, and newlines are inserted at each adjacent string.

 e.g   "[This] is" "YOUR FINAL WARNING" "BURN ALL YOUR [CLASSIFIED DOCUMENTS]"  

will resolve to:

> **This** is  
 YOUR FINAL WARNING  
 BURN ALL YOUR **CLASSIFIED DOCUMENTS**  

****

### Install:

Run `install.sh`

which calls *scripts/{buildInstallPamScript.sh, buildWarner.sh, buildOnAuth.sh}*, then copies all configs over to root directories.

***Specifically:***

 * `buildInstallPamScript.sh` builds and copies pam_script.so into */lib/security/*

 * `buildWarner.sh` builds and copies the interactive warner to *configs/etc/security/warner*
 
 * `buildOnAuth.sh` generates and inserts custom messages into *configs/etc/security/onauth*


### Run:

Attempt to login from a tty (*not* vtty) and input incorrect credentials 3 times in a row.

****  
**NOTE:** It does not actually format your machine, merely frightens unsuspecting users.
