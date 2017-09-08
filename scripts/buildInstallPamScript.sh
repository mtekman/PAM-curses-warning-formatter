#!/bin/bash

[ "`basename $(pwd)`" = "scripts" ] && echo "Please execute this from the root directory" && exit -1

cd src/pam_script_module/libpam-script-0.1.12
make clean
make
sudo cp pam_script.so /lib/security/
cd  - 2>&1 >/dev/null

echo "[OK] pam_script built and installed"
