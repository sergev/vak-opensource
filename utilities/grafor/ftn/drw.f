      subroutine drw(dx,dy,dz,nlines,nl1,nl2)
      common/gfmt/t(4,4),tt(4,4),tr(4,4),dpr1(3),itpprj
      common/gfcrdn/xp,yp,zp
      dimension dx(2,nlines),dy(2,nlines),dz(2,nlines)
C.....nl1,nl2 - the numbers of initial and final lines drawn.
      do 1 i=nl1,nl2
      call hcncor(dx(1,i),dy(1,i),dz(1,i),t)
      call tmf(xp,yp,xf,yf)
      call move(xf,yf,0)
      call hcncor(dx(2,i),dy(2,i),dz(2,i),t)
      call tmf(xp,yp,xf,yf)
      call move(xf,yf,1)
1     continue
      return
      end
