	subroutine blanc (x, y, n, in)
	real x (n), y (n)
	common /gfatrn/ aa1, bb1, cc1, aa2, bb2, cc2, is
	common /gfblan/ jj, bl1 (4), ncop, jch, kch, bl2 (2), ich
	common /gfblu/  ibp (16, 4), bp (16, 6), bl (6)
	jj = 1
	kch = kch+1
	if (kch.lt.17) goto 1
	do 4 i=1,6
		do 7 j=1,15
			if (i.le.4) ibp (j,i) = ibp (j+1,i)
  7                     bp (j,i) = bp (j+1,i)
  4             continue
	kch = 16
	call gffals (3)
    1   ibp (kch,1) = iadr (x)
	ibp (kch,2) = iadr (y)
	ibp (kch,3) = n
	ibp (kch,4) = in
	bp (kch,1) = aa1
	bp (kch,2) = bb1
	bp (kch,3) = cc1
	bp (kch,4) = aa2
	bp (kch,5) = bb2
	bp (kch,6) = cc2
	return
	end
