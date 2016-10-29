
# Start info server daemon.
# Called from system /etc/rc startup file.

LOGNAME=infoserv
USER=infoserv
HOME=/usr/spool/infoserv
export LOGNAME USER HOME

echo

su infoserv -c /usr/spool/infoserv/bin/infoservd

echo 'Relcom Info Server daemon started'
echo 'Copyright (C) 1992 Serge Vakulenko'
