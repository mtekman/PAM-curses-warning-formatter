#!/bin/bash

cd pam_script_module/libpam-script-0.1.12
make clean
make
sudo cp pam_script.so /lib/security/
cd  ../../
