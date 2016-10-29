      subroutine scrmod(iscr,scrn,idim1,idim2,xl,yl,xir,yir,xk,yk)
C...01/09/78
      common /gftrg1/job,xloc,yloc,step,dop,itrgl(3)
      dimension iscr(1),scrn(idim1,idim2,2),x(3),y(3)
    1 format(30hincrease the number of screens)
      max=0
      x(1)=xl
      y(1)=yl
      x(2)=xir
      y(2)=yir
      x(3)=xk
      y(3)=yk
      do 3 j=1,2
      do 3 i=1,2
      if(x(i).le.x(i+1)) goto3
      a1=x(i)
      a2=y(i)
      x(i)=x(i+1)
      y(i)=y(i+1)
      x(i+1)=a1
      y(i+1)=a2
    3 continue
      n=x(1)/step+2
      x0=step*(n-1)
      call segang(x(1),y(1),x(3),y(3),a1,step,ind1)
      call segang(x(1),y(1),x(2),y(2),a2,step,ind2)
      call segang(x(2),y(2),x(3),y(3),a3,step,ind3)
   50 if((x0-x(3)).ge.0) goto 7
      y1=a1
      if(ind1.le.0)
     *y1=y(1)+a1*(x0-x(1))
      if(x0.gt.x(2)) goto 12
      y2=a2
      if(ind2.le.0)
     *y2=y(1)+a2*(x0-x(1))
      go to 15
   12 y2=a3
      if(ind3.le.0)
     *y2=y(2)+a3*(x0-x(2))
   15 if(y1.le.y2) goto 18
      r1=y1
      y1=y2
      y2=r1
   18 k=iscr(n)
      ish=1
      if(k.ne.0) goto 21
      scrn(n,1,1)=y1
      scrn(n,1,2)=y2
      go to 45
   21 m=0
      do 22 i=1,k
      if(scrn(n,i,1).le.y1) goto 22
      m=i
      go to 25
   22 continue
   25 if(m.ne.0) goto 26
      if((y1-scrn(n,k,2)-dop).gt.0) goto 29
      if(y2.gt.scrn(n,k,2)) scrn(n,k,2)=y2
       ish=ish-1
      go to 45
   29 scrn(n,k+1,1)=y1
      scrn(n,k+1,2)=y2
      go to 45
   26 do 30 i=m,k
      if((scrn(n,i,1)-y2-dop).gt.0) goto 32
   30 ish=ish-1
   32 if(m.eq.1) goto 34
      if((scrn(n,m-1,2)+dop-y1).le.0) goto 34
      m=m-1
      ish=ish-1
   34 if (ish.le.0) goto 36
      j=k-m+1
      do 38 i=1,j
      l=k-i+1
      scrn(n,l+1,1)=scrn(n,l,1)
   38 scrn(n,l+1,2)=scrn(n,l,2)
      scrn(n,m,1)=y1
      scrn(n,m,2)=y2
      go to 45
   36 if(scrn(n,m,1).gt.y1) scrn(n,m,1)=y1
      l=m-ish
      if(scrn(n,l,2).ge.y2) goto 42
      scrn(n,m,2)=y2
      go to 43
   42 scrn(n,m,2)=scrn(n,l,2)
   43 if(ish.ge.0) goto 45
      j=m-ish+1
      if(k.lt.j) goto 45
      do 47 i=j,k
      l=i+ish
      scrn(n,l,2)=scrn(n,i,2)
   47 scrn(n,l,1)=scrn(n,i,1)
   45 iscr(n)=iscr(n)+ish
      if(iscr(n).gt.max) max=iscr(n)
      n=n+1
      x0=x0+step
      if(max.le.idim2) goto 50
   46 write (*,1)
      stop
    7 return
      end
