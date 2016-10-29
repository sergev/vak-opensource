      subroutine lscale(x1,y1,x2,y2,r)
C     subroutine    lscale    01/09/79
      if(y1.eq.y2) goto 2
      call rotate((x2*y1-x1*y2)/(y1-y2),0.,
     c-atan2(y1-y2,x1-x2)*(180./3.141593))
      call atran2(1.,0.,0.,0.,r,0.)
      call rotate((x2*y1-x1*y2)/(y1-y2),0.,
     c atan2(y1-y2,x1-x2)*(180./3.141593))
      go to 3
   2  call atran2(1.,0.,0.,0.,r,(1.-r)*y1)
  3   return
      end
