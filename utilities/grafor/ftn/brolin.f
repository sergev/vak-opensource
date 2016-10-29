       subroutine brolin(k,xs1,ys1,xf1,yf1,s1,s2)
C***
C... to draw a character line  (a wroken line, one dot-chain line and to
C    chain line).
C    k        selective parameter of character line.
C    k=1      broken line
C    k=2      one dot chain line
C    k=3      two dot chain lines
C    xs1,ys1  x and y coordinates of a starting point of the char.line
C    xf1,yf1 x and y coordinates of an end point of the car. line
C    s1       length of the long segment.
C    s2       length of the short segment and the space.
        xss=xs1
        yss=ys1
        xff=xf1
        yff=yf1
        sleng=sqrt((xss-xff)**2+(yss-yff)**2)
C***
        if(xff.ne.xss)goto 20
        theta=3.14159265*0.5
        goto 50
   20   thett=abs((yff-yss)/(xff-xss))
        theta=atan(thett)
   50   continue
        xsign=xff-xss
        ysign=yff-yss
        xpitch=sign(s1,xsign)*cos(theta)
        ypitch=sign(s1,ysign)*sin(theta)
        xspace=sign(s2,xsign)*cos(theta)
        yspace=sign(s2,ysign)*sin(theta)
C***
        xs=xss
        ys=yss
        goto (100,200,300),k
C***  character line ***hasen
  100   xf=xs+xpitch
        yf=ys+ypitch
        cleng=sqrt((xss-xf)**2+(yss-yf)**2)
       if(cleng.ge.sleng)goto 130
       call move(xs,ys,0)
       call move(xf,yf,1)
       xs=xf+xspace
       ys=yf+yspace
       cleng=sqrt((xss-xs)**2+(yss-ys)**2)
       if(cleng.ge.sleng) goto 900
       goto 100
C ***  end
 130    call move(xs,ys,0)
       call move(xff,yff,1)
       goto 900
C ***character line ***1 ten basen
  200  kk=1
C***
  210 if(kk.gt.1)goto 240
  220 kk=2
      xf=xs+xpitch
      yf=ys+ypitch
      goto 250
 240  kk=1
      xf=xs+xspace
      yf=ys+yspace
 250  cleng=sqrt((xss-xf)**2+(yss-yf)**2)
       if(cleng.ge.sleng) goto 270
      call move(xs,ys,0)
      call move(xf,yf,1)
      xs=xf+xspace
      ys=yf+yspace
      cleng=sqrt((xss-xs)**2+(yss-ys)**2)
      if(cleng.ge.sleng) goto 900
      goto 210
C *** end
  270  call move(xs,ys,0)
      call move(xff,yff,1)
      goto 900
C***   character line ***2 ten sasen
  300 kk=3
C **
  305 goto (310,320,330),kk
  310 kk=2
      goto 340
  320 kk=3
  340 xf=xs+xspace
      yf=ys+yspace
      goto 350
  330 kk=1
      xf=xs+xpitch
      yf=ys+ypitch
 350  cleng=sqrt((xss-xf)**2+(yss-yf)**2)
      if(cleng.ge.sleng) goto 370
      call move(xs,ys,0)
      call move(xf,yf,1)
      xs=xf+xspace
      ys=yf+yspace
      cleng=sqrt((xss-xs)**2+(yss-ys)**2)
      if(cleng.ge.sleng)goto 900
      goto 305
C***  end
  370 call move(xs,ys,0)
      call move(xff,yff,1)
 900  return
      end
