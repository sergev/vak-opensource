       subroutine condek(lx,ly,x,y,z,step,k0,n,c)
C... condek 01/09/78
      dimension       x(lx),y(ly),z(lx,ly),c(n)
      dimension root1(10),root2(10),root(10)
           dimension work(10)
      common/drinfn/in,xy,cont,k
      common /drin/ nexits,maxnst,ubx,vbx,uby,vby,xstep1,ystep1,
     *nroots,root,alfa,sx,sy,ks1,s2,imark,s0
       call where(s0,s1,s2)
      ks1=0.3*s2/step
      imark=0
      k=k0
      xlx=x(lx)
      yly=y(ly)
      h=xlx-x(1)
      h1=yly-y(1)
       call tfm(0.,0.,satx,saty)
      call tfm(step,step,sx,sy)
       sx=sx-satx
       sy=sy-saty
      epsx=0.1*sx
      epsy=0.1*sy
      uby=y(1)-epsy
      vby=yly+epsy
      maxnst=2*(h/sx+h1/sy)
      alfa=(sx+sy)*2.5
      h=h/k
      do  2  i=1,n
      if(i.eq.10) ks1=1.5*ks1
      kol=mod(i,2)+1
       s0=i
      xmax=xlx+h
      cont=c(i)
      j1=1
      j2=1
      in=0
         xy=yly
      call fnroot(lx,ly,x,y,z,n1,root1)
          xy=y(1)
      call fnroot(lx,ly,x,y,z,n2,root2)
      in=1
      xy=xlx
       call fnroot(lx,ly,x,y,z,nwork,work)
      do 2 l=1,k
      xmax=xmax-h
      xmin=xmax-h
      ubx=xmin-epsx
      vbx=xmax+epsx
      nexits=0
       nroots=nwork
       do 54 kzh=1,nroots
       root(kzh)=work(nwork)
54     nwork=nwork-1
      xy=xmax
       in=1
      xstep1=-sx
      ystep1=0.
      if(l.ne.kol) go to 21
      imark=1
      kol=kol+k/6+1
21    continue
       call insdek(lx,ly,x,y,z)
      nroots=0
10        if(n1.eq.0.or.j1.gt.n1) goto 60
6     if(root1(j1).lt.xmin) go to 3
        nroots=nroots+1
      root(nroots)=root1(j1)
      j1=j1+1
      if(j1.le.n1) go to 6
3      xy=yly
      in=0
      xstep1=0.
       ystep1=-sy
       call insdek(lx,ly,x,y,z)
       goto 60
70           continue
       nroots=0
1     if(n2.eq.0.or.j2.gt.n2) go to 9
8     if(root2(j2).lt.xmin) go to 5
        nroots=nroots+1
       root(nroots)=root2(j2)
      j2=j2+1
      if(j2.le.n2) go to 8
5      xy=y(1)
      in=0
      xstep1=0.
         ystep1=sy
       call insdek(lx,ly,x,y,z)
        go to 2
60      xy=xmin
      in=1
      call fnroot(lx,ly,x,y,z,nroots,root)
       nwork=nroots
       do 340 kzh=1,nroots
340    work(kzh)=root(kzh)
      xstep1=sx
      ystep1=0.
       call insdek(lx,ly,x,y,z)
        go to 70
9      continue
 2    continue
      return
      end
