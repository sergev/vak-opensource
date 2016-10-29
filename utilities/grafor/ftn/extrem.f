      subroutine extrem(r,t,n1,n2,xmn,xmx,ymn,ymx,ikop)
C...subroutine extrem 01/09/78
      dimension r(n1),t(n2)
      xmn=r(1)
      ymn=t(1)
      if(ikop.ne.1) goto 12
      xmn=xpol(r(1),t(1))
      ymn=ypol(r(1),t(1))
   12 xmx=xmn
      ymx=ymn
      do9 i=1,n1
      do9 j=1,n2
      xi=r(i)
      yi=t(j)
      if(ikop.ne.1) goto 1
      xi=xpol(r(i),t(j))
      yi=ypol(r(i),t(j))
    1 if(xmn.gt.xi) xmn=xi
      if(xmx.lt.xi) xmx=xi
      if(ymn.gt.yi) ymn=yi
      if(ymx.lt.yi) ymx=yi
9     continue
      return
      end
