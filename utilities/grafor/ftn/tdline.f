       subroutine tdline(x,y,z,n)
       common/gfcrd/xp,yp,zp
       dimension x(1),y(1),z(1)
       call hcncrd(x(1),y(1),z(1))
       call tmf(xp,zp,x1,z1)
       call move(x1,z1,0)
       do 1 i=2,n
       call hcncrd(x(i),y(i),z(i))
       call tmf(xp,zp,x1,z1)
       call move(x1,z1,1)
    1 continue
       return
       end
