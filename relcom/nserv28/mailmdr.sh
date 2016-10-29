:
# Unapproved article: mail it to the moderator(s).
# Look for a route for this group.

mailpaths=/usr/lib/news/mailpaths
modroute=/tmp/pm$$

if [ -f /usr/lib/news/bin/gngp ]; then
	gngp=/usr/lib/news/bin/gngp
fi

if [ -f /usr/lib/newsbin/gngp ]; then
	gngp=/usr/lib/newsbin/gngp
fi

if [ x"$gngp" = x ]; then
	echo "mailmdr: gngp not found"
	exit 1
fi

ng=$1

if [ x"$ng" = x ]; then
	echo "Usage: mailmgr group < article"
	exit 1
fi

trap "rm -f $modroute; exit 1" 0 1 2 3 15

($gngp -a -r $ng < $mailpaths; echo default %s) | tr '\011' ' ' | sed -n "1{
	s/^[^ ]*  *//
	s/%s/`echo $ng | tr . -`/
	p
	q
}" >$modroute
moderator=`cat $modroute`
rm -f $modroute
trap '' 0 1 2 3 15
exec mail $moderator
