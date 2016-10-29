      subroutine vplim(dx,dy,dz,nlines,nl11,nl2,s)
      common/gfmt/t(4,4),tt(4,4),tr(4,4),dpr1(3),itpprj
      common/gfcrdn/xp,yp,zp
      dimension dx(2,nlines),dy(2,nlines),dz(2,nlines)
C.....nl1,nl2 - the numbers of initial and final lines drawn.
      nl1=iabs(nl11)
      call hcncor(dx(1,nl1),dy(1,nl1),dz(1,nl1),t)
      xmn=xp
      xmx=xp
      ymn=yp
      ymx=yp
      do 1 i=nl1,nl2
      do 1 j=1,2
      call hcncor(dx(j,i),dy(j,i),dz(j,i),t)
      if(xmn.gt.xp) xmn=xp
      if(xmx.lt.xp) xmx=xp
      if(ymn.gt.yp) ymn=yp
      if(ymx.lt.yp) ymx=yp
 1    continue
      if(nl11.ge.0) goto 5
C.....logical "or" between old and new values.
      if(xmin.lt.xmn) xmn=xmin
      if(xmax.gt.xmx) xmx=xmax
      if(ymin.lt.ymn) ymn=ymin
      if(ymax.gt.ymx) ymx=ymax
5     continue
      call limits(xmn,xmx,ymn,ymx)
C**   print 2,xmn,xmx,ymn,ymx
C##2     format(' limits:',6f10.4)
      xmin=xmn
      xmax=xmx
      ymin=ymn
      ymax=ymx
      s=(xmx-xmn)/(ymx-ymn)
      return
      end
