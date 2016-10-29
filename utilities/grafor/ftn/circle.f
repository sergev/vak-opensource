      subroutine circle (xa,ya,th0,thf,r0,rf,l)
C      circle 01/09/78
      common /gftab/irdb(5),rdb(17)
      i2= 1
      i5 = l + 1
      x=xa
      y=ya
      call move (x,y,0)
      knt=5
      rorrf = sqrt(abs(r0)+abs(rf)+0.00001)
      fctr=rdb(17)/rdb(1)
      dth= (0.3/rorrf) * fctr
      t0= th0/57.2958
      tf= thf/57.2958
      c= x- r0 * cos(t0)
      tn=(tf-t0) / dth
      if(tf.ge.t0) go to 104
      tn=abs(tn)
      dth=-dth
 104  b=y-r0*sin(t0)
      n=tn
      if(n.le.0) go to 115
      tn=(rf-r0)/tn
      rn=r0-tn
      do 110 i=1,n
      t0=t0+dth
      rn=rn+tn
      x=rn*cos(t0)+c
      y=rn*sin(t0)+b
      if(knt.gt.0) go to 111
      i2=i5-i2
      knt=5
 111  knt=knt-1
 110  call move(x,y,i2)
 115  x=rf*cos(tf)+c
      y=rf*sin(tf)+b
      call move(x,y,i2)
      return
      end
