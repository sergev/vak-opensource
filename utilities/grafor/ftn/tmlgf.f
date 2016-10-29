      subroutine tmlgf (x,y,lg,n,xf,yf)
C...subroutine     tmlgf     01/09/73
C...subroutine          tmlgf            01/09/78
      common /gftab/ irdb(5),rdb(17)
      xm=x
      do 11 j=1,2
      xf=yf
      r11=rdb(j+10)
      r13=rdb(j+12)
      if (n.ge.0) xm=amax1(amin1(r11,r13),amin1(xm,amax1(r11,r13)))
      if (lg*(-1)**j.gt.0) go to 9
      if (amin1(r11,r13,xm).gt.0.) go to 7
      xm=r13
      go to 9
    7 r11=alog10(r11)
      r13=alog10(r13)
    8 xm=alog10(xm)
    9 yf=rdb(j+8)*(xm-r11)/(r13-r11)+rdb(j+6)
   11 xm=y
      return
      end
