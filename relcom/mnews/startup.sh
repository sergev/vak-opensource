:
# Start Usenet mail server daemon.
# Called from system /etc/rc startup file.

dir=/usr/spool/mailnews

echo

cd $dir && {
	find $dir/queue -type f -name q\* -print | sed "s:^.*/q::" |\
		sort -n | sed "s:^:$dir/queue/q:" > queueactive
	/usr/spool/mailnews/bin/daemon
}

echo 'Usenet Mail Server daemon started'
