#!/bin/sh

pts=$(tty)

# prime tally
tally=/tmp/tally
! [ -e $tally ] && echo 0 > $tally


function message(){
    color="\e[91m" #red
    reset="\e[39m"

    for arg in "$@"; do   
	mess="$arg"
	echo -en "$color" >> $pts
	
	for (( c=0; c <${#mess}; c ++ )); do
	    char=${mess: $c:1}
	    case "$char" in
		"[") char=${reset}"\e[41m" ;; # bold red
		"]") char="\e[49m\e[91m" ;; # regular red
	    esac
	    echo -en "$char" >> $pts
	    sleep 0.05
	done

	sleep 0.5
	echo -en "$reset\n" >> $pts
    done
    echo -en "\n\n" >> $pts
}
