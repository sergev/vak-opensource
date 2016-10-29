	subroutine endlev
      common /gfatrm/ a(6),b(6),n
      common /gfatrn/ r(6),is
	common /gfs/ s
      real s(96)
      integer isr(16)
      dimension ai(6)
      dimension e(6)
      data ai/1.,3*0.,1.,0./
      if (n.eq.0) goto 7
      do 3 i=1,6
      r(i)=b(i)
      b(i)=ai(i)
   3   continue
      is=isr(n)
      do 6 i=1,6
   6  a(i)=s(i+6*n-6)
      n=n-1
      if (n.eq.0) return
      do 4 j=1,n
      k=(j-1)*6
      call mtmpl(s(k+1),s(k+2),s(k+3),s(k+4),s(k+5),s(k+6),
     cb(1),b(2),b(3),b(4),b(5),b(6),
     ce(1),e(2),e(3),e(4),e(5),e(6))
      do 5 i=1,6
   5  b(i)=e(i)
   4  continue
      return
   7  call grafer ('ENDL')
      return
      end
