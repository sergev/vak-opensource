      subroutine markbe (xi,yi,il)
      common /gfbet/ kt,istep,size
       dimension  xi(il),yi(il)
      ilberg=il-1
      do 10 i=1,ilberg,istep
      bi=xi(i+1)-xi(i)
      bj=yi(i+1)-yi(i)
      bnorm=sqrt(bi**2+bj**2)
      ci= + bj/bnorm
      cj= - bi/bnorm
      xim=(xi(i)+xi(i+1))*0.5
      yim=(yi(i)+yi(i+1))*0.5
C  yctahobka  ep"- tp xa
      call tmf(xim,yim,xf,yf)
      if(size.gt.0) go to 1
C  pa mep b matem. e  h  ax
      xb1=xim-size*ci
      yb1=yim-size*cj
      call tmf(xb1,yb1,xb,yb)
      go to 2
C  pa mep  b    . e  h  ax
  1   xb=xf+size*ci
      yb=yf+size*cj
  2   call move(xf,yf,0)
      call move(xb,yb,1)
 10   continue
      return
      end
