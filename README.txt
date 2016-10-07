

buildInstallPamScript.sh
  --- builds and copies pam_script.so into `/lib/security/`

buildWarner.sh
  --- builds and copies warner into `configs/etc/security/`

installAll.sh 
  --- calls buildInstallPamScript.sh and buildWarner.sh, 
      then copies all configs over to root directories.
