#!/bin/bash

[ "$3" = "" ] && echo "Generates an onauth script of custom messages to be used for incorrect login attempts.

  `basename $0` <onauth_head.sh> <onauth_repl.sh> <messages.txt>

" && exit -1

mess_file=$3
! [ -e $mess_file ] && echo "Messages file does not exist" && exit -1

# Set values
message1=$(cat $mess_file | grep message1)
message2=$(cat $mess_file | grep message2)
message3=$(cat $mess_file | grep message3)

[ "$message1" = "" ] && echo "message1 not set" && exit -1
[ "$message2" = "" ] && echo "message2 not set" && exit -1
[ "$message3" = "" ] && echo "message3 not set" && exit -1

# Begin generating onauth script
onauth_head=$1; ! [ -e $onauth_head ] && echo "onauth_head.sh not found" && exit -1
onauth_repl=$2; ! [ -e $onauth_repl ] && echo "onauth_repl.sh not found" && exit -1

cat $onauth_head

# Replace messages in onauth_repl.sh
tmp1=`mktemp`
tmp2=`mktemp`
tmp3=`mktemp`

## Escape double quotes, remove number specifiers
message1=$(echo "$message1" | sed 's|"|\\"|' | sed 's|message[0-9]|message|')
message2=$(echo "$message2" | sed 's|"|\\"|' | sed 's|message[0-9]|message|')
message3=$(echo "$message3" | sed 's|"|\\"|' | sed 's|message[0-9]|message|')

cat $onauth_repl | sed "s|__MESSAGE1__|$message1|" > $tmp1
cat $tmp1        | sed "s|__MESSAGE2__|$message2|" > $tmp2
cat $tmp2        | sed "s|__MESSAGE3__|$message3|" > $tmp3

cat $tmp3
