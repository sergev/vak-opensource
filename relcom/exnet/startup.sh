
# Start Infosoft mail server daemon.
# Called from system /etc/rc startup file.

echo

rm -f /usr/spool/trade/*lock
/usr/spool/trade/bin/daemon

echo 'Infosoft Mail Server daemon started'
echo 'Copyright (C) 1991 Serge Vakulenko'
