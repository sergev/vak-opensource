      subroutine rdmvc(x0,y0,xm,ym,dl)
C.....subroutine rdmvc  01/06/75     01/01/80
      common/gfgel/gf(28)
      if(gf(9).ge.0.) call strmod(0)
      i=abs(gf(9))
      if(gf(i).ne.0.) goto 5
      x0=gf(i+1)
      y0=gf(i+2)
      delx=gf(i+6)-gf(i+1)
      dely=gf(i+7)-gf(i+2)
      xm=gf(i+1)+delx/2
      ym=gf(i+2)+dely/2
      dl=sqrt(delx**2+dely**2)
      goto 6
  5   call gffals(6)
  6   if(gf(9).gt.0.) call strmod(0)
      return
      end
