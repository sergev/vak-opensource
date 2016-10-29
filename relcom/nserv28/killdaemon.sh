:
# Shut down news mail server daemon.

dir=/usr/spool/newsserv
pidfile=$dir/daemonpid
rpidfile=$dir/rgrouppid

if [ -f $pidfile ]; then
	kill `cat $pidfile` 2>/dev/null
fi

if [ -f $rpidfile ]; then
	kill -9 `cat $rpidfile` 2>/dev/null
fi

if [ -f $dir/bin/rgroupd ]; then
	$dir/bin/nsadmin die
fi
