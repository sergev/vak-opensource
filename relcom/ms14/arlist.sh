:
#
# List contents of archives.
# Usage:
#       arlist archive...
#

PATH=/usr/local/bin:/usr/ucb:/usr/bin:/bin:/etc
export PATH

t=/tmp/al$$

for ar
do
	bar=`basename $ar`
	if [ ! -f $ar ]; then
	       echo "! No such file: $bar"
	       continue
	fi
	echo
	echo "File $bar:"
	case $bar in
	*.cpio)         cpio -itvc < $ar;;
	*.cpio.Z)       zcat $ar | cpio -itvc;;
	*.cpio.gz)      gzip -d < $ar | cpio -itvc;;
	*.cpio.F)       fcat $ar | cpio -itvc;;
	*.ar)           ar tv $ar;;
	*.ar.Z)         zcat $ar > $t; ar tv $t; rm $t;;
	*.ar.gz)        zcat $ar > $t; ar tv $t; rm $t;;
	*.ar.F)         fcat $ar > $t; ar tv $t; rm $t;;
	*.tar)          tar tvf $ar;;
	*.tar.Z)        zcat $ar | tar tvf -;;
	*.taz)          zcat < $ar | tar tvf -;;
	*.tar.F)        fcat $ar | tar tvf -;;
	*.tar.gz)       gzip -d < $ar | tar tvf -;;
	*.tgz)          gzip -d < $ar | tar tvf -;;
	*.zoo)          zoo l $ar;;
	*.zip)          unzip -l $ar;;
	*.lzh)          lharc v $ar;;
	*.arc)          arc l $ar;;
	*.arj)          unarj l $ar;;
	*)              echo "Unknown type of archive $bar";;
	esac
	echo
done
