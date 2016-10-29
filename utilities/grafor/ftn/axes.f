      subroutine  axes (namex,nx,ux,kx,namey,ny,uy,ky,m)
	common/gftab/irdb(5),rdb(17)
      dimension	ir(4),namex(1),namey(1)
	character*4 n10
	equivalence (n10,in10)
      equivalence (mk,ir(1))
       data n10/' *10'/
	if(irdb(2).lt.2)go to261
  30   l=irdb(5)
      xleft=rdb(11)
      xrite=rdb(13)
      ydn=rdb(12)
      yup=rdb(14)
      xor=rdb(7)
      yor=rdb(8)
      xmr=xor+rdb(9)
      ymr=yor+rdb(10)
      zm=.3*rdb(17)/rdb(1)
       l1=irdb(4)
      c3=10.**6
      ix=max0(1,kx)
      if(ix-12)	113,113,111
  111 ix=1
  113 iy=max0(1,ky)
      if(iy-12)	117,117,115
  115 iy=1
  117 mf=m
      if(mf-10)	65,46,65
   46 mf=2
   65 mf=min0(max0(0,mf),3)+1
      ds=.1*rdb(17)/rdb(1)
      db=1.5*ds
      bsx=ux
      call astep (xleft,xrite,bsx,mkx,kdx)
      if(xleft-xrite) 24,24,23
   23 bsx=-bsx
   24 xno=aint(xleft/bsx)*bsx
      eps=0.000005*abs(xleft)
       if(abs(xno-xleft)-eps) 131,131,140
  140  if(xno-xleft) 133,131,134
  133 if(bsx)  131,132,132
  134 if(bsx)  132,132,131
  131 xno=xno-bsx
  132 bsy=uy
      call astep (ydn,yup,bsy,mky,kdy)
      cx=rdb(9)/(xrite-xleft)
      cy=rdb(10)/(yup-ydn)
      if(ydn-yup) 22,22,21
   21 bsy=-bsy
   22 ss=bsy/iy*cy
      yno=aint(ydn/bsy)*bsy
      eps=0.000005*abs(ydn)
      if(abs(yno-ydn)-eps) 31,31,40
   40  if(yno-ydn) 33,31,34
   33 if(bsy) 31,32,32
   34 if(bsy) 32,32,31
   31 yno=yno-bsy
   32 if (xleft) 1,5,3
    1 if(xrite)	2,5,5
   3  if(xrite)	5,5,2
    5 fx0=xor-cx*xleft
   55 call move(fx0,yor,0)
      z=amin1(.16*bsy*cy,zm)
      zh=1.5*z
      a=.1**kdy*c3
      y0=yno+bsy
      ano=yor+cy*(yno-ydn)
   10 do 6 i=1,iy
      ano=ano+ss
      if(ano-yor) 6,7,8
    8 if(ano-ymr) 7,7,9
    7 call move	(fx0,ano,1)
      call move	(fx0-ds,ano,1)
      call move	(fx0,ano,1)
    6 continue
      go to (67,68,67,68),mf
   67 call move	(fx0+db,ano,1)
      call move	(fx0-db,ano,1)
   39 yno=yno+bsy
      n=0
       r=yno*.1**mky
      if(r) 44,66,77
   44 r=amod(r,a*.1)-.000005
      go to 66
   77 r=amod(r,a)+.000005
   66 rano=r
      call bcd (rano,ir,kdy)
      call symbol (fx0-db,ano-(8-1.-kdy)*z,zh,ir,8,90.)
   69 call move	(fx0,ano,0)
      go to 10
   68 call move(xor,ano,1)
      call move(xmr,ano,1)
      go to 39
    9 call move	(fx0,ymr,1)
      ss=bsx/ix*cx
       if (ydn)	11,15,13
   11 if(yup) 12,15,15
   13 if(yup) 15,15,12
   15 fy0=yor-cy*ydn
  155 call move	(xor,fy0,0)
      ano=xor+cx*(xno-xleft)
      z=amin1(.16*bsx*cx,zm)
       zh=1.5*z
      a=.1**kdx*c3
  110 do 16 i=1,ix
      ano=ano+ss
      if(ano-xor) 16,17,18
   18 if(ano-xmr) 17,17,19
   17 call move	(ano,fy0,1)
      call move	(ano,fy0-ds,1)
      call move	(ano,fy0,1)
   16 continue
      go to (167,167,168,168),mf
  167 call move	(ano,fy0+db,1)
      call move	(ano,fy0-db,1)
   29 xno=xno+bsx
      n=0
       r=xno*.1**mkx
      if(r) 244,266,277
  244 r=amod(r,a*.1)-.000005
      go to 266
  277 r=amod(r,a)+.000005
  266 rano=r
      call bcd (rano,ir,kdx)
      call symbol (ano-(8-1.-kdx)*z,fy0-db-zh,zh,ir,8,0.)
  169 call move	(ano,fy0,0)
      go to 110
  168 call move	(ano,yor,1)
      call move(ano,ymr,1)
      go to 29
   19 call move	(xmr,fy0,1)
      lx=iabs(nx)
      ly=iabs(ny)
      z=amin1(.6*amin1(rdb(9)/max0(1,lx),rdb(10)/max0(1,ly)),zm)
      zh=1.5*z
       a3=(-6)*z*2/3
       x=xor-db-zm-zm
       y=yor-zh-db-zm-zm
      a1=xor+rdb(9)/2
      a2=z*lx/2
      call symbol(a1-a2,y,zh,namex,lx,0.)
      if(mkx) 71,61,71
   71 mk=mkx
       n=0
   41 mk=mk/10
      n=n+1
      if(mk) 41,51,41
   51  call ibcd(mkx,mk)
       call symbol (0.,0.,zh,in10,-4,0.)
       call symbol (a3,zh/2,z,mk,-8,0.)
   61 a1=yor+rdb(10)/2
      a2=z*ly/2
      call symbol(x,a1-a2,zh,namey,ly,90.)
      if(mky) 271,261,271
  271 mk=mky
       n=0
  241 mk=mk/10
      n=n+1
      if(mk) 241,251,241
 251   call ibcd(mky,mk)
       call symbol (0.,0.,zh,in10,-4,90.)
       call symbol (-zh/2,a3,z,mk,-8,90.)
  261 return
    2 x0=xno+bsx
      if(abs(x0)-abs(xrite)) 57,57,4
    4 x0=x0+bsx
      d=abs(x0)-abs(xrite)
      eps=0.000005*abs(xrite)
      if(abs(d)-eps) 57,57,500
  500  if(d) 56,57,4
   56 x0=x0-bsx
   57 fx0=xor+cx*(x0-xleft)
      go to 55
   12 if(abs(y0)-abs(yup)) 157,157,14
   14 y0=y0+bsy
      d=abs(y0)-abs(yup)
      eps=0.000005*abs(yup)
      if(abs(d)-eps) 157,157,400
  400 if(d) 156,157,14
  156 y0=y0-bsy
  157 fy0=yor+cy*(y0-ydn)
      go to 155
      end
