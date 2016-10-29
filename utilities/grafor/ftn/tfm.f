       subroutine tfm(xf,yf,xm,ym)
C...   tfm 01/09/78
       common /gftab/irdb(5),rdb(17)
       xm=rdb(11)+(abs(xf)-rdb(7))*(rdb(13)-rdb(11))/rdb(9)
       ym=rdb(12)+(abs(yf)-rdb(8))*(rdb(14)-rdb(12))/rdb(10)
       return
       end
