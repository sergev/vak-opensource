      function dist(j)
C     function      dist      01/06/75
      common/gfgel/gf(28)
      common/gfbftr/xtr,ytr
      x1=xtr
      y1=ytr
      if(gf(9).ge.0.) call strmod(0)
      i=abs(gf(9))
      k=gf(i)
      if(k) 17,4,10
C  line
    4 if(gf(9)) 6,3,5
    5 call strmod(0)
    6 call secant(0.,90.,x,y)
      dist=sqrt((x-x1)**2+(y-y1)**2)
      if(j) 3,28,7
    7 if(ivest(x,gf(i+1)/2.+gf(i+6)/2.,abs(gf(i+1)/2.-gf(i+6)/2.)).ne.0.
     1) goto 9
      if(ivest(y,gf(i+2)/2.+gf(i+7)/2.,abs(gf(i+2)/2.-gf(i+7)/2.)))
     19,28,9
    9 if(ivest((gf(i+1)-x)**2+(gf(i+2)-y)**2,(gf(i+6)-x)**2+(gf(i+7)-y)*
     1*2,0.00000001).gt.0.) goto 26
      sl2=(gf(i+1)-x)**2+(gf(i+2)-y)**2
      gf(19)=gf(i+1)
      gf(20)=gf(i+2)
      go to 27
   26 sl2=(gf(i+6)-x)**2+(gf(i+7)-y)**2
      gf(19)=gf(i+6)
      gf(20)=gf(i+7)
   27 dist=sqrt(sl2+dist**2)
    3 return
   28 gf(19)=x
      gf(20)=y
      go to 3
C  circle
   10 r1=abs(gf(i+3))
      th1=angler(x1-gf(i+1),y1-gf(i+2))*gf(28)
      x2=gf(i+1)+r1*cos(th1)
      y2=gf(i+2)+r1*sin(th1)
      if(k-2) 12,11,14
   11 dist=abs(r1-sqrt((gf(i+1)-x1)**2+(gf(i+2)-y1)**2))
      gf(19)=x2
      gf(20)=y2
   12 if(gf(9).gt.0.) call strmod(0)
      return
   14 th01=gf(i+6)*0.01745329
      thf1=gf(i+7)*0.01745329
      x0=gf(i+1)+r1*cos(th01)
      y0=gf(i+2)+r1*sin(th01)
      xf=gf(i+1)+r1*cos(thf1)
      yf=gf(i+2)+r1*sin(thf1)
      s=(x0-xf)*(y2-yf)-(x2-xf)*(y0-yf)
      if(gf(i+3)*s) 15,12,11
   15 if(j) 12,11,16
   16 if(ivest((x0-x1)**2+(y0-y1)**2,(xf-x1)**2+(yf-y1)**2,0.00000001).
     1gt.0.) go to 30
      dist=sqrt((x0-x1)**2+(y0-y1)**2)
      gf(19)=x0
      gf(20)=y0
      go to 12
   30 dist=sqrt((xf-x1)**2+(yf-y1)**2)
      gf(19)=xf
      gf(20)=yf
      go to 12
C  ellipse
   17 alpha1=gf(i+5)*0.01745329
      a1=abs(gf(i+3))
      g=abs(gf(i+4))/a1
      xn=x1-gf(i+1)
      yn=y1-gf(i+2)
      x1a=xn*cos(alpha1)+yn*sin(alpha1)
      y1a=(-xn*sin(alpha1)+yn*cos(alpha1))/g
      th=angler(x1a,y1a)*0.01745329
      dl1=10000.
      if(k+2) 19,18,12
   18 ith0=0
      ithf=6283
      go to 22
   19 call areb1(gf(i+6),gf(i+7),1/g,th02,thf2)
      th01=th02*0.01745329
      thf1=thf2*0.01745329
      s=(cos(th01)-cos(thf1))*(sin(th)-sin(thf1))-(cos(th)-cos(thf1))*(s
     1in(th01)-sin(thf1))
      if(gf(i+3)*s) 20,12,18
   20 if(j) 12,18,21
   21 ith0=th01*1000
      ithf=thf1*1000
   22 do 24 ith=ith0,ithf
      th1=ith/1000.
      x=a1*cos(th1)
      y=a1*sin(th1)
      dl=(x-x1a)**2+((y-y1a)*g)**2
      if(dl1.le.dl) goto 24
      dl1=dl
      xm=x
      ym=y
   24 continue
      dist=sqrt(dl1)
      gf(19)=xm*cos(alpha1)-ym*sin(alpha1)*g+gf(i+1)
      gf(20)=xm*sin(alpha1)+ym*cos(alpha1)*g+gf(i+2)
      go to 12
      end
