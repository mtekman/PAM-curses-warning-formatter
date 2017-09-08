#!/bin/bash

[ "`basename $(pwd)`" = "scripts" ] && echo "Please execute this from the root directory" && exit -1

cd src/warner
gcc curses_warner.c -lncurses -o warner
cd - 2>&1 >/dev/null
cp src/warner/warner configs/etc/security/ &&

echo "[OK] warner built and installed"
