      subroutine wherp(x,y,f)
C     subroutine    wherp     01/06/75     01/01/80
C     common/gfbftr/xtr,ytr
      common/gftab/irdb(5),rdb(17)
      x=xtr
      y=ytr
      f=rdb(17)/rdb(1)
      return
      end
