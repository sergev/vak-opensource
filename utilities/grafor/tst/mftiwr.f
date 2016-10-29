	parameter (nx=80, ny=40, nm=1000, nl=7)
	parameter (nb=nx*2+ny*2-3)
	integer ix (nb), iy (nb), iarea (nx,ny)
	common z (nx,ny), xm (nm), ym (nm), xx (nx), yy (ny), cont (nl)

	call setmtf ('/tmp/mtf', 8)
	call setpen (4)
	call page (30.,22.,'DEMOS',5,1)
	call notch (xm,ym,nm,nt,-0.02,0,1)
	call symbol (0.,0.,1.,'DEMOS',5,0.)
	call rentch
	call endpg (' ')
	do 10 i=2,nt
		xm (i-1) = xm (i)
		ym (i-1) = ym (i)
10              continue
	nt = nt - 2
	call points (xm,ym,nt)
	call minmax (xm,nt,x1,x2)
	call minmax (ym,nt,y1,y2)
	x0 = (x2-x1)/5
	y0 = (y2-y1)/2
	x1 = x1-x0
	x2 = x2+x0
	y1 = y1-y0
	y2 = y2+y0
	do 70 jx=1,nx
		x = (x2-x1)*(jx-1)/nx+x1
		xx(jx) = x
		do 71 jy=1,ny
			y = (y2-y1) * (jy-1) / ny + y1
			yy (jy) = y
			r = (x - xm(1)) * (x - xm(1)) +
     *                          (y - ym(1)) * (y - ym(1))
			do 72 j=2,nt
				dx = x - xm(j)
				dx = dx * dx
				if (dx.gt.r) goto 72
				dy = y - ym(j)
				dy = dy * dy
				if (dy.gt.r) goto 72
				r0 = dx + dy
				if (r0.lt.r) r = r0
72                              continue
			z (jx,jy) = 1. / (1 + 100*r)
71                      continue
70              continue
	open (41, file='mfti.dat', status='new')
	write (41, *) x1, x2, y1, y2, xx, yy, z
	close (41)
	end


	subroutine points (x,y,n)
	real x (n), y (n)
	if (j.eq.1) return
	k = 0
	do 100 j=1,n
C		print *,x(j),y(j)
		if (x(j+1).ge.0.or.x(j).gt.0.0)then
			if (x(j).eq.0..and.y(j).eq.0..and.
     *                          x(j+1).eq.0..and.y(j+1).eq.0.)
     *                          go to 100
			k = k+1
			x(k) = abs(x(j))
			y(k) = abs(y(j))
		end if
100	continue	
	if (x(n+1).ge.0.0)then
		k = k+1
		x(k) = x(n+1)
		y(k) = y(k+1)
	end if
	n = k
	return
	end


	subroutine move (x,y,j)
	call move2 (x,y,j)
	return
	end
