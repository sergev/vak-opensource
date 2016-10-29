       subroutine  oper(lrp1,i,x,rz)
C...01/09/78
        dimension x(1),rz(lrp1)
       lr=lrp1-1
       if(lr)100,200,300
200    rz(1)=1.0
  400  return
  300  lrpi=lr+i
       rz(1)=lr/(x(i)-x(lrpi))
       rz(2)=-rz(1)
       if(lr.eq.1) go to 400
       do 20 k=2,lr
       k0=1+lr-k
       kp1=1+k
       ipk0=k0+i
       do 10 l=1,kp1
       l0=kp1-l
       ipl0=l0+i
       ipkl0=ipk0+l0
       if(l0.eq.k) rz(l0+1)=rz(l0)*k0/(x(ipkl0-1)-x(ipl0-1))
       if(l0.eq.0) rz(1)=rz(1)*k0/(x(i)-x(ipk0))
       if(l0.gt.0.and.l0.lt.k) rz(l0+1)=(rz(l0+1)/(x(ipl0)-x(ipkl0))
     * +rz(l0)/(x(ipkl0-1)-x(ipl0-1)))*k0
   10  continue
   20  continue
       go to 400
  100  write (*,1)
1     format(/9hlrp1 le 1/32herror detected in routine   oper/)
      return
      end
