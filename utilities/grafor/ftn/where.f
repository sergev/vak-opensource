      subroutine where(x,y,f)
C     subroutine    where     10/12/73
C...subroutine   where    01/09/78
      common /gftab/irdb(5),rdb(17)
      common/gfaftr/ax,ay
      x=ax
      y=ay
      f=rdb(17)/rdb(1)
      return
      end
