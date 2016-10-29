      subroutine lcross(x1,y1,x2,y2,x3,y3,x4,y4,c,d)
C...01/09/78
      r5=x3-x4
      r6=y4-y3
      r7=x1-x2
      r8=y2-y1
      r9=(y1-y3)*(x4-x3)-(x1-x3)*(y4-y3)
      r5=r5*r8-r7*r6
      c=-r7*r9/r5+x1
      d=r9*r8/r5+y1
      return
      end
