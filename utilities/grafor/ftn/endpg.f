      subroutine endpg(numb)
C...endpg 01/02/74 01/09/78 01/09/79
      common /gftab/irdb(5),rdb(17)
      common/gffrst/ifirst,lom
      common /gferr/ nn,iter(4)
      dimension	numb(1)
c
      call atrst
      do 6 i=1,16
	  call rentch
    6     call reblan
      do 3 i=1,3
 3        irdb(i)=0
      rdb(1)=rdb(17)
      do 5 i=5,16
 5        rdb(i)=0.
      ifirst=0
      call end
      return
      end
