      subroutine move(x,y,j)
C...subroutine     move      01/02/74
C...subroutine    move    01/09/78
      common/gfbftr/xtr,ytr
      common/gfaftr/ax,ay
      common/gfatrn/aa1,bb1,cc1,aa2,bb2,cc2,is
      common/gfblan/jj,xx1,xx2,yy1,yy2,ncop,jch,kch,xxx,yyy,ich
      common /gftab/irdb(5),rdb(17)
      j1=j
      if(j1.gt.2) j1=j1-990
      if(j1.lt.0) go to 3
      if(j1.gt.2) go to 3
      xtr=x
      x1=x
      ytr=y
      y1=y
      if(is.le.0) go to 7
      x1=aa1*x+bb1*y+cc1
      y1=aa2*x+bb2*y+cc2
 7    k=0
      if(j1.eq.2) go to 4
      if(j.ge.990) go to 6
      x1=amin1(rdb(5),amax1(0.,x1))
      y1=amin1(rdb(6),amax1(0.,y1))
    6 k=2
      if(j1.eq.0) k=-2
    4 f=rdb(1)
      call plot(int(x1*f),int(y1*f),k)
      xx1=x1
      ax=x1
      yy1=y1
      ay=y1
    3 return
      end
