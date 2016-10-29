	dimension x(42),y(42),a(42,42),xf1(84),yf1(84)
	dimension rmax(500),rmin(500)
	data xa,ya,xb,yb/0.525,1.025,2*1.525/
	data x(1),y(1),x(2),y(2)/-0.01,-0.01,0.,0./
	do 85 i=3,42
	x(i)=x(i-1)+0.05
85	y(i)=x(i)
	do 86 k=1,42
	a(1,k)=-0.3
86	a(k,1)=-0.3
	do 87 j=2,42
	do 87 i=2,42
87	a(i,j)=.2*(1./sqrt((x(i)-xa)**2+(y(j)-ya)**2)+
     *       2./sqrt((x(i)-xb)**2+(y(j)-yb)**2))
	call setpen(3)
	call page(32.,22.,'SURFACE',7,1)
	call region(1.,1.,30.,20.,0,0,0)
	call setpen(5)
	call mnmx(-15.,-20.,10.,x,y,a,42,42,1,42,1,42,s)
	call prsp(-15.,-20.,10.,x,y,a,42,42,1,42,1,42,xf1,yf1,
     *       rmax,rmin,500)
	call endpg(0)
	end
