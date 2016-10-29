      subroutine transf(ziz,xi,yi,ilm)
      dimension xi(ilm),yi(ilm)
      do 1 i=1,ilm
      xs=xi(i)
      ys=yi(i)
      xi(i)=atdx(ziz,xs,ys)
      yi(i)=atdy(ziz,xs,ys)
  1   continue
      return
      end
