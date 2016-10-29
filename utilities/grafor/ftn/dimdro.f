      subroutine dimdro(d,j)
C.....subroutine dimdro  01/06/75
      common/gfgel/gf(28)
      common/gftab/irdb(5),rdb(17)
      common/gfbftr/xtr,ytr
      dimension buf(28)
      f=rdb(17)/rdb(1)
      call wrstr(buf)
      m1=1
      m2=1
      if(gf(9).ge.0.) call strmod(0)
      i=abs(gf(9))
      if(iabs(j).eq.10) goto 15
      if(j) 16,15,17
 16   m1=0
      go to 15
 17   m2=0
 15   l=gf(i)
      if(gf(9)) 41,3,38
  38  call strmod(0)
  41  if(l) 10,4,8
   4  call rdmva(x0,y0,dl,th)
      call move(x0,y0,0)
      if(d.eq.0.)go to 6
      if(th-90.) 11,11,12
  11  th1=th+sign(90.,d)
      m3=1
      goto 7
  12  if(th.gt.270.) goto 11
      th1=th-sign(90.,d)
      m3=0
  7    call mova(abs(d)+0.3*f,th1,m1*m3+m2*(1-m3))
       call mova(dl,th,0)
       call mova(abs(d)+0.3*f,th1+180.,m1*(1-m3)+m2*m3)
       call mova(abs(d),th1,0)
   20  if(iabs(j).ge.10) goto 19
       call wrmva(dl,th+180.)
       call arrow(0)
   40  call move(x0,y0,0)
       call mova(dl,th,0)
      goto 9
   6  call mova(dl,th,0)
      goto 20
  19  call mova(0.6*f,th,0)
      call wrmva(0.6*f,th+180.)
      call arrow(1)
      call mova(dl,th+180.,1)
      call wrmva(0.6*f,th+180.)
      call arrow(-1)
      goto 40
  8   call rdaca(x0,y0,r,th0,thf)
      call rdacb(x0,y0,r,xf,yf,j1)
      call move(x0,y0,0)
      if(d.eq.0.)goto 22
      if(r*(thf-th0)) 23,3,24
  23  th1=180.+th0/2+thf/2
      goto 25
  24  th1=th0/2+thf/2
 25   k=0
      if(th1.gt.180.) k=1
  28  if(r*k) 29,30,31
  30  if(r) 31,3,29
  29  m3=0
      goto 32
  31  m3=1
  32  if(d*(0.5-k)) 34,3,33
  33  q=0.
      goto 35
  34  q=1.
  35  call mova(abs(d)+0.3*f,th0+q*180.,m1*m3+m2*(1-m3))
      call move(xf,yf,0)
      call mova(abs(d)+0.3*f,thf+q*180.,m1*(1-m3)+m2*m3)
      call mova(0.3*f,thf+q*180.+180.,0)
  39  xf1=xtr
      yf1=ytr
      r1=abs(r)+abs(d)*(1.-2.*q)
      if(iabs(j).ge.10) goto 37
      call wraca(sign(r1,-r),thf,th0)
      call arrow(0)
      call move(xf,yf,0)
      goto 9
  22  call move(xf,yf,0)
      goto 39
  37  call wraca(sign(r1,r),thf,thf+sign(8.,r))
      call arrow(-1)
      call move(xf1,yf1,0)
      call arcia(sign(r1,-r),thf,th0)
      call wraca(sign(r1,-r),th0,th0-sign(8.,r))
      call arrow(-1)
      call move(xf,yf,0)
      goto 9
  10  call gffals(6)
   9  call rdstr(buf)
   3  return
      end
