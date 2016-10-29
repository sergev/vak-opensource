      subroutine tmf(xm,ym,xf,yf)
C...subroutine	   tmf	 01/09/78
      common /gftab/irdb(5),rdb(17)
      cx=rdb(9)/(rdb(13)-rdb(11))
      cy=rdb(10)/(rdb(14)-rdb(12))
      xf=rdb(7)+cx*(xm-rdb(11))
      yf=rdb(8)+cy*(ym-rdb(12))
      return
      end
