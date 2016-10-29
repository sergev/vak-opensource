      subroutine move1(x,y,j)
C     move1     24/08/76, 22/12/75, 16/04/76, 01/12/80, 01/12/81
      common/gfbftr/xtr,ytr
      common/gfaftr/ax,ay
      common/gfatrn/aa1,bb1,cc1,aa2,bb2,cc2,is
      common/gfblan/jj,xx1,xx2,yy1,yy2,ncop,jch,kch,xxx,yyy,ich
      common/gfgobs/njch,nkch,jg,nris,nach(16)
      common/gftab/irdb(5),rdb(17)
      xtr=x
      ytr=y
      if(is.gt.0) goto 13
  12  x1=x
      y1=y
      go to 14
   13 x1=aa1*x+bb1*y+cc1
      y1=aa2*x+bb2*y+cc2
   14 if(j.lt.990) goto 4
      j1=j-990
      k=0
      if(j1-1) 7,7,15
    4 j1=j
      x1=amin1(rdb(5),amax1(0.,x1))
      y1=amin1(rdb(6),amax1(0.,y1))
    7 k=2
      if(j1.ne.0) goto 15
      k=-2
      goto 9
C
  15  if(jj.eq.0) goto 9
      xx2=x1
      yy2=y1
      xxx=xx2
      yyy=yy2
      call blanch
  10  x1=xx2
      y1=yy2
      k=ich
    9 f=rdb(1)
      call plot(int(x1*f),int(y1*f),k)
      xx1=x1
      yy1=y1
      ax=x1
      ay=y1
      if(j1.eq.0) return
      if (jj.eq.0) return
      if ((xxx-xx1)**2+(yyy-yy1)**2.le.0.000005) goto 200
      call blan
      goto 10
 200  return
      end
