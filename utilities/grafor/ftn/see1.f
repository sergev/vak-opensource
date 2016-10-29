      subroutine see1(i1,i2,i3,i4,x,y,xv,yv,ind)
C...01/09/78
      dimension x(1),y(1)
      x1=x(i1)
      y1=y(i1)
      x2=x(i2)
      y2=y(i2)
      x3=x(i3)
      y3=y(i3)
      x4=x(i4)
      y4=y(i4)
      m=0
      a=x1
      b=y1
      i=i1
    9 if(i.eq.i3) goto 2
      if(i.eq.i4) goto 2
      r1=cosin1(yv-y4,x4-xv,a-xv,b-yv)
      r2=cosin1(yv-b,a-xv,x3-xv,y3-yv)
      if(r1*r2.lt.0) goto2
      call lcross(a,b,xv,yv,x3,y3,x4,y4,c,d)
      goto 7
    2 r1=cosin1(yv-b,a-xv,(x3+x4)/2-xv,(y3+y4)/2-yv)
      if(m.ne.0) goto 11
      r3=r1
      a=x2
      b=y2
      i=i2
      m=m+1
      go to 9
   11 c=(x3+x4)/2
      d=(y3+y4)/2
      if(r1*r3.ge.0) goto 8
      call lcross(c,d,xv,yv,x1,y1,x2,y2,a,b)
    7 ind=0
      if(((a-xv)**2+(b-yv)**2-(c-xv)**2-(d-yv)**2).gt.0) return
    8 ind=1
      return
      end
