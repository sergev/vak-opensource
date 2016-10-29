      subroutine token(x,y,mrk,fz,n,h,th)
      call tmf(x,y,xf,yf)
      call move(xf,yf,0)
      call marker(mrk)
      call number(xf,yf,h,fz,n,th)
      return
      end
