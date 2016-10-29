      subroutine arcc1(xm,ym,xf,yf,j,r,jb)
C     subroutine    arcc1     01/04/76
      common/gfbftr/xtr,ytr
      equivalence(xtr,x0),(ytr,y0)
      a=sqrt((x0-xf)**2+(y0-yf)**2)
      b=sqrt((xf-xm)**2+(yf-ym)**2)
      if(ivest(a,0.,.000001))   3,8,10
   10 c=sqrt((xm-x0)**2+(ym-y0)**2)
      p=(a+b+c)/2
      s2=p*(p-a)*(p-b)*(p-c)
      if(ivest(s2,0.,.000001))   3,9,11
   11 s1=sqrt(s2)
      r=(a*b*c)/(4*s1)
      z=sqrt((p-b)*(p-c)/(p*(p-a)))
      alpha=2*atan(z)/0.01745329
      jb=1
      if(ivest(alpha,90.,0.000001)  .gt.0) jb=0
      s=(x0-xf)*(ym-yf)-(xm-xf)*(y0-yf)
      if(ivest(s,0.,.000001))   5,3,6
    5 r=-r
    6 if(j.le.0) goto 3
      jb=1-jb
      r=-r
      goto 3
    8 r=b/2
      jb=1
      goto 3
    9 r=0
      jb=0
    3 return
      end
