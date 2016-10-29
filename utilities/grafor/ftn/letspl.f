      subroutine letspl(xi,yi,ilm,ziz,niz,kind)
      dimension xi(ilm),yi(ilm),ziz(niz)
      common /gfnam2/ key,pl(1)
      il=ilm-kind
      call tdline  (xi,yi,pl,il)
      key=1
      return
      end
