       subroutine hcinv(x,y,z,xp,yp,zp)
C      subroutine hcinv 01.09.78
       common/gfmatr/t(4,4),a(4,4),nt,nt1
       dimension b(3,4)
       do 1 i=1,4
       do 1 j=1,4
       i1=i-1
       if(i1.le.0)goto 3
       do 4 k=1,i1
       do 4 l=1,4
    4 b(k,l)=t(k,l)
    3 if(i.ge.4)goto 5
      i1=i+1
       do 7 k=i1,4
       do 7 l=1,4
    7 b(k-1,l)=t(k,l)
    5 if(j.ge.4)goto 9
      i1=j+1
       do 10 k=i1,4
       do 10 l=1,3
   10 b(l,k-1)=b(l,k)
    9 a(j,i)=(b(1,1)*b(2,2)*b(3,3)+b(2,1)*b(3,2)*b(1,3)+
     *  b(1,2)*b(2,3)*b(3,1)-b(3,1)*b(2,2)*b(1,3)-
     *  b(3,2)*b(2,3)*b(1,1)-b(2,1)*b(1,2)*b(3,3))*((-1.)**(i+j))
    1 continue
      c=x*a(1,4)+y*a(2,4)+z*a(3,4)+a(4,4)
      xp=(x*a(1,1)+y*a(2,1)+z*a(3,1)+a(4,1))/c
      yp=(x*a(1,2)+y*a(2,2)+z*a(3,2)+a(4,2))/c
      zp=(x*a(1,3)+y*a(2,3)+z*a(3,3)+a(4,3))/c
      return
       end
