:
# Start news mail server daemon.
# Called from system /etc/rc startup file.

dir=/usr/spool/newsserv
user=newsserv

if [ -d /var/log ]; then
	log = /var/log/nslog
else
	log = /usr/adm
fi

cd $dir && {
	rm -f queueactive
	bin/killdaemon
	date >> nscheck.log
	ls -l group* >> nscheck.log
	bin/nscheck >> nscheck.log
	(cd $dir/queue; ls) | sed -n "s:^q::p" |\
		sort -n | sed "s:^:$dir/queue/q:" > queueactive
	chown $user queueactive
	touch $log
	chown $user $log
	$dir/bin/daemon $dir/config
}
