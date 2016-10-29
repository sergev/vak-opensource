      subroutine yaxis(x0,namey,ny,uy,ky,m,j)
C...subroutine     yaxis     01/09/73
	common/gftab/irdb(5),rdb(17)
      dimension namey(1),ir(4),n10(2)
      equivalence (mk,ir(1))
      character*2 n10
      data n10/' *','10'/
      if(irdb(2)-2) 261,20,20
  20  if(irdb(3)) 261,261,30
  30  l=8
      ja=iabs(j)
      ydn=rdb(12)
      yup=rdb(14)
      yor=rdb(8)
      ymr=yor+rdb(10)
      zm=.3*rdb(17)/rdb(1)
      l1=8
      ly=iabs(ny)
      l2=irdb(4)-2
      c3=10.**6
      iy=max0(1,ky)
      if(iy-12) 117,117,115
 115  iy=1
 117  ds=.1*rdb(17)/rdb(1)
      db=1.5*ds
      bsy=uy
      call astep(ydn,yup,bsy,mky,kdy)
      cy=rdb(10)/(yup-ydn)
      if(ydn-yup) 22,22,21
  21  bsy=-bsy
  22  ss=bsy/iy*cy
      yno=aint(ydn/bsy)*bsy
      eps=0.000005*abs(ydn)
      if(abs(yno-ydn)-eps) 31,31,40
  40  if(yno-ydn) 33,31,34
  33  if(bsy) 31,32,32
  34  if(bsy) 32,32,31
  31  yno=yno-bsy
  32  call tmf(x0,yno,fx0,ano)
      call move(fx0,yor,0)
      if(ja-2)  12,12,11
   11 z=amin1(zm,cy*bsy/2)
      go to 13
   12 z=amin1(.16*bsy*cy,zm)
   13 zh=1.5*z
      b1=l1*z
      b4=(l-1-kdy)*z
      b6=l*z
      x1=fx0-db
      x2=fx0+db
      x3=x2+zh
      x4=x1-zh
      a=.1**kdy*c3
  10  do 6  i=1,iy
      ano=ano+ss
      if(ano-yor) 6,7,8
   8  if(ano-ymr) 7,7,9
   7  call move(fx0,ano,1)
      call move(fx0-ds,ano,1)
      call move(fx0,ano,1)
   6  continue
      if(m) 67,67,68
   67 call move(x2,ano,1)
      call move(x1,ano,1)
  39  yno=yno+bsy
      n=0
      r=yno*.1**mky
      if(r) 71,73,72
   71 r=amod(r,a*.1)-.000005
      go to 73
   72 r=amod(r,a)+.000005
   73 rano=r
 300  n=n+1
      r=r*.1
      if(aint(r)) 300,301,300
 301  r=(n-1)*z
      b2=l2*z-r
      b3=b1-r
      b5=b4-z-z-r
      y1=ano-z/2
      y2=ano+z/2
      call bcd(rano,ir,kdy)
      k=l
      if(j) 4,69,3
   3  go to (601,602,603,604), ja
   4  go to (611,612,613,614), ja
  601 x=x1
      go to 41
  602 x=x3
   41 y=ano-b4
      th=90.
      go to 49
  603 x=x1-b6
      go to 42
  604 x=x2-b5
   42 y=y1
      th=0.
      go to 49
  611 x=x4
      go to 43
  612 x=x2
   43  y=ano+b4
      th=270.
      go to 49
  613 x=x1+b5
      go to 44
  614 x=x2+b6
   44  y=y2
      th=180.
      go to 49
  621 x=x1
      go to 45
  622 x=x3
   45 y=ano-b3
      th=90.
      go to 49
  623 x=x1-b1
      go to 46
  624 x=x2-b2
   46 y=y1
      th=0.
      go to 49
  631 x=x4
      go to 47
  632 x=x2
   47 y=ano+b3
      th=270.
      go to 49
  633 x=x1+b2
      go to 48
  634 x=x2+b1
   48 y=y2
      th=180.
   49 call symbol(x,y,zh,ir,k,th)
  69  call move (fx0,ano,0)
      go to 10
   68 xor = rdb(7)
      xmr = xor+rdb(9)
      call move (xor,ano,1)
      call move (xmr,ano,1)
      go to 39
   9  call move(fx0,ymr,1)
      z=amin1(.6*rdb(10)/max0(1,ly),zm)
      zh=1.5*z
      a3=-l2*z*2/3
      a1=yor+rdb(10)/2
      a2=z*ly/2
      if(j) 79,724,5
   5  go to (35,36,713,724), ja
  79  go to (37,38,731,742), ja
   35 x=x4-ds
      go to 56
   36 x=x3+zh+ds
      go to 56
   37 x=x4-zh-ds
      go to 57
   38 x=x3+ds
      go to 57
  724 x=x1-ds
      go to 56
  713 x=x3+db
   56 y=a1-a2
      th=90.
      go to 27
  742 x=x4-db
      go to 57
  731 x=x2+ds
   57 y=a1+a2
      th=270.
  27  call symbol(x,y,zh,namey,ly,th)
      if(mky) 271,261,271
  271 mk=mky
      n=0
  241 mk=mk/10
      n=n+1
      if(mk) 241,251,241
 251  call ibcd(mky,mk)
      call symbol(0.,0.,zh,n10,-4,th)
      call symbol(-zh/2,a3,z,mk,-2,th)
  261 return
      end
