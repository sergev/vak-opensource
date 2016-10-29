:
# Compact the encoding table for Kiarchive HTTP daemon.
#
# Copyright (C) 1994 Cronyx Ltd.
# Author: Serge Vakulenko, <vak@zebub.msk.su>
# Date: Thu Mar 17 18:56:45 MSK 1994

awk '
BEGIN {
	cnt = 0
}
/^#/ || /^ *$/ {
	print
	next
}
{
	if ($2 != ">") {
		print
		next
	}
	if (o1+cnt == $1 && o3+cnt == $3)
		++cnt
	else {
		if (cnt > 1)
			print o1 "-" o1+cnt-1 " > " o3 "-" o3+cnt-1
		else if (cnt > 0)
			print o1 " > " o3
		o1 = $1
		o3 = $3
		cnt = 1
	}
}
END {
	if (cnt > 1)
		print o1 "-" o1+cnt-1 " > " o3 "-" o3+cnt-1
	else if (cnt > 0)
		print o1 " > " o3
}' $*
