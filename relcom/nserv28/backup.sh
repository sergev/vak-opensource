:
# Backup the server database.
# Called by server daemon.

dir=/usr/spool/newsserv

cd $dir
if [ $? != 0 ]; then
	echo "Cannot chdir to $dir"
	exit 1
fi

if [ -f groups~ ]; then
	mv -f groups.6.F groups.7.F
	mv -f groups.5.F groups.6.F
	mv -f groups.4.F groups.5.F
	mv -f groups.3 groups.4
	/usr/local/bin/freeze groups.4
	mv -f groups.2 groups.3
	mv -f groups.1 groups.2
	mv -f groups~  groups.1
fi


if [ -f core ]; then
	mv -f core.1 core.2
	mv -f core   core.1
fi

mv -f log.8.z log.9.z
mv -f log.7.z log.8.z
mv -f log.6.z log.7.z
mv -f log.5.z log.6.z
mv -f log.4.z log.5.z
mv -f log.3 log.4
gzip log.4
mv -f log.2 log.3
mv -f log.1 log.2
mv -f log log.1

mv -f instat.6 instat.7
mv -f instat.5 instat.6
mv -f instat.4 instat.5
mv -f instat.3 instat.4
mv -f instat.2 instat.3
mv -f instat.1 instat.2
mv -f instat instat.1

#mv -f outstat.6 outstat.7
#mv -f outstat.5 outstat.6
#mv -f outstat.4 outstat.5
#mv -f outstat.3 outstat.4
#mv -f outstat.2 outstat.3
#mv -f outstat.1 outstat.2
mv -f outstat outstat.1
bin/daemon > /dev/null 2>& 1 &
