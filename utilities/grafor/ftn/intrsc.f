      subroutine intrsc(x1,y1,x2,y2,x3,y3,x4,y4,x0,y0)
      a1=y2-y1
      b1=x1-x2
      a2=y4-y3
      b2=x3-x4
C
      d=a1*b2-a2*b1
C.....test: are the lines collinear to each other  ?
      if(abs(d).le.1.e- 8) goto 1
      c1=x2*y1-x1*y2
      c2=x4*y3-x3*y4
C.....compute the intersection point "x0,y0".
      x0=(c2*b1-b2*c1)/d
      y0=(c1*a2-c2*a1)/d
C.....test: does the point "x0,y0" belong to both line segments ?
      if(x0.lt.amax1(amin1(x1,x2),amin1(x3,x4))) goto 1
      if(x0.gt.amin1(amax1(x1,x2),amax1(x3,x4))) goto 1
      if(y0.lt.amax1(amin1(y1,y2),amin1(y3,y4))) goto 1
      if(y0.gt.amin1(amax1(y1,y2),amax1(y3,y4))) goto 1
      return
1     continue
C....in unnormal situation: a half of the element of the "screen".
      x0=0.5*(x3+x4)
      y0=0.5*(y3+y4)
      return
      end
