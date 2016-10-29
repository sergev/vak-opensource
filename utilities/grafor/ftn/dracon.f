       subroutine dracon(lx,ly,x,y,z,step,k,n,c,funx,funy)
C...01/09/78
		external funx,funy
       dimension x(lx),y(ly),z(lx,ly),c(n)
       dimension root1(10),root2(10),root(10),work(10),alfa(2)
       common /drinfn/in,xy,cont,lxm1
       common /drin/nexits,maxnst,ubx,vbx,uby,vby,xstep1,ystep1,nroots,
     1root,alfa,sx,sy,ks1,s2,imark,s0
      lxm1=lx-1
      call where(s0,s1,s2)
      ks1=0.3*s2/step
      imark=0
      lxmk=lx-k
      yly=y(ly)
      do 2 i=1,n
      if(i.eq.10)
     -ks1=1.5*ks1
      kol=mod(i,2)+1
      s0=i
      cont=c(i)
      in=0
      xy=yly
      call fnroot(lx,ly,x,y,z,n1,root1)
      xy=y(1)
      call fnroot(lx,ly,x,y,z,n2,root2)
      in=1
      xy=x(1)
      j1=n1
      j2=n2
      call fnroot(lx,ly,x,y,z,nwork,work)
      do 2 l=1,lxmk,k
      lpk=l+k
      if(l.gt.lxmk)
     -lpk=lx
      xl=x(l)
      xlpk=x(lpk)
      sx0=funx(xl,y(1))
      sy0=funy(xl,y(1))
      sx1=funx(xlpk,y(1))
      sy1=funy(xlpk,y(1))
      sx2=funx(xlpk,yly)
      sy2=funy(xlpk,yly)
      sx3=funx(xl,yly)
      sy3=funy(xl,yly)
      call tmf(sx0,sy0,sxf0,syf0)
      call tmf(sx1,sy1,sxf1,syf1)
      call tmf(sx2,sy2,sxf2,syf2)
      call tmf(sx3,sy3,sxf3,syf3)
      dx01=sxf0-sxf1
      dx23=sxf2-sxf3
      dx03=sxf0-sxf3
      dx12=sxf1-sxf2
      dy01=syf0-syf1
      dy23=syf2-syf3
      dy03=syf0-syf3
      dy12=syf1-syf2
       xk=0.5*(sqrt(dx01*dx01+dy01*dy01)+sqrt(dx23*dx23+
     +dy23*dy23))/step
      yk=0.5*(sqrt(dx03*dx03+dy03*dy03)+sqrt(dx12*dx12+
     +dy12*dy12))/step
      sx=(xlpk-xl)/xk
      sy=(yly-y(1))/yk
      epsx=0.1*sx
      epsy=0.1*sy
      uby=y(1)-epsy
      vby=yly+epsy
      ubx=xl-epsx
      vbx=xlpk+epsx
      maxnst=2.*(xk+yk)
      alfa(1)=sx*2.5
      alfa(2)=sy*2.5
      nexits=0
      nroots=nwork
      do 54 kzh=1,nroots
      root(kzh)=work(nwork)
54    nwork=nwork-1
      in=1
      xy=xl
      xstep1=sx
      ystep1=0.
      if(l.ne.kol)
     - go to 21
      imark=1
      kol =kol+lx/6+1
21    continue
      call inside(lx,ly,x,y,z,funx,funy)
      nroots=0
10     if(j1.lt.1)
     - go to 3
       if(root1(j1).gt.xlpk) goto 3
       nroots=nroots+1
      root(nroots)=root1(j1)
      j1=j1-1
      go to 10
3     xy=yly
      in=0
      xstep1=0.
      ystep1=-sy
      call inside(lx,ly,x,y,z,funx,funy)
      go to 60
70    continue
      nroots=0
1     if(j2.lt.1)
     -go to 5
       if(root2(j2).gt.xlpk) goto 5
       nroots=nroots+1
      root(nroots)=root2(j2)
      j2=j2-1
      go to 1
5     xy=y(1)
      in=0
      xstep1=0.
      ystep1=sy
      call inside(lx,ly,x,y,z,funx,funy)
      go to 2
60    xy=xlpk
      in=1
      call fnroot(lx,ly,x,y,z,nroots,root)
      nwork=nroots
      do 340 kzh=1,nroots
340   work(kzh)=root(kzh)
      xstep1=-sx
      ystep1=0.
      call inside(lx,ly,x,y,z,funx,funy)
      go to 70
2     continue
      return
      end
