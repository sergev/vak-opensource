      subroutine xaxis(y0,namex,nx,ux,kx,m,j)
      common/gftab/irdb(5),rdb(17)
      dimension ir(4),n10(2)
      equivalence (mk,ir(1))
      character*2 n10
      data n10/' *','10'/
      if(irdb(2).lt.2) return
   20 if(irdb(3).le.0) return
   30 l=8
      ja=iabs(j)
      xleft=rdb(11)
      xrite=rdb(13)
      xor =rdb(7)
      xmr=xor+rdb(9)
      zm =.3*rdb(17)/rdb(1)
	l1=8
      lx =iabs(nx)
      l2=irdb(4)-2
	c3=10.**6
      ix=max0(1,kx)
      if(ix-12) 117,117,115
  115 ix=1
  117 ds=.1*rdb(17)/rdb(1)
      db=1.5*ds
      bsx=ux
      call astep(xleft,xrite,bsx,mkx,kdx)
      cx=rdb(9)/(xrite-xleft)
      if(xleft-xrite) 22,22,21
   21 bsx=-bsx
   22 ss=bsx/ix*cx
      xno=aint(xleft/bsx)*bsx
      eps=0.000005*abs(xleft)
      if(abs(xno-xleft)-eps) 31,31,40
   40 if(xno-xleft) 33,31,34
   33 if(bsx) 31,32,32
   34 if(bsx) 32,32,31
   31 xno=xno-bsx
   32 call tmf(xno,y0,ano,fyo)
      call move(xor,fyo,0)
      if(ja-2) 12,12,11
   11 z=amin1(zm,cx*bsx/2)
      go to 13
   12 z=amin1(.16*bsx*cx,zm)
   13 zh=1.5*z
      b1=l1*z
      b4=(l-1-kdx)*z
      b6=l*z
      y1=fyo-db
      y2=fyo+db
      y3=y2+zh
      y4=y1-zh
      a	=.1**kdx*c3
   10 do 6  i=1,ix
      ano=ano+ss
      if(ano-xor) 6,7,8
    8 if(ano-xmr) 7,7,9
    7 call move(ano,fyo,1)
      call move(ano,fyo-ds,1)
      call move(ano,fyo,1)
    6 continue
      if(m)67,67,68
   67 call move(ano,y2,1)
      call move(ano,y1,1)
   39 xno=xno+bsx
      n=0
      r=xno*.1**mkx
      if(r) 71,73,72
   71 r=amod(r,a*.1)-.000005
      go to 73
   72 r=amod(r,a)+.000005
   73 rano=r
  300 n=n+1
      r=r*.1
      if(aint(r))300,301,300
  301 r=(n-1)*z
      b2=l2*z -r
      b3=b1-r
      b5=b4-z-z-r
      x1=ano-z/2
      x2=ano+z/2
      call bcd(rano,ir,kdx)
      k=l
      if(j)4,69,3
    3 go to(601,602,603,604),ja
    4 go to(611,612,613,614),ja
  601 y=y4
      go to 41
  602 y=y2
   41 x=ano-b4
      th=0.
      go to 49
  613 y=y1+b5
      go to 42
  614 y=y2+b6
   42 x=x1
      th=270.
      go to 49
  611 y=y1
      go to 43
  612 y=y2+zh
   43 x=ano+b4
      th=180.
      go to 49
  603 y=y1-b6
      go to 44
  604 y=y2-b5
   44 x=x2
      th=90.
      go to 49
  621 y=y1-zh
      go to 45
  622 y=y2
   45 x=ano-b3
      th=0.
      go to 49
  633 y=y1+b2
      go to 46
  634 y=y2+b1
   46 x=x1
      th=270.
      go to 49
  631 y=y1
      go to 47
  632 y=y3
   47 x=ano+b3
      th=180.
      go to 49
  623 y=y1-b1
      go to 48
  624 y=y2-b2
   48 x=x2
      th=90.
   49 call symbol(x,y,zh,ir,k,th)
   69 call move(ano,fyo,0)
      go to 10
   68 yor=rdb(8)
      ymr=yor+rdb(10)
      call move(ano,yor,1)
      call move(ano,ymr,1)
      go to 39
    9 call move(xmr,fyo,1)
      z=amin1(.6*rdb(9)/max0(1,lx),zm)
      zh=1.5*z
      a3=-l2*z*2/3
      a1=xor+rdb(9)/2
      a2=z*lx/2
      if(j)79,724,5
    5 go to(35,36,713,724), ja
   79 go to(37,38,731,742), ja
   35 y=y4-zh-ds
      go to 56
   36 y=y3+ds
      go to 56
   37 y=y4-ds
      go to 57
   38 y=y3+zh+ds
      go to 57
  724 y=y4-db
      go to 56
  713 y=y2+ds
   56 x=a1-a2
      th=0.
      go to 27
  742 y=y1-ds
      go to 57
  731 y=y3+db
   57 x=a1+a2
      th=180.
   27 call symbol(x,y,zh,namex,lx,th)
      if (mkx.eq.0) return
      mk=mkx
      n=0
  241 mk=mk/10
      n=n+1
      if (mk.ne.0) goto 241
      call ibcd(mkx,mk)
      call symbol(0.,0.,zh,n10,-4,0.)
      call symbol(a3,zh/2,z,mk,-2,0.)
      return
      end
