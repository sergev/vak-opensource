      subroutine strot3(naxes,cs,sn,r)
C   "r" - rotation matrix about axis "naxes" in right coord. system
C      clockwise if look from the end of the unit axis vector to the
C      origin.
      dimension r(4,4),i(4)
      data i/2,3,1,2/
      call hcinit(r)
      k=i(naxes)
      l=i(naxes+1)
      r(k,k)=cs
      r(l,l)=cs
      r(k,l)=-sn
      r(l,k)=sn
      return
      end
