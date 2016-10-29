:
#
# Mail uuencoded file to specified address.
# If file is too large, split it into smaller pieces.
# Usage:
#       uusplit [-d] [-#] [-o ###] [-compress] [-freeze] [-lharc] [-zoo]
#               [-zip] [-gzip] [-nopack] [-uuencode] [-btoa] [-ship]
#               [-split] [-bsplit] [-p cmd] address files...
# Flags:
#       -compress       - use `tar | compress', default
#       -gzip           - use `tar | gzip'
#       -freeze         - use `tar | freeze'
#       -lharc          - use `lharc' archiver
#       -zoo            - use `zoo' archiver
#       -zip            - use `zip' archiver
#       -nopack         - don't pack files
#       -uuencode       - use `uuencode' encoder, default
#       -btoa           - use `btoa' encoder
#       -ship           - use `ship' encoder
#       -split          - encode file, then split into pieces, default
#       -bsplit         - split binary file, then encode each piece separately
#       -d              - don't send directories
#       -#              - maximum piece length in kbytes
#       -o #            - list of pieces to send
#       -a archive      - archive to extract
#       -p cmd          - protocol command to execute
#
# Author: Serge Vakulenko, vak@kiae.su
#

# Archiver           Encoder
# ------------------------------------
# tar | compress  |  uuencode | split
# tar | freeze    |  btoa | split
# lharc           |  bsplit | uuencode
# zoo             |  bsplit | btoa
# zip             |  ship
# tar | gzip      |
# none            |

PATH=/var/spool/mailserv/bin:/usr/local/bin:/usr/ucb:/usr/bin:/bin:/usr/sbin:/sbin
export PATH

arcname="Kiarchive"
protocol=:
kmax=30
dflag=0
arfile=
archiver=compress
encoder=uuencode
split=split
olist=

while [ $# -ge 1 ]
do
	case "$1" in
	-d)
		dflag=1
		shift
		;;
	-[0-9]*)
		kmax=`echo $1 | tr -d -`
		shift
		;;
	-o)
		shift
		olist=$1
		shift
		;;
	-a)
		shift
		arfile=$1
		if [ ! -f $arfile ]
		then
			echo "Archive file $arfile does not exist"
			exit 1
		fi
		shift
		;;
	-p)
		shift
		protocol=$1
		if [ ! -f $protocol ]
		then
			protocol=:
		fi
		shift
		;;
	-compress)      archiver=compress;      shift ;;
	-gzip)          archiver=gzip;          shift ;;
	-freeze)        archiver=freeze;        shift ;;
	-lharc)         archiver=lharc;         shift ;;
	-zoo)           archiver=zoo;           shift ;;
	-zip)           archiver=zip;           shift ;;
	-nopack)        archiver=none;          shift ;;
	-uuencode)      encoder=uuencode;       shift ;;
	-btoa)          encoder=btoa;           shift ;;
	-ship)          encoder=ship;           shift ;;
	-split)         split=split;            shift ;;
	-bsplit)        split=bsplit;           shift ;;
	*)
		break;
		;;
	esac
done

if [ $# -lt 2 ]
then
	echo "Usage:"
	echo "      uusplit [-d] [-#] [-o ###] [-compress] [-freeze] [-lharc] [-zoo]"
	echo "              [-zip] [-gzip] [-nopack] [-uuencode] [-btoa] [-ship]"
	echo "              [-split] [-bsplit] [-p cmd] address files..."
	echo "Flags:"
	echo "      -d        - don't send directories"
	echo "      -#        - maximum piece length in kbytes, default $kmax"
	echo "      -a archive - archive to extract from"
	echo "      -compress - use 'tar | compress', default"
	echo "      -gzip     - use 'tar | gzip'"
	echo "      -freeze   - use 'tar | freeze'"
	echo "      -lharc    - use 'lharc' archiver"
	echo "      -zoo      - use 'zoo' archiver"
	echo "      -zip      - use 'zip' archiver"
	echo "      -nopack   - don't pack files"
	echo "      -uuencode - use 'uuencode' encoder, default"
	echo "      -btoa     - use 'btoa' encoder"
	echo "      -ship     - use 'ship' encoder"
	echo "      -split    - encode file, then split into pieces, default"
	echo "      -bsplit   - split binary file, then encode each piece separately"
	echo "      -p cmd    - protocol command to execute"
	exit 1
fi

address="$1"
shift

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

btemp=/tmp/s$$b
temp=/tmp/s$$f
tdir=/tmp/s$$d
tadir=/tmp/s$$a
rm -rf $temp $btemp* $tdir $tadir

bmax=`expr $kmax \* 1024`

wd=`pwd 2>/dev/null`
case $wd in
/*)     ;;
*)      echo "Cannot compute name of current directory"
	exit 1
	;;
esac

trap "rm -rf $temp $btemp* $tdir $tadir; exit 1" 1 2 3 15

filelist=
for file
do
	filelist="$filelist $file"
done

if [ x$arfile != x ]
then
	#
	# Extract files from archive.
	#

	mkdir $tadir

	filelist=
	for file
	do
		case "$file" in
		/*)
			echo "Cannot handle absolute path: $file"
			;;
		*)
			filelist="$filelist $file"
			;;
		esac
	done

	if [ x"$filelist" = x ]; then
		rm -rf $tadir
		exit 1
	fi

	if [ x"$1" = x"*" ]; then
		filelist=
	fi

	cd $tadir

	case $arfile in
	/*)     absfile=$arfile;;
	*)      absfile=$wd/$arfile;;
	esac

	case $arfile in
	*.cpio)         cpio -icd < $absfile $filelist >/dev/null 2>&1;;
	*.cpio.Z)       zcat $absfile | cpio -icd $filelist >/dev/null 2>&1;;
	*.cpio.gz)      gzip -d -c $absfile | cpio -icd $filelist >/dev/null 2>&1;;
	*.cpio.F)       fcat $absfile | cpio -icd $filelist >/dev/null 2>&1;;
	*.ar)           ar x $absfile $filelist;;
	*.ar.Z)         zcat $absfile > $temp; ar x $temp $filelist; rm $temp;;
	*.ar.gz)        gzip -d -c $absfile > $temp; ar x $temp $filelist; rm $temp;;
	*.ar.F)         fcat $absfile > $temp; ar x $temp $filelist; rm $temp;;
	*.tar)          tar xf $absfile $filelist;;
	*.tar.Z|*.taz)  zcat $absfile | tar xf - $filelist;;
	*.tar.gz|*.tgz) gzip -c -d $absfile | tar xf - $filelist;;
	*.tar.F)        fcat $absfile | tar xf - $filelist;;
	*.zoo)          zoo xq $absfile $filelist;;
	*.zip)          unzip -xdq $absfile $filelist;;
	*.lzh)          lharc xq $absfile $filelist;;
	*)              echo "Unknown type of archive $arfile"
			rm -rf $tadir
			exit 1
			;;
	esac

	if [ x"$1" = x"*" ]; then
		case $arfile in
		*.cpio)         remotebase=`basename $arfile .cpio`;;
		*.cpio.Z)       remotebase=`basename $arfile .cpio.Z`;;
		*.cpio.F)       remotebase=`basename $arfile .cpio.F`;;
		*.ar)           remotebase=`basename $arfile .ar`;;
		*.ar.Z)         remotebase=`basename $arfile .ar.Z`;;
		*.ar.F)         remotebase=`basename $arfile .ar.F`;;
		*.tar)          remotebase=`basename $arfile .tar`;;
		*.tar.Z)        remotebase=`basename $arfile .tar.Z`;;
		*.tar.F)        remotebase=`basename $arfile .tar.F`;;
		*.zoo)          remotebase=`basename $arfile .zoo`;;
		*.zip)          remotebase=`basename $arfile .zip`;;
		*.lzh)          remotebase=`basename $arfile .lzh`;;
		esac

		filelist=.
	fi
	dflag=0
fi

for file in $filelist
do
	if [ ! -r "$file" ]
	then
		echo "No such file:" `basename "$file"`
		continue
	fi
	if [ ! -f "$file" -a ! -d "$file" ]
	then
		echo "Cannot send nonregular files:" `basename "$file"`
		continue
	fi

	if [ $dflag = 1 -a -d "$file" ]
	then
		echo "Cannot send directories:" `basename "$file"`
		continue
	fi

	if [ -d "$file" ]; then
		#
		# Archive directory.
		#
		case $archiver in
		compress)
			if [ "$file" = . ]; then
				remotename="$remotebase".tar.Z
			else
				remotename=`basename "$file"`.tar.Z
			fi
			binfile=$btemp
			tar cf - "$file" | compress -c > $binfile
			;;
		gzip)
			if [ "$file" = . ]; then
				remotename="$remotebase".tgz
			else
				remotename=`basename "$file"`.tgz
			fi
			binfile=$btemp
			tar cf - "$file" | gzip > $binfile
			;;
		freeze)
			if [ "$file" = . ]; then
				remotename="$remotebase".tar.F
			else
				remotename=`basename "$file"`.tar.F
			fi
			binfile=$btemp
			tar cf - "$file" | freeze > $binfile
			;;
		zoo)
			if [ "$file" = . ]; then
				remotename="$remotebase".zoo
			else
				remotename=`basename "$file"`.zoo
			fi
			binfile=$btemp.zoo
			rm -f $binfile
			find "$file"/. -type f -print | sed 's;/\./;/;' | zoo aqI $binfile
			;;
		zip)
			if [ "$file" = . ]; then
				remotename="$remotebase".zip
			else
				remotename=`basename "$file"`.zip
			fi
			binfile=$btemp.zip
			rm -f $binfile
			zip -rq $binfile "$file"
			;;
		lharc)
			if [ "$file" = . ]; then
				remotename="$remotebase".lzh
			else
				remotename=`basename "$file"`.lzh
			fi
			binfile=$btemp.lzh
			rm -f $binfile
			lharc aq $binfile "$file"
			;;
		none)
			echo "Cannot send directories without archiver: $file"
			continue
			;;
		*)
			echo "Bad archiver: $archiver"
			continue
		esac
	else
		case "$file" in
		*.Z | *.F | *.gif | *.jpeg | *.shar | *.zip |\
		*.lha | *.tgz | *.arc | *.lzh | *.zoo | *.arj | *.dbz)
			#
			# Don't archive packed files.
			#
			binfile="$file"
			remotename=`basename "$file"`
			;;
		*)
			#
			# Archive regular file.
			#
			case $archiver in
			compress)
				remotename=`basename "$file"`.Z
				binfile=$btemp
				compress -c < "$file" > $binfile
				;;
			gzip)
				remotename=`basename "$file"`.gz
				binfile=$btemp
				gzip < "$file" > $binfile
				;;
			freeze)
				remotename=`basename "$file"`.F
				binfile=$btemp
				freeze < "$file" > $binfile
				;;
			zoo)
				remotename=`basename "$file"`.zoo
				binfile=$btemp.zoo
				rm -f $binfile
				zoo aq $binfile "$file"
				;;
			zip)
				remotename=`basename "$file"`.zip
				binfile=$btemp.zip
				rm -f $binfile
				zip -q $binfile "$file"
				;;
			lharc)
				remotename=`basename "$file"`.lzh
				binfile=$btemp.lzh
				rm -f $binfile
				lharc aq $binfile "$file"
				;;
			none)
				remotename=`basename "$file"`
				binfile="$file"
				;;
			*)
				echo "Bad archiver: $archiver"
				continue
			esac
			;;
		esac
	fi

	#
	# Compute size of $binfile in bytes.
	#
	size=`filesize $binfile`
	if [ $encoder = uuencode ]; then
		tsize=`expr $size \* 4 / 3`
	else
		tsize=`expr $size \* 6 / 5`
	fi

	if [ $size -le $bmax ]
	then
		#
		# Don't split $binfile, just encode it.
		#
		case $encoder in
		uuencode)
			uuencode $binfile `basename "$remotename"` > $temp
			;;
		btoa)
			btoa -o < $binfile > $temp
			;;
		ship)
			ship -q < $binfile > $temp
			;;
		*)
			echo "Bad encoder: $1"
			rm -f $btemp*
			continue
			;;
		esac

		dir=`dirname $file`
		file=`(cd $dir; /bin/pwd)`/`basename $file`
		file=`echo $file | sed 's=^/ar/pub/=/='`
		file=`echo $file | sed 's=^/\.[0-9][^/]*/=/='`
		mail -s "$arcname: $remotename ($encoder)" $address < $temp
		echo "File $remotename sent, $size bytes"
		$protocol send file "$file" to "$address", $size bytes, $tsize total
		continue
	fi

	#
	# Encode $binfile and split it into pieces.
	#
	rm -rf $tdir
	mkdir $tdir

	if [ $encoder = ship ]; then
		#
		# Ship encodes file and splits it.
		#

		limit=$size
		while [ $limit -gt $bmax ]
		do
			limit=`expr $limit + 1`
			limit=`expr $limit / 2`
		done
		limit=`expr $limit + 1023`
		limit=`expr $limit / 1024`

		(cd $tdir; ship -q -$limit) < $binfile
		npacks=`(cd $tdir; echo part*) | $awk '{ print NF }'`
	elif [ $split = bsplit ]; then
		if [ $encoder = uuencode ]; then
			max=`expr $bmax \* 3 / 4`
		else
			max=`expr $bmax \* 5 / 6`
		fi
		limit=$size
		while [ $limit -gt $max ]
		do
			limit=`expr $limit + 1`
			limit=`expr $limit / 2`
		done
		npacks=`expr $size + $limit - 1`
		npacks=`expr $npacks / $limit`

		/usr/spool/mailserv/bin/bsplit -$limit $binfile $tdir/bin
		for ext in $tdir/bin*
		do
			uu=`echo $ext | sed 's;/bin;/part;'`
			if [ $encoder = uuencode ]; then
				uun=`echo $ext | sed 's;.*/bin;part;'`
				uuencode $ext $uun > $uu
			else
				btoa -o < $ext > $uu
			fi
		done
		rm -f $tdir/bin*
	else
		if [ $encoder = uuencode ]; then
			uuencode $binfile `basename "$remotename"` > $temp
			max=`expr $bmax / 43`
		else
			btoa -o < $binfile > $temp
			max=`expr $bmax / 70`
		fi

		len=`wc -l $temp | $awk '{ print $1 }'`
		limit=$len
		while [ $limit -gt $max ]
		do
			limit=`expr $limit + 1`
			limit=`expr $limit / 2`
		done
		npacks=`expr $len + $limit - 1`
		npacks=`expr $npacks / $limit`
		split -$limit $temp $tdir/part
	fi

	n=01
	npackssent=0
	sizesent=0
	for ext in $tdir/part*
	do
		if [ x$olist != x ]; then
			mustmail=`echo $olist | $awk -F, -v n=$n '
			BEGIN { yes = 0; n += 0 }
			{
				for (i=1; i<=NF; ++i) {
					if ($i ~ "-") {
						split ($i, range, "-")
						if (range[1] == "")
							range[1] = 1
						if (range[2] == "")
							range[2] = 999999
						if (range[1] <= n && n <= range[2] ||
						    range[2] <= n && n <= range[1])
							yes = 1
					} else if ($i == n)
						yes = 1
					if (yes == 1) {
						print "yes"
						exit
					}
				}
			}
			END { if (yes == 0) print "no" }
			'`
		else
			mustmail=yes
		fi

		if [ $mustmail = yes ]; then
			mail -s "$arcname: $remotename, Part $n of $npacks ($encoder)" "$address" < $ext

			# Give a chance to sendmail!
			sleep 1
			npackssent=`expr $npackssent + 1`
			partsize=`filesize $ext`
			sizesent=`expr $sizesent + $partsize`
		fi
		n=`expr $n + 1 | $awk '{ printf "%02d\n", $1 }'`
	done
	rm -rf $tdir $temp $btemp*

	dir=`dirname $file`
	file=`(cd $dir; /bin/pwd)`/`basename $file`
	file=`echo $file | sed 's=^/ar/pub/=/='`
	file=`echo $file | sed 's=^/\.[0-9][^/]*/=/='`
	if [ $npackssent = 0 ]; then
		echo "Nothing to send: $file"
	elif [ $npackssent != $npacks ]; then
		echo "Package $remotename sent, $npackssent extents of $npacks, $size bytes, $tsize total"
		$protocol send package "$file" to "$address", $npackssent extents of $npacks, $size bytes, $tsize total
	else
		echo "Package $remotename sent, $npacks extents, $size bytes, $tsize total"
		$protocol send package "$file" to "$address", $npacks extents, $size bytes, $tsize total
	fi
done

cd /
rm -rf $temp $btemp* $tdir $tadir
