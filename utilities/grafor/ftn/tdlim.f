	subroutine tdlim (x,y,z,nrow,ncol,ista,ifn,jst,jfn,s)
C       subroutine tdlim 01/09/78
	real z (nrow,ncol), x (ncol), y (nrow)
	common /gfcrd/ xp,yp,zp /gflim/ xmin,xmax,zmin,zmax
c
	ist = iabs (ista)
	if (ifn.lt.ist) return
	if (jfn.lt.jst) return
	call hcncrd (x (jst), y (ist), z (ist,jst))
	xmx = xp
	xmn = xp
	zmn = zp
	zmx = zp
	do 20 i = ist,ifn
		do 10 j = jst,jfn
			call hcncrd (x (j), y (i), z (i,j))
			if (zmn.gt.zp) zmn = zp
			if (zmx.lt.zp) zmx = zp
			if (xmn.gt.xp) xmn = xp
			if (xmx.lt.xp) xmx = xp
  10                    continue
  20            continue
	if (ista.ge.0) go to 30
		xmn = amin1 (xmn, xmin)
		xmx = amax1 (xmx, xmax)
		zmn = amin1 (zmn, zmin)
		zmx = amax1 (zmx, zmax)
  30    continue
	call limits (xmn,xmx,zmn,zmx)
	xmin = xmn
	xmax = xmx
	zmin = zmn
	zmax = zmx
	if (zmax.le.zmin) zmax = zmin+1
	s = (xmax-xmin) / (zmax-zmin)
	return
	end
