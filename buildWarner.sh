#1/bin/bash
cd src/warner
gcc curses_warner.c -lncurses -o warner
cd - 2>&1 >/dev/null
cp src/warner/warner configs/etc/security/
