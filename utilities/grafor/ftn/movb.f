      subroutine movb(dx,dy,j)
C  01/06/75    10/02/79
      common /gfbftr/xtr,ytr
      call move(xtr+dx,ytr+dy,j)
      return
      end
