      subroutine blanch
C     blanch    18/06/73, 01/12/80, 01/12/81
      common/gfblan/jj,xx1,xx2,yy1,yy2,ncop,jch,kch,xxx,yyy,ich
      common/gfblu/ibp(16,4),bp(16,6),bl(6)
      common/gfatbl/a1,b1,c1,a2,b2,c2
C      correction  9-feb-83
	common/gfblaw/ymas(4),sinal,cosal,xx
      common/gfatrn/aa1,bb1,cc1,aa2,bb2,cc2,is
      if((xxx-xx1)**2+(yyy-yy1)**2.le.0.00005)  return
      al=atan2(yy2-yy1,xx2-xx1)
      sinal=sin(al)
      cosal=cos(al)
      xx=xx1*sinal-yy1*cosal
      ymas(2)=xx1*cosal+yy1*sinal
      ymas(3)=xx2*cosal+yy2*sinal
C
C
  13  ymas(1)=ymas(2)
      eps=0.00005
      delt=eps*(abs(ymas(2)))
      y1=ymas(2)+sign(delt,(ymas(3)-ymas(2)))
      y2=ymas(3)
      j1=0
      jch=0
   2  if (jch.eq.kch) goto 10
      jch=jch+1
C
      jax=ibp(jch,1)
      jay=ibp(jch,2)
      n=ibp(jch,3)
      in=ibp(jch,4)
      a1=bp(jch,1)
      b1=bp(jch,2)
      c1=bp(jch,3)
      a2=bp(jch,4)
      b2=bp(jch,5)
      c2=bp(jch,6)
C
      i=in
      j=0
	x = peek (jax, n)
	y = peek (jay, n)
	xr=a1*x+b1*y+c1
	yr=a2*x+b2*y+c2
      xr1=xr*sinal-yr*cosal
      yr1=xr*cosal+yr*sinal
C
      do 5 k=1,n
	x = peek (jax, k)
	y = peek (jay, k)
	xr=a1*x+b1*y+c1
	yr=a2*x+b2*y+c2
      xr2=xr*sinal-yr*cosal
      yr2=xr*cosal+yr*sinal
      if(abs (xr1-xx).lt.eps) xr1=xr1+sign(eps,xr1)
      if(abs (xr2-xx).lt.eps) xr2=xr2+sign(eps,xr2)
      if(((xr1-xx)*(xr2-xx)).gt.0.) goto 1
C
      yr=yr1+(xx-xr1)*(yr2-yr1)/(xr2-xr1)
      if (y1.lt.y2) goto 4
      if (yr.gt.y1) goto 1
      i=1-i
      if (yr.lt.y2) goto 1
      if(j.eq.1) goto 8
   3  j=1
      j1=1
   7  ymas(2)=yr
      goto 1
   8  if (yr.lt.ymas(2)) goto 1
      goto 7
   4  if (yr.lt.y1) goto 1
      i=1-i
      if (yr.gt.y2) goto 1
      if (j.eq.1) goto 9
      goto 3
   9  continue
      if(yr.gt.ymas(2)) goto 1
      goto 7
   1  xr1=xr2
      yr1=yr2
   5  continue
C
      if(i.eq.0) goto 10
      if(j.eq.1) y2=ymas(2)
      goto 2
  10  continue
      if(j1.eq.0) goto 11
      if(abs(ymas(1)-ymas(2)).gt.eps) goto 12
      goto 13
  11  ymas(2)=ymas(3)
C
  12  ich=2
      if(i.eq.0) ich=-2
      xx1=xx*sinal+ymas(1)*cosal
      yy1=-xx*cosal+ymas(1)*sinal
      xx2=xx*sinal+ymas(2)*cosal
      yy2=-xx*cosal+ymas(2)*sinal
      return
      end
