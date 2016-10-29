      subroutine  minmax(a,n,rmn,rmx)
      dimension	a(1)
      amn=a(1)
      amx=a(1)
      do 2 i=2,n
      r=a(i)
      amn=amin1(amn,r)
   2  amx=amax1(amx,r)
      rmn=amn
      rmx=amx
      return
      end
