#1/bin/bash
cd warner
gcc curses_warner.c -lncurses -o warner
cd ..
cp warner/warner configs/etc/security/
