      subroutine haloed(dx,dy,dz,nlines,ndraw1,ndraw2,nc1,
     ,nc2,nclust,nlndr,g1,tol1,ityp)
      common/gfsv/te(15,2),nl,invsln
      common/gfeps/ eps,lengte
      common/gfgtol/g,tol,ndr1,ndr2
      dimension dx(2,nlines),dy(2,nlines),dz(2,nlines)
      dimension nclust(nlndr)
C------------- "nlndr=ndraw2-ndraw1+1", if ityp=3 - 8.
C
C..... "dx,dy,dz" are sorted so as the end point with the greater "dy"
C      value has the index "1" (if need call "sorty").
C.....parameters "nc1,nc2" are significant only for ityp=1 or 2 .
C
      g=g1
      tol=tol1
      ndr1=ndraw1
      ndr2=ndraw2
      goto(1,2,3,3,5,3,3,5),ityp
C
1     continue
C    ******************************************
C.....test each line with each of other lines.
C    ******************************************
      do 10 i=ndraw1,ndraw2
C.....set flag: line "i" is visible |
      invsln=1
C.... the number of invisible parts on line "i" is set to zero.
      nl=0
      call hldln(dx,dy,dz,nlines,i,nc1,nc2,nclust,nlndr,1)
C**   print 300,((te(i1,j1),j1=1,2),i1=1,5 )
C##300   format(' haloed te(15,2):'/(2f10.5))
C**   print 301,nl,invsln
C##301   format(' nl=',i5,' invsln=',i5)
C.....test: is "i"-th line visible
      if(invsln.eq.0) goto 10
C....draw visible parts of "i"-th line.
      call drawte
10    continue
      return
C
2     continue
C    **********************************************************
C..... lines are sorted in some way |
C..... test each line only with lines having less numbers than
C     this line.
C    **********************************************************
      do 20 i=ndraw1,ndraw2
C.....set flag: line "i" is visible |
      invsln=1
C.....the number of invisible parts on line "i" is set to zero.
      nl=0
      call hldln(dx,dy,dz,nlines,i,nc1,i,nclust,nlndr,2)
C.....test: is "i"-th line visible
      if(invsln.eq.0) goto 20
C..... draw visible parts of "i"-th line.
      call drawte
20    continue
      return
C
3     continue
C *******************************************************************
C  lines drawn are splitted on "3" clusters by "y0z" or "x0z" plane
C  of world coord.system.
C *******************************************************************
      i=ndraw1
39    continue
C.....set flag: line "i" is visible.
      invsln=1
C.....the number of invisible parts on line "i" is set to zero.
      nl=0
      incl=nclust(i-ndraw1+1)
      goto(31,32,33),incl
31    call hldln(dx,dy,dz,nlines,i,1,3,nclust,nlndr,3)
      goto 36
32    call hldln(dx,dy,dz,nlines,i,2,3,nclust,nlndr,3)
      goto 36
33    call hldln(dx,dy,dz,nlines,i,ndraw1,ndraw2,nclust,nlndr,1)
36    continue
C.....test: is "i"-th line visible
      if(invsln.eq.0) goto 37
C....draw visible parts of "i"-th line.
      call drawte
37    continue
      if(i.ge.ndraw2) return
      i=i+1
      goto(1,2,39,39,59,39,39,59),ityp
C
5     continue
C *******************************************************************
C  lines drawn are splitted on "5" clusters by "y0z" and "x0z" plane
C  of world coord.system.
C *******************************************************************
      i=ndraw1
59    continue
C.....set flag: line "i" is visible |
      invsln=1
C.... the number of invisible parts on line "i" is set to zero.
      nl=0
      incl=nclust(i-ndraw1+1)
      goto (51,52,53,54,55),incl
51    call hldln(dx,dy,dz,nlines,i,1,5,nclust,nlndr,5)
      goto 36
52    call hldln(dx,dy,dz,nlines,i,2,5,nclust,nlndr,5)
      goto 36
53    call hldln(dx,dy,dz,nlines,i,3,5,nclust,nlndr,5)
      goto 36
54    call hldln(dx,dy,dz,nlines,i,4,5,nclust,nlndr,5)
      goto 36
55    call hldln(dx,dy,dz,nlines,i,ndraw1,ndraw2,nclust,nlndr,1)
      goto 36
      end
