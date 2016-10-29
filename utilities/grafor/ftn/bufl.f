      subroutine bufl(ixf,yfi,ibx,byi,rmax,rmin)
      common /gfl/ l,l1,n5,i,ldedg,ldgj,jj,lyj
      dimension rmax(1),rmin(1)
      if(ldedg.lt.0)    goto 55
      if(ldgj.lt.0)   goto 55
      if(n5)   53,55,56
 56   if (l.gt.0)   goto 53
      ib=ibx
      by=byi
      ia=ixf
      ay=yfi
      return
53    call linint(ib,by,ia,ay,rmax,rmin)
       if(n5.lt.0)  return
 55                 call linint (ibx,byi,ixf,yfi,rmax,rmin)
      return
      end
