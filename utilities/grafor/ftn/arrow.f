      subroutine arrow(j)
C     subroutine    arrow     01/06/75
C     subroutine    arrow     01/04/76
      common/gfgel/gf(28)
      common/gfbftr/xtr,ytr
      common/gftab/irdb(5),rdb(17)
      f=rdb(17)/rdb(1)
      if(gf(9).ge.0.) call strmod(0)
      i=abs(gf(9))
      if(ivest(gf(i),0.,.00000001)) 10,4,8
   4  x0=gf(i+1)
      y0=gf(i+2)
      xf=gf(i+6)
      yf=gf(i+7)
      delx=gf(i+6)-gf(i+1)
      dely=gf(i+7)-gf(i+2)
      th=angler(delx,dely)
      call move(x0,y0,0)
      if(j) 5,5,6
   5  call mova(0.302*f,th+8.,1)
      call mova(0.084*f,th-90.,0)
      call move(x0,y0,1)
      if(ivest(gf(i),0.,.00000001)) 10,6,9
   6  call move(xf,yf,1)
      if(j) 11,7,7
   7  call mova(0.302*f,th+172.,1)
      call mova(0.084*f,th-90.,0)
      call move(xf,yf,1)
      go to 11
   8  th0=gf(i+6)
      thf=gf(i+7)
      th1=gf(i+6)*gf(28)
      th2=gf(i+7)*gf(28)
      x0=gf(i+1)+abs(gf(i+3))*cos(th1)
      y0=gf(i+2)+abs(gf(i+3))*sin(th1)
      xf=gf(i+1)+abs(gf(i+3))*cos(th2)
      yf=gf(i+2)+abs(gf(i+3))*sin(th2)
      r=gf(i+3)
      call move(x0,y0,0)
      th=th0+90.*r/abs(r)
      if(j) 5,5,9
   9  call arcia(r,th0,thf)
      th=thf+90.*r/abs(r)
      if(j) 11,7,7
   10 call gffals(6)
   11 if(gf(9).gt.0.)call strmod(0)
      return
      end
