      subroutine segang(x1,y1,x2,y2,a,step,ind)
C...01/09/78
      if(abs(x1-x2).gt.step/2) goto 2
      ind=1
      i1=x1/step
      x=step*i1
      if(x2.le.x) goto 6
      x=x+step
      if(x2.lt.x) goto 5
    6 a=y1+(y2-y1)*(x-x1)/(x2-x1)
      go to 5
    2 ind=0
      a=(y2-y1)/(x2-x1)
    5 return
      end
