       subroutine reset
C     subroutine    reset      01/09/79
      common /gfatrm/ a(6),b(6),n
      common /gfatrn/ r(6),is
      dimension ai(6)
      data ai/1.,3*0.,1.,0./
      do 1 i=1,6
      a(i)=ai(i)
   1  r(i)=b(i)
      return
	end
