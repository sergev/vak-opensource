      subroutine move3(x,y,j)
C     move3  06/02/74  24/08/76  01/09/79  01/01/82
      common/gfbftr/xtr,ytr
      common/gfaftr/ax,ay
      common/gfatrn/aa1,bb1,cc1,aa2,bb2,cc2,is
      common/gfblan/jj,xx1,xx2,yy1,yy2,ncop,jch,kch,xxx,yyy,ich
      common/gfntch/chekx(16),cheky(16),chek(16),xx3,yy3,nsign
      common/gfgobs/njch,nkch,jg,nris,nach(16)
      common/gftab/irdb(5),rdb(17)
      xtr=x
      ytr=y
      if(is.gt.0) goto 8
      x1=x
      y1=y
      goto 9
    8 x1=aa1*x+bb1*y+cc1
      y1=aa2*x+bb2*y+cc2
    9 if(j.lt.990) goto 2
      j1=j-990
      k=0
      if(j1-1)6,6,4
    2 j1=j
      x1=amin1(rdb(5),amax1(0.,x1))
      y1=amin1(rdb(6),amax1(0.,y1))
    6 k=2
      if(j1.eq.0) k=-2
C
    4 if(jg.eq.0) goto 35
      do 52 kk=1,nkch
      njch=kk
      if(nach(njch).gt.2*(nach(njch)/2)) goto 52
      if(nach(njch)/2.eq.2*(nach(njch)/4)) goto 44
      xx3=x4
      yy3=y4
      go to 41
   44 xx3=x2
      yy3=y2
   41 call notch1
      nach(njch)=nach(njch)+1
   52 continue
C
   35 x2=amax1(0.,x)
      y2=amax1(0.,y)
      x4=x1
      y4=y1
      nsign=2*j1-1
C
      if(jg.eq.0) goto 15
      do 51 kk=1,nkch
      njch=kk
      itrn=(nach(njch)/2-2*(nach(njch)/4))
      if(itrn*jj.ne.0) goto 51
      if(itrn.eq.0) goto 42
      xx3=x4
      yy3=y4
      go to 46
   42 xx3=x2
      yy3=y2
   46 call notch1
   51 continue
C
   15 if(jj.eq.0) goto 99
      if(j1.eq.0) goto 100
      xx2=x4
      yy2=y4
      xxx=xx2
      yyy=yy2
      call blanch
   10 x1=xx2
      y1=yy2
      k=ich
C
      if(jg.eq.0) goto 100
      do 66 kk=1,nkch
      njch=kk
      if(nach(njch)/2.eq.2*(nach(njch)/4)) goto 66
      xx3=xx2
      yy3=yy2
      nsign=2*((ich+2)/4)-1
      call notch1
  66	 continue
      if(nris.eq.0) k=-2
      goto 100
C
   99 if(jg.eq.0) goto 100
C
      if(nris.eq.0) goto 37
  100 f=rdb(1)
      call plot(int(x1*f),int(y1*f),k)
      ax=x1
      ay=y1
   37 xx1=x1
      yy1=y1
      nsign=2*((k+2)/4)-1
C
      if(j1.eq.0) return
C
      if (jj.eq.0) return
      if ((xxx-xx1)**2+(yyy-yy1)**2.le.0.000005) goto 200
      call blan
      goto 10
C
  200 return
      end
