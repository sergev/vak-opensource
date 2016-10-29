	parameter (nx=80, ny=40, nm=1500, ngrd=100)
	parameter (hpen=0.01, al=0.3)
	parameter (na=2*nx, nam=2*ngrd)
	real ar (na), am (nam)
	common z (nx, ny), xm (nm), ym (nm), xx (nx), yy (ny)
	integer jdx (4), jdy (4)
	data jdx /0,0,1,1/
	data jdy /0,1,0,1/
c
	call init
	call isomet
	call setpen(3)
	call page(32.,22.,'DEMOS',5,1)
	call notch(xm,ym,nm,nt,-hpen,0,1)
	call symbol(0.,0.,1.,'DEMOS',5,0.)
	do 10 i=2,nt
		xm (i-1) = xm (i)
		ym (i-1) = ym (i)
10              continue
	nt = nt - 1
	call points(xm,ym,nt)
	call rentch
	do 20 j=1,nt
20              ym(j)=1.-ym(j)
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
	do 40 jx=1,nx
		x=(x2-x1)*(jx-1)/nx+x1
		xx(jx)=x
		do 30 jy=1,ny
			y=(y2-y1)*(jy-1)/ny+y1
			yy(jy)=y
			z(jx,jy)=(x2-x1)**2+(y2-y1)**2
30                      continue
40              continue
	do 60 j=1,nt
		jx0=(xm(j)-x1)/hx+1
		jy0=(ym(j)-y1)/hy+1
		do 50 jj=1,4
			jx=jdx(jj)+jx0
			jy=jdy(jj)+jy0
			r=(xx(jx)-xm(j))**2+(yy(jy)-ym(j))**2
			if(z(jx,jy).gt.r)then
				z(jx,jy)=r
			end if
50                      continue
60              continue
	do 80 jx=1,nx
		do 70 jy=1,ny
			z(jx,jy)=al/(1+200*z(jx,jy))
70                      continue
80              continue
	call tdlim(yy,xx,z,nx,ny,1,nx,1,ny,s)
	if(s.gt.30./20.)then
		xr=30.
		yr=30./s
	else
		yr=20.
		xr=yr*s
	end if
	call region(1.,1.,xr,yr,0,0,0)
	call setpen(5)
	call threed(yy,xx,z,nx,ny,1,nx,1,ny,1,ngrd,am,ar)
	call endpg(0)
	end


	subroutine points(x,y,n)
	real x(n),y(n)
	if(j.eq.1) return
	k=0
	do 100 j=1,n
		if(x(j+1).ge.0.or.x(j).gt.0.0)then
		if(x(j).eq.0..and.y(j).eq.0..and.
     *                          x(j+1).eq.0..and.y(j+1).eq.0.) go to 100
			k=k+1
			x(k)=abs(x(j))
			y(k)=abs(y(j))
		end if
100             continue
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
