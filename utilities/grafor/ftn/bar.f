      subroutine bar(x,y,h,w,sh,ihat,np1)
C   subroutine     bar       01/09/78
      common/gftab/irdb(5),rdb(17)
      if(irdb(2).ne.2) goto 9
      ak=w
      call tmf(x,y,xn,yn)
      call move(xn,yn,0)
      x1=x
      y1=y+h
      call tmf(x1,y1,xn1,yn1)
      call move(xn1,yn1,1)
      x1=x1+ak
      call tmf(x1,y1,xn1,yn1)
      call move(xn1,yn1,1)
      x1=x +ak
      y1=y
      call tmf(x1,y1,xn1,yn1)
      call move(xn1,yn1,1)
      call tmf(x,y,xn,yn)
      call move(xn,yn,1)
C     set flags accoding to ihat
      if (sh.le.0) goto 9
      k1=0
      k2=0
      if(ihat-1)9,15,8
  8   if(ihat-3)11,16,13
 11   k1=1
      go to 15
 13   k1=1
 16   k2=1
C     hatch one
  15  x2=x
      y2=y+sh
      x3=x2+ak
      y3=y2
      call tmf(x3,y3,xn3,yn3)
      call move(xn3,yn3,k1)
      call tmf(x2,y2,xn2,yn2)
      call move(xn2,yn2,1)
      call tmf(x1,y1,xn1,yn1)
      call move(xn1,yn1,k2)
      if (ihat.le.1) goto 9
      fnp1=np1
      l=w*fnp1
      m=sh*fnp1
      if(m.le.0) goto 9
      if(l.le.0) goto 9
      if(l.gt.m) goto 41
      f=l
      go to 42
   41 f=m
  42  dh=sh/f
      dw=w/f
      w2=0.0
      h1=0.0
      ih=f-1.0
      do 50 i=1,ih
      h1=h1+dh
      h2=sh-h1
      w2=w2+dw
      w1=w-w2
      xp1=x+w1
      yp1=y
      xp2=x
      yp2=y+h2
      xp3=x2+w2
      yp3=y2
      xp4=x1
      yp4=y1+h1
      call tmf(xp1,yp1,xpn1,ypn1)
      call move(xpn1,ypn1,0)
      call tmf(xp2,yp2,xpn2,ypn2)
      call move(xpn2,ypn2,k2)
      call tmf(xp3,yp3,xpn3,ypn3)
      call move(xpn3,ypn3,k1)
      call tmf(xp4,yp4,xpn4,ypn4)
      call move(xpn4,ypn4,k2)
      call tmf(xp1,yp1,xpn1,ypn1)
      call move(xpn1,ypn1,k1)
  50  continue
 9    return
      end
