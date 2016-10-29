	parameter(nx=200,ny=50,nm=1000,ngrd=300)
	parameter(hpen=0.03,al=1.)
	parameter(na=2*nx,nam=2*ngrd)
	real ar(na),am(nam)
	common z(nx,ny),xm(nm),ym(nm),xx(nx),yy(ny)
	integer jdx (9), jdy (9)
	real dz (9)
	data jdx /-1, -1, -1, 0, 0, 0, 1, 1, 1/
	data jdy /-1, 0, 1, -1, 0, 1, -1, 0, 1/
	data dz /1., 1., 1., 1., 1., 1., 1., 1., 1./

	call init
c       call dimet
	call view (20., -10., 20.)
	call setpen(3)
	call page (32.,22.,'DEMOS/*',7,1)
	call notch(xm,ym,nm,nt,-hpen,0,1)
	call symbol(0.,0.,1.,'DEMOS/*',7,0.)
	call rentch
c       write (*,*)' nt= ',nt
	do 10 i=2,nt
		xm (i-1) = xm (i)
		ym (i-1) = ym (i)
10              continue
	nt = nt - 2
	call points(xm,ym,nt)
	do 69 j=1,nt
		ym(j) = 2. - 2. * ym(j)
69      continue
	call minmax(xm,nt,x1,x2)
	call minmax(ym,nt,y1,y2)
	x0=(x2-x1)/5
	y0=(y2-y1)/5
	x1=x1-x0
	x2=x2+x0
	y1=y1-y0
	y2=y2+y0
	hx=(x2-x1)/nx
	hy=(y2-y1)/ny
	do 70 jx=1,nx
		xx(jx) = (x2-x1)*(jx-1)/nx+x1
		do 71 jy=1,ny
			yy(jy) = (y2-y1)*(jy-1)/ny+y1
			z(jx,jy) = 0
71              continue
70      continue
	do 72 j=1,nt
		jx0=(xm(j)-x1)/hx+1
		jy0=(ym(j)-y1)/hy+1
		do 73 jj=1,9
			jx=jdx(jj)+jx0
			jy=jdy(jj)+jy0
			r=dz(jj)
			if(z(jx,jy).lt.r)then
				z(jx,jy)=r
			end if
73              continue
72      continue
	call tdlim(yy,xx,z,nx,ny,1,nx,1,ny,s)
c       write(*,*)'zmin=',zmin,'zmax=',zmax,'xmin=',xmin,'xmax='
c    *   ,xmax,'s=',s
	if(s-30./20. .gt. 0.)then
		xr=30.
		yr=20./s
	else
		yr=20.
		xr=yr*s
	end if
c       call rotate (xr/2+1,yr/2+1,36.)
	call region (1.,1.,xr,yr,0,0,0)
	call setpen (5)
	call threed (yy,xx,z,nx,ny,1,nx,1,ny,1,ngrd,am,ar)
	call endpg (0)
	end


	subroutine points(x,y,n)
	real x(n),y(n)
	if(j.eq.1) return
	k=0
	do 100 j=1,n
C		print *,x(j),y(j)
		if(x(j+1).ge.0.or.x(j).gt.0.0)then
		if(x(j).eq.0..and.y(j).eq.0..and.
     *                       x(j+1).eq.0..and.y(j+1).eq.0.) go to 100
			k=k+1
			x(k)=abs(x(j))
			y(k)=abs(y(j))
		end if
100     continue
	if(x(n+1).ge.0.0)then
		k=k+1
		x(k)=x(n+1)
		y(k)=y(n+1)
	end if
	n=k
	return
	end


	subroutine move(x,y,j)
	call move2(x,y,j)
	return
	end
