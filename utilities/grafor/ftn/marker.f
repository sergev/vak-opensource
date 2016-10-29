      subroutine marker(nmark)
      common/gfbftr/xtr,ytr
	common/gftab/irdb(5),rdb(17)
      dimension	mm(16),km(50),krab(5)
c      data km/o'23',o'20',o'23',o'32',o'02',o'33',o'00',o'33',
c     *o'31',o'04',o'32',o'11',o'13',o'33',o'31',
c     *o'33',o'20',o'02',o'24',o'42',o'31',o'24',
c     *o'00',o'24',o'40',o'33',o'02',o'40',o'02',
c     *o'44',o'32,o'11',o'24',o'31',o'02',o'12',
c     *o'33',o'20',o'13',o'42',o'21',o'33',o'02',
c     *o'31',o'24',o'23',o'31',o'02',o'33',o'20'/
      data km( 1), km( 2), km( 3), km( 4), km( 5) /19,16,19,26, 2/
      data km( 6), km( 7), km( 8), km( 9), km(10) /27, 0,27,25,04/
      data km(11), km(12), km(13), km(14), km(15) /26,09,11,27,25/
      data km(16), km(17), km(18), km(19), km(20) /27,16,02,20,34/
      data km(21), km(22), km(23), km(24), km(25) /25,20,00,20,32/
      data km(26), km(27), km(28), km(29), km(30) /27,02,32,02,36/
      data km(31), km(32), km(33), km(34), km(35) /26,09,20,25,02/
      data km(36), km(37), km(38), km(39), km(40) /10,27,16,11,34/
      data km(41), km(42), km(43), km(44), km(45) /17,27,02,25,20/
      data km(46), km(47), km(48), km(49), km(50) /19,25,02,27,16/
      data mm /1,2,3,4,5,6,33,7,8,9,10,66,65,49,1073,17185/
      ik=1
      j=nmark
      x=xtr
      y=ytr
      f=rdb(17)/rdb(1)
      x1=x
      y1=y
      f=0.0285*f
      if(j)5,9,9
    5 j=-j
      f=f*0.5
   9  do 100  k5=1,5
 100  krab(k5)=km(k5+10)
      k=krab(1)
      assign 2 to l
      go to 1
    2 if(j)8,8,3
    3 m=mm(j)
      f=5.0*f
    7 mx=mod(m,16)
      mx=5*mx-4
      do 101 k5=1,5
  101 krab(k5)=km(k5+mx-1)
      k=krab(1)
      assign 6 to l
      go to 1
    6 m=m/16
      if(m)8,8,7
    1 x=x+f*(mod(k,8)-2)
      k=k/8
      y=y+f*(mod(k,8)-2)
      call move(x,y,1)
      ik=ik+1
      k=krab(ik)
      if(ik.le.5)goto 1
      ik=1
    4 call move(x1,y1,1)
      x=x1
      y=y1
      go to l,(2,6)
    8 return
      end
