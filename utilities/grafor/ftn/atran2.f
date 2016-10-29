      subroutine atran2(a1,b1,c1,a2,b2,c2)
C     subroutine    atran2    01/09/79
      common /gfatrm/ a(6),b(6),n
      common /gfatrn/ r(6),is
      call mtmpl (a(1),a(2),a(3),a(4),a(5),a(6),
     ca1,b1,c1,a2,b2,c2,r(1),r(2),r(3),r(4),r(5),r(6))
      do 1 i=1,6
   1  a(i)=r(i)
       call mtmpl (a(1),a(2),a(3),a(4),a(5),a(6),
     cb(1),b(2),b(3),b(4),b(5),b(6),r(1),r(2),r(3),r(4),r(5),r(6))
      is=1
      return
      end
