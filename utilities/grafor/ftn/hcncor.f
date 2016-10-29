      subroutine hcncor(x,y,z,t)
      common/gfcrdn/xp,yp,zp
      dimension t(4,4)
      c=x*t(1,4)+y*t(2,4)+z*t(3,4)+t(4,4)
      xp=(x*t(1,1)+y*t(2,1)+z*t(3,1)+t(4,1))/c
      yp=(x*t(1,2)+y*t(2,2)+z*t(3,2)+t(4,2))/c
      zp=(x*t(1,3)+y*t(2,3)+z*t(3,3)+t(4,3))/c
      return
      end
