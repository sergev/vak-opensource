      subroutine beglev
C     subroutine    beglev     01/09/79
      common /gfatrm/ a(6),b(6),n
      common /gfatrn/ r(6),is
	common /gfs/s
      real s(96)
      integer isr(16)
      dimension ai(6)
      dimension e(6)
      data ai/1.,3*0.,1.,0./
      if (n.gt.16) goto 2
      do 1 i=1,6
      s(n*6+i)=a(i)
      a(i)=ai(i)
      b(i)=r(i)
   1  continue
      n=n+1
      isr(n)=is
      return
   2  call grafer ('BEGL')
      return
	end
