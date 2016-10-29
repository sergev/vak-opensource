      subroutine symbol(x,y,size,jbcd,limit,theta)
C     subroutine    symbol    04/01/74
C...subroutine	symbol	  01/09/78
      common /gftab/irdb(5),rdb(17)
	common/gfatrn/aa1,bb1,cc1,aa2,bb2,cc2,is
      dimension	nplot(2,16),jbcd(1)
	data xn/0./
	data yn/0./
      is1=irdb(1)
       ait=rdb(2)
      ik=irdb(4)
      fact=size/9.
      t=(theta/180.)*3.14159265
      ct=fact*cos(t)
      st=fact*cos(1.5708-t)
      fx=ct*6.
      fy=st*6.
      n=limit
      xlp=rdb(5)
      ylp=rdb(6)
      xt=x
      yt=y
      if(n)8,400,7
    8 xt=xn+xt
      yt=yn+yt
      n=-n
    7 xn=xt
      yn=yt
      j=1
50	continue
      do 300 i=1,ik
      if(n.le.0) go to 400
      if(is.gt.0) go to	17
c      if(xn.lt.0.) go to 355
c      if(yn.lt.0.) go to 355
      if((xn-xlp).gt.0.001) go to 355
      if(yn-ylp-0.001)60,60,355
   17 xn1=aa1*xn+bb1*yn+cc1
      yn1=aa2*xn+bb2*yn+cc2
      if(xn1.lt.0.) go to 355
      if(yn1.lt.0.) go to 355
      if((xn1-xlp).gt.0.001) go	to 355
      if((yn1-ylp).gt.0.001) go	to 355
   60 call symtab(jbcd(j),i,nplot,kk,is1)
      if(kk.le.0) go to	355
      ipen=990
      do 200 k=1,kk
      if(nplot(1,k).lt.7) go to	100
      ipen=990
      go to 200
  100 yt=nplot(2,k)
      xt=nplot(1,k)+yt*ait
      tnx=xn+xt*ct-yt*st
      tny=yn+xt*st+yt*ct
      call move(tnx,tny,ipen)
      ipen=991
  200 continue
  355 n=n-1
      xn=xn+fx
  300 yn=yn+fy
      j=j+1
      go to 50
  400 return
      end
