      subroutine fatarc(r,xf,yf,j,d)
C     subroutine    fatarc    01/04/76
      common/gfbftr/xtr,ytr
      common/gftab/irdb(5),rdb(17)
      x0=xtr
      y0=ytr
      f=rdb(17)/rdb(1)
      call arcang(r,xf-x0,yf-y0,j,th0,thf)
      call arcia(r,th0,thf)
      call mova(f*(0.1*d/2-0.015),thf-180.,1)
      r1=sign(abs(r)-f*(0.1*d/2-0.015),r)
      call arcia(-r1,thf,th0)
      call mova(f*(0.1*d-0.03),th0,1)
      call arcia(2*r-r1,th0,thf)
      call move(xf,yf,1)
      return
      end
