       subroutine search(ixf,yfi,ibx,byi,xf1,yf1,rmax,rmin)
      common /gfl/ l,l1,n5,i,ldedg,ldgj,jj,lyj
      equivalence (xl,cx1)
      equivalence (rdb7,rdb(7))
       common /gfxy/ xf,yf,r1,xlast,ylast
       common /gftab/ irdb(5),rdb(17)
      dimension rmax(1),rmin(1)
      dimension xf1(1),yf1(1)
      rmat(ix) = (float(ix) - 1.0) / r1 + rdb7
      if(ibx - ixf) 115,118,110
  110 n3 = ibx - ixf
       do 112 k = 1,n3
      cyi =(yfi - byi)* k/(ibx - ixf) + byi
      if(k.ge.n3)   cyi=yfi
      if (cyi.ge.rmax(ibx - k))   go to 113
      if (cyi.le.rmin(ibx - k))   go to 114
  112 continue
C
  113 icx = ibx - k
      l2 = -1
      ipr=2
C
C
520               cx1=rmat(icx)
      icx2=icx-l2
       cx2=rmat(icx2)
      if(ipr.eq.1)    go to 25
26    cy1=rmax(icx)
      cy2=rmax(icx2)
      go to   19
25    cy1=rmin(icx)
      cy2=rmin(icx2)
C
19    if(icx.ne.ixf)   goto 20
       cx3=rmat(ixf)
         cy3=yfi
         cx4=rmat(ibx)
         cy4=byi
         call intrsc (cx1,cy1,cx2,cy2,cx3,cy3,cx4,cy4,x0,y0)
         go to 512
C
  20  if (l.ne.0)   goto 516
      call intrsc( cx1,cy1 ,cx2,cy2,xf,yf,xl ast,ylast,x0,y0)
       go to 512
  516 call intrsc(cx1,cy1,cx2,cy2,xf,yf,xf1(i),yf1(i),x0,y0)
C
512   call bufl (ixf,yfi,icx,cyi,rmax,rmin)
C
C
C
131   if (l.ne.0)    goto 139
      call move (x0,y0,0)
      if(l1.eq.0)   goto 285
      if(ldedg.lt.0)   goto 55
      if(ldgj.ge.0)   goto 52
 55               xl=rmat(ixf)
                  call move(xl,yfi,1)
                  go to   51
 52   call move(xlast,ylast,1)
  51  call move(x0,y0,0)
      return
  285 call move (xf,yf,1)
      return
  139 call move (x0,y0,0)
      if(l1.ne.0)   goto 285
      call move (xf1(i),yf1(i),1)
      if(jj.ne.lyj)   call move(x0,y0,0)
      return
C
C
  114 icx = ibx - k
      l2 = -1
      ipr=1
      go to   520
C
C
C
  115 n3=ixf - ibx
       do 117 k=1,n3
      cyi = (yfi - byi) * k/(ixf-ibx) + byi
      if(k.ge.n3)   cyi=yfi
      if (cyi.ge.rmax(ibx +k))   goto 119
      if (cyi.le.rmin(ibx +k))   goto 132
  117 continue
  119 icx = ibx + k
      l2 = 1
      ipr=2
      go to   520
  132 icx = ibx + k
      l2 = 1
      ipr=1
      go to   520
C
118   cyi=yfi
      k=0
      end
