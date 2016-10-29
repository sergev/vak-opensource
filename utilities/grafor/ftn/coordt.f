      subroutine coordt(dx,dy,dz,nlines,n1,n2,nclust,nlndr,ityp)
      common/gfmt/t(4,4),tt(4,4),tr(4,4),dpr1(3),itpprj
	common/gfcrdn/xp,yp,zp,dxcent,dycent,dzcent
      common/gfeps/eps,lengte
      dimension dx(2,nlines),dy(2,nlines),dz(2,nlines)
      dimension nclust(nlndr)
C------------- "nlndr=n2-n1+1", if ityp=3 - 8.
C  ===========================================================
C   set default value for length of array "te()" (historically).
C  ===========================================================
      lengte=15
      eps=1.e-8
C
      if(ityp.gt.2) goto 345
C----- "1" - ordinary; "2" - ordered lines.
2     continue
C  =============================================================
C     transform from 3d "world" to 3d "view plane" coord. system.
C      we change "dx,dy,dz" ||
C  =============================================================
      do 1 i=n1,n2
      do 1 j=1,2
      call hcncor(dx(j,i),dy(j,i),dz(j,i),t)
      dx(j,i)=xp
      dy(j,i)=yp
      dz(j,i)=zp
1     continue
C  =============================================================
C     sort end points of lines so as the end point with greater "dy"
C     value has the index "1".
C  =============================================================
      call sorty(dx,dy,dz,nlines,n1,n2)
C***  do 500 i=1,nlines
C***500   print 40,i,dx(1,i),dx(2,i),dy(1,i),dy(2,i),dz(1,i),dz(2,i)
      return
C
C
345   continue
C**   do 50 i=1,nlines
C**50    print 40,i,dx(1,i),dx(2,i),dy(1,i),dy(2,i),dz(1,i),dz(2,i)
C##40    format(' world c.',i6,6f18.8)
      if(ityp.gt.5) goto 18
C ==================================================================
C   calc.average dx and dy coords(the center of weight of"n1,n2"lines
C   in world coord.system.
C ==================================================================
      dxcent=0.
      dycent=0.
      dzcent=0.
      do 17 i=n1,n2
      do 17 j=1,2
      dxcent=dxcent+dx(j,i)
      dycent=dycent+dy(j,i)
17    dzcent=dzcent+dz(j,i)
      s=2.*(n2-n1+1)
      dxcent=dxcent/s
      dycent=dycent/s
      dzcent=dzcent/s
C
18    continue
C**   print 77,dxcent,dycent,dzcent,s,ityp
C##77    format(' dcent=',3f20.10,' s=',f8.2,' ityp=',i5)
C ==========================================
C       set transf.matrix for splitting.
C ==========================================
C.....test: parallel or perspective projection
      if(itpprj.eq.0) goto 30
C----persp.projection.
      dpr1(1)=dpr1(1)-dxcent
      dpr1(2)=dpr1(2)-dycent
      dpr1(3)=dpr1(3)-dzcent
30    continue
C---parallel proj.(and for persp.too).
      s=sqrt(dpr1(1)**2+dpr1(2)**2+dpr1(3)**2)
      if(abs(s).le.eps) goto 8
      dpr1(1)=dpr1(1)/s
      dpr1(2)=dpr1(2)/s
      dpr1(3)=dpr1(3)/s
C....translate the center of the object into origin of world c.syst.
      call hcinit(tt)
      tt(4,1)=-dxcent
      tt(4,2)=-dycent
      tt(4,3)=-dzcent
C....rotations:
      s=sqrt(dpr1(1)**2+dpr1(3)**2)
C    test: are dpr1(1) and dpr1(3) both equal to zero
      if(s.le.eps) goto 6
C.....rotate the "dpr1 vector" about the y-axis into the yz-plane.
      call strot3(2,dpr1(3)/s,dpr1(1)/s,tr)
      call mxmult(tt,tr)
C.....rotate about the x-axis so the "dpr1 vector" lies along the z-axis
6     call strot3(1,s,-dpr1(2),tr)
      call mxmult(tt,tr)
C
7     goto(2,2,3,4,5,3,4,5),ityp
      goto 5
8     call hcinit(tt)
      goto 7
C
3     continue
C =====================================================================
C       split lines from n1 to n2 on "3" clusters by "y0z" plane.
C =====================================================================
      j=0
      do 33 i=n1,n2
C    "j" - the counter for "nclust".
      j=j+1
      call hcncor(dx(1,i),dy(1,i),dz(1,i),tt)
      dx1=xp
      call hcncor(dx(2,i),dy(2,i),dz(2,i),tt)
      dx2=xp
        if(dx1) 34,35,36
34        if(dx2.ge.0.) goto 35
            nclust(j)=2
            goto 33
36        if(dx2.le.0.) goto 35
            nclust(j)=1
            goto 33
35        nclust(j)=3
33    continue
C**   call printn(nclust,nlndr)
      goto 2
C
4     continue
C ==================================================
C        split on "3" clusters by "x0z" plane.
C ==================================================
      j=0
      do 43 i=n1,n2
C    "j" - the counter for "nclust".
      j=j+1
      call hcncor(dx(1,i),dy(1,i),dz(1,i),tt)
      dy1=yp
      call hcncor(dx(2,i),dy(2,i),dz(2,i),tt)
      dy2=yp
        if(dy1) 44,45,46
44        if(dy2.ge.0.) goto 45
            nclust(j)=2
            goto 43
46        if(dy2.le.0.) goto 45
            nclust(j)=1
            goto 43
45        nclust(j)=3
43    continue
C**   call printn(nclust,nlndr)
      goto 2
C
5     continue
C ======================================================================
C    split lines from n1 to n2 on "5" clusters by "x0z" and "y0z" planes
C    of v.pl.coord.syst.: "1" - dx.gt.0;dy.gt.0; "2" - dx.lt.0;dy.gt.0;
C    "3" - dx.lt.0;dy.lt.0; "4" - dx.gt.0;dy.lt.0.
C ======================================================================
      j=0
      do 55 i=n1,n2
C   "j" - the counter for "nclust()".
      j=j+1
      call hcncor(dx(1,i),dy(1,i),dz(1,i),tt)
      dy1=yp
      dx1=xp
C**         zzz=zp
      call hcncor(dx(2,i),dy(2,i),dz(2,i),tt)
      dx2=xp
      dy2=yp
C**   print 556,j,dx1,dy1,dx2,dy2,zzz,zp
C##556   format('after',i5,6f18.8)
      if(dx1) 25,15,20
20      if(dy1) 21,15,23
C         because sorted |
21        if(dx2) 15,15,14
23        if(dx2.le.0.) goto 15
            if(dy2) 15,15,11
25      if(dy1) 26,15,28
C         because sorted |
26        if(dx2) 13,15,15
28        if(dx2.ge.0.) goto 15
            if(dy2) 15,15,12
11    nclust(j)=1
      goto 55
12    nclust(j)=2
      goto 55
13    nclust(j)=3
      goto 55
14    nclust(j)=4
      goto 55
15    nclust(j)=5
55     continue
C**   call printn(nclust,nlndr)
      goto 2
	end
