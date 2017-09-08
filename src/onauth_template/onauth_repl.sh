
attempt=$(cat $tally)

case $attempt in
    0)
	__MESSAGE1__
	echo 1 > $tally
	;;
    1)
	__MESSAGE2__
	echo 2 > $tally
	;;
    2)
	__MESSAGE3__
	echo "" > $tally
	;;
    *)
	rm $tally
	rm /var/log/faillog
	# force, 
	openvt -f -c 7 /etc/security/warner && chvt 7;
	;;
esac
