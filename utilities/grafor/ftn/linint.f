       subroutine linint(ibx,byi,ixf,yfi,rmax,rmin)
       dimension rmax(1),rmin(1)
       if(ibx.lt.ixf)   goto 82
      m=ibx-ixf+1
      do 89 k2=1,m
      k = k2-1
        z=yfi
       if (m.eq.1)   goto 12
      z=(byi-yfi)*k/(ibx-ixf)+yfi
12    continue
       if(rmin(ixf+k).gt.rmax(ixf+k))   goto 17
      if(z.le.rmax(ixf+k))   goto 13
      rmax(ixf+k)=z
      go to 89
13    if(z.ge.rmin(ixf+k))   goto 89
      rmin(ixf+k)=z
      go to 89
17    rmax(ixf+k)=z
       rmin(ixf+k)=z
  89  continue
      if(ibx.ne.ixf)   goto 87
 82   m=ixf-ibx+1
      do 90 k2=1,m
      k=k2-1
       z=byi
       if (m.ne.1)
     *z = (byi - yfi) * k / (ibx - ixf) + byi
       if(rmin(ibx+k).gt.rmax(ibx+k))   goto 27
      if(z.le.rmax(ibx+k))   goto 23
      rmax(ibx+k)=z
      go to 90
23    if(z.ge.rmin(ibx+k))   goto 90
      rmin(ibx+k)=z
      go to 90
27    rmax(ibx+k)=z
       rmin(ibx+k)=z
  90  continue
  87  return
      end
