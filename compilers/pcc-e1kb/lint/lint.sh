TOUT=/usr/tmp/tlint.$$
HOUT=/usr/tmp/hlint.$$
LDIR=/usr/SVSB/lib
LLDIR=/usr/SVSB/lib
PATH=/bin:/usr/bin:/usr/SVSB/bin
CCF="-E -C -Dlint"
LINTF=
FILES=
NDOTC=
DEFL=$LLDIR/llib-lc.ln
LLIB=
CONLY=
pre=
post=
optarg=
trap "rm -f $TOUT $HOUT; exit 2" 1 2 3 15
for OPT in "$@"
do
	if [ "$optarg" ]
	then
		if [ "$optarg" = "LLIB" ]
		then
			OPT=`basename $OPT`
		fi
		eval "$optarg=\"\$$optarg \$pre\$OPT\$post\""
		pre=
		post=
		optarg=
		continue
	fi
	case "$OPT" in
	*.c)	FILES="$FILES $OPT"	NDOTC="x$NDOTC";;
	*.ln)	FILES="$FILES $OPT";;
	-*)     OPT=`echo $OPT | sed s/-//`
		while [ "$OPT" ]
		do
			O=`echo $OPT | sed 's/\\(.\\).*/\\1/'`
			OPT=`echo $OPT | sed s/.//`
			case $O in
			p)	LINTF="$LINTF -p"
				CCF="$CCF -Wp,-T"
				DEFL=$LLDIR/llib-port.ln;;
			n)	LINTF="$LINTF -n"
				DEFL=;;
			c)	CONLY=1;;
			[abhuvx]) LINTF="$LINTF -$O";;
			[gO])	CCF="$CCF -$O";;
			[IDU])	if [ "$OPT" ]
				then
					CCF="$CCF -$O$OPT"
				else
					optarg=CCF
					pre=-$O
				fi
				break;;
			l)	if [ "$OPT" ]
				then
					FILES="$FILES $LLDIR/llib-l$OPT.ln"
				else
					optarg=FILES
					pre=$LLDIR/llib-l
					post=.ln
				fi
				break;;
			o)	if [ "$OPT" ]
				then
					OPT=`basename $OPT`
					LLIB="llib-l$OPT.ln"
				else
					LLIB=
					optarg=LLIB
					pre=llib-l
					post=.ln
				fi
				break;;
			*)	echo "lint: bad option ignored: $O";;
			esac
		done;;
	*)	echo "lint: file with unknown suffix ignored: $OPT";;
	esac
done
if [ "$NDOTC" != "x" ]
then
	NDOTC=1
else
	NDOTC=
fi
if [ "$CONLY" ]
then
	for i in $FILES
	do
		case $i in
		*.c)	T=`basename $i .c`.ln
			if [ "$NDOTC" ]
			then
				echo $i:
			fi
			(cc $CCF $i | $LDIR/lint1 $LINTF -H$HOUT $i >$T) 2>&1
			$LDIR/lint2 -H$HOUT
			rm -f $HOUT;;
		esac
	done
else
	rm -f $TOUT $HOUT
	for i in $FILES
	do
		case $i in
		*.ln)	cat <$i >>$TOUT;;
		*.c)	if [ "$NDOTC" ]
			then
				echo $i:
			fi
			(cc $CCF $i|$LDIR/lint1 $LINTF -H$HOUT $i >>$TOUT)2>&1;;
		esac
	done
	if [ "$LLIB" ]
	then
		cp $TOUT $LLIB
	fi
	if [ "$DEFL" ]
	then
		cat <$DEFL >>$TOUT
	fi
	if [ -s "$HOUT" ]
	then
		$LDIR/lint2 -T$TOUT -H$HOUT $LINTF
	else
		$LDIR/lint2 -T$TOUT $LINTF
	fi
fi
rm -f $TOUT $HOUT
