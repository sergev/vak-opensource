#!/bin/sh
echo $* >> /usr/spool/newsserv/logmail
exec /bin/cat > /dev/null
