      function coord (co,j)
C...function       coord     01/09/73
C...function       coord      01/09/78
      common /gftab/irdb(5),rdb(17)
      coord=rdb(11+j)
      if(amin1(co,rdb(11+j),rdb(13+j)).gt.0.)
     *  coord=alog10(co/rdb(11+j)) / alog10(rdb(13+j)/rdb(11+j)) *
     *  (rdb(13+j)-rdb(11+j)) + rdb(11+j)
      return
       end
