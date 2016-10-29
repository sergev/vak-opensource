:
#
# Split binary file into pieces.
# Usage:
#       bsplit [-#] [file [name]]
# Flags:
#       -#      - maximum piece length in bytes
#
# Author: Serge Vakulenko, vak@kiae.su
#

PATH=/usr/spool/mailserv/bin:/usr/local/bin:/usr/ucb:/usr/bin:/bin:/etc
export PATH

max=102400

case $1 in
-[0-9]*)
	max=`echo $1 | tr -d -`
	shift
	;;
esac

if [ $# = 1 ]
then
	file=$1
	name=part
elif [ $# = 2 ]
then
	file=$1
	name=$2
else
	echo "Usage:"
	echo "      bsplit [-#] [file [name]]"
	echo "Flags:"
	echo "      -#      - maximum piece length in bytes, default $max"
	exit 1
fi

if [ -f /usr/bin/gawk ]; then
	awk=/usr/bin/gawk
elif [ -f /usr/local/bin/gawk ]; then
	awk=/usr/local/bin/gawk
elif [ -f /usr/bin/nawk ]; then
	awk=/usr/bin/nawk
elif [ -f /usr/bin/awk ]; then
	awk=/usr/bin/awk
elif [ -f /bin/awk ]; then
	awk=/bin/awk
else
	echo "Cannot find awk."
	exit 1
fi

tmpf=/tmp/bs$$
trap "rm -rf $tmpf; exit 1" 1 2 3 15

if [ x$file = x- ]
then
	dd of=$tmpf
	file=$tmpf
fi

size=`filesize $file`
exec < $file

n=1
skip=0
while [ $skip -lt $size ]
do
	ext=$name`echo $n | awk '{ printf "%04d\n", $1 }'`
	dd bs=$max of=$ext count=1 >/dev/null 2>&1
	skip=`expr $skip \+ $max`
	n=`expr $n + 1`
done

rm -f $tmpf
