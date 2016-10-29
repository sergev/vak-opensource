      subroutine rdelp(xc,yc,a,b,alpha)
C    rdelp     01/04/76      01/01/80
      common/gfgel/gf(28)
      if(gf(9).ge.0.) call strmod(0)
      i=abs(gf(9))
      if(gf(i).gt.-2.) go to 5
      xc=gf(i+1)
      yc=gf(i+2)
      a=gf(i+3)
      b=gf(i+4)
      alpha=gf(i+5)
      go to 6
  5   call gffals(6)
  6   if(gf(9).gt.0.) call strmod(0)
      return
      end
