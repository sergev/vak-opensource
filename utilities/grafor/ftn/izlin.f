	subroutine izlin(z,m,n,x,y,zmi,ziz,niz,nx,ny,kx,ky,xi,yi,nl)
	dimension x(m),y(n)
	dimension z(m,n),xi(nl),yi(nl)
	nline=nl-1
	kind=0
	zizol=ziz+zmi
	ki=kx+1
	kj=ky+1
	mm=kx-1
	nn=ky-1
	ngi=nx-1
	ngj=ny-1
	ny1=ny+1
	nx1=nx+1
	jn=ny1
	il=0
	ili=70
	i=nx
c(((((  do 70 l=ny,nn
	l = ny - 1
   70           continue
		l = l + 1
c)))))
		j=l
		if(z(i,j).lt.0.) go to 80
		if((abs(z(i,j+1)).gt.zizol).and.(zizol.ge.z(i,j)))
     *                  go to 38
   80           if((z(i,j+1).gt.zizol).and.(zizol.ge.abs(z(i,j))))
     *                  go to 38
c(((((  70      continue
		if (l.lt.nn) goto 70
c)))))
	ili=71
	j=ky
c(((((  do 71 l=nx,mm
	l = nx - 1
   71           continue
		l = l + 1
c)))))
		i=l
		if(z(i,j).lt.0.) go to 81
		if((abs(z(i+1,j)).gt.zizol).and.(zizol.ge.z(i,j)))
     *                  go to 39
   81           if((z(i+1,j).gt.zizol).and.(zizol.ge.abs(z(i,j))))
     *                  go to 39
c(((((  71      continue
		if (l.lt.mm) goto 71
c)))))
	ili=72
	i=kx
c(((((  do 72 l=ny1,ky
	l = ny1 - 1
   72           continue
		l = l + 1
c)))))
		j=l
		if(z(i,j).lt.0.) go to 82
		if((abs(z(i,j-1)).gt.zizol).and.(zizol.ge.z(i,j)))
     *                  go to 36
   82           if((z(i,j-1).gt.zizol).and.(zizol.ge.abs(z(i,j))))
     *                  go to 36
c(((((  72      continue
		if (l.lt.ky) goto 72
c)))))
	ili=73
	j=ny
c(((((  do 73 l=nx1,kx
	l = nx1 - 1
   73           continue
		l = l + 1
c)))))
		i=l
		if(z(i,j).lt.0.) go to 83
		if((abs(z(i-1,j)).gt.zizol).and.(zizol.ge.z(i,j)))
     *                  go to 37
   83           if(( z(i-1,j).gt.zizol).and.(zizol.ge.abs(z(i,j))))
     *                  go to 37
c(((((  73      continue
		if (l.lt.kx) goto 73
c)))))
   12   ili=0
	do 1 j=jn,nn
	do 1 i=nx,mm
		if (z(i,j).lt.0.) go to 11
		if((abs(z(i+1,j)).gt.zizol).and.(zizol.ge.z(i,j)))
     *                  go to 4
   11           if((z(i+1,j).gt.zizol).and.(zizol.ge.abs(z(i,j))))
     *                  go to 4
    1           continue
	call recur (z,m,n,nx,ny,kx,ky)
	return
    4   in=i
	jn=j
   49   if((zizol.ge.abs(z(i,j))).and.(abs(z(i+1,j)).gt.zizol)) go to 39
	i=i+1
	if (i.eq.ki) go to 50
   48   if((zizol.ge.abs(z(i,j))).and.(abs(z(i,j+1)).gt.zizol)) go to 38
	j=j+1
	if (j.eq.kj) go to 50
   47   if((zizol.ge.abs(z(i,j))).and.(abs(z(i-1,j)).gt.zizol)) go to 37
	i=i-1
	if (i.eq.ngi) go to 50
   46   if((zizol.ge.abs(z(i,j))).and.(abs(z(i,j-1)).gt.zizol)) go to 36
	j=j-1
	if (j.eq.ngj) go to 50
	if((i.eq.in).and.(j.eq.jn)) go to 50
	go to 49
   39   if (il.ge.nline) go to 50
	il=il+1
	xi(il)=xiliy (z(i+1,j),z(i,j),x(i+1),x(i),zizol,i,j,1,0)
	yi(il)=y(j)
	j=j-1
	if((i.eq.in).and.(j.eq.jn)) go to 50
	if (j.eq.ngj) go to 50
	go to 49
   38   if (il.ge.nline) go to 50
	il=il+1
	yi(il)= xiliy(z(i,j+1),z(i,j),y(j+1),y(j),zizol,i,j,0,1)
	xi(il)=x(i)
	i=i+1
	if (i.eq.ki) go to 50
	go to 48
   37   if (il.ge.nline) go to 50
	il=il+1
	xi(il)= xiliy(z(i-1,j),z(i,j),x(i-1),x(i),zizol,i,j,-1,0)
	yi(il)=y(j)
	j=j+1
	if (j.eq.kj) go to 50
	go to 47
   36   if (il.ge.nline) go to 50
	il=il+1
	yi(il)= xiliy(z(i,j-1),z(i,j),y(j-1),y(j),zizol,i,j,0,-1)
	xi(il)=x(i)
	i=i-1
	if (i.eq.ngi) go to 50
	go to 46
  50    if(ili.eq.0) kind=1
	call letizo(xi,yi,il+kind,ziz,niz,kind)
	il=0
	if(ili.eq.0) go to 12
	i=nx
	if(ili.eq.70) go to 70
	j=ky
	if(ili.eq.71) go to 71
	i=kx
	if(ili.eq.72) go to 72
	j=ny
	if(ili.eq.73) go to 73
	go to 12
	end
