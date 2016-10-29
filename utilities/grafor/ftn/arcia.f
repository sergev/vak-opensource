      subroutine arcia(r,th0,thf)
C     subroutine    arcia     24/08/76
C     subroutine    arcia     01/06/75
      common/gfbftr/xtr,ytr
      common/gftab/irdb(5),rdb(17)
      x=xtr
      y=ytr
      r1=abs(r)
      th=abs(th0-thf)
      if(ivest(th,360.,0.00000001)) 7,6,9
    7 if(th0.gt.thf) goto 2
      if(r) 4,9,6
    4 th02=th0+360.
      thf2=thf
      go to 3
    2 if(r) 6,9,5
    5 thf2=thf+360.
      th02=th0
      go to 3
    6 th02=th0
      thf2=thf
    3 th01=th02*0.01745329
      thf1=thf2*0.01745329
      fctr=(rdb(17)/rdb(1))*2.54
      dth=fctr*0.01/r1
      c=x-r1*cos(th01)
      tn=(thf1-th01)/dth
      if(thf1.ge.th01) goto 104
      tn=abs(tn)
      dth=-dth
  104 b=y-r1*sin(th01)
      n=tn
      if(n.le.0) goto 115
      do 110 i=1,n
      th01=th01+dth
      x=r1*cos(th01)+c
      y=r1*sin(th01)+b
  110 call move(x,y,1)
  115 x=r1*cos(thf1)+c
      y=r1*sin(thf1)+b
      call move(x,y,1)
      return
    9 call gffals(5)
      return
      end
