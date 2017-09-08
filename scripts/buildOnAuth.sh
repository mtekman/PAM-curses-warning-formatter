#!/bin/bash

[ "`basename $(pwd)`" = "scripts" ] && echo "Please execute this from the root directory" && exit -1


function fileCheck(){
    file=$1
    ! [ -e $file ] && echo "$file is missing!" && exit -1
    return 0
}

onauth_folder=src/onauth_template;      fileCheck $onauth_folder &&
messages=./custom_fail_messages.txt;    fileCheck $messages &&

gnrt=$onauth_folder/generate.sh;        fileCheck $gnrt &&
head=$onauth_folder/onauth_head.sh;     fileCheck $head &&
repl=$onauth_folder/onauth_repl.sh;     fileCheck $repl &&

$gnrt $head $repl $messages > configs/etc/security/onauth &&

echo "[OK] onauth script built and custom messages inserted"
