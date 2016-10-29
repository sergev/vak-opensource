      subroutine pictur(iscr,scrn,idim1,idim2,x1,y1,x2,y2)
C...01/09/78
      common /gftrg1/job,xloc,yloc,step,dop,itrgl(3)
      dimension iscr(1),scrn(idim1,idim2,2)
      xp=x1
      yp=y1
      ifst=0
      ipen=0
      istep=1
      if(x1.gt.x2) istep=-1
      nl=x1/step+1
      if(istep.lt.0) step=-step
      call segang(x1,y1,x2,y2,a,abs(step),iv)
      call scren2(iscr,scrn,idim1,idim2,xp,yp,nl,abs(step),xbeg,a1,b1,
     1a2,b2,ind)
      idrp=1
      if(ind.lt.0) idrp=0
      m=0
   26 xf=xp+step
      if(m.ne.0) goto 15
      xf=abs(step)*(nl-1)
      if(istep.gt.0) xf=step*nl
   15 if((xf-x2)*istep.ge.0) goto 17
      yf=a
      if(iv.le.0) yf=y1+a*(xf-x1)
      n1=nl
      if(istep.gt.0) n1=nl+1
      callscren1(iscr,scrn,idim1,idim2,yf,n1,idrf)
      if(idrp.ne.idrf) goto 24
      xp=xf
      yp=yf
      ifst=1
      m=1
      idrp=idrf
      nl=nl+istep
      go to 26
   24 m=0
   45 call scren2(iscr,scrn,idim1,idim2,xp,yp,nl,abs(step),xbeg,a1,b1,
     1a2,b2,indp)
      x3=xbeg
      x4=xbeg+step
      y=b2+a2*(xf-xbeg)
      if(indp.eq.0) goto 31
      if(indp.ne.1) goto 29
      if(yf-y) 31,31,32
   29 if(iabs(indp).ne.2) goto 33
      if(yf.ge.y) goto 32
      y=b1+a1*(xf-xbeg)
      if(yf.gt.y) goto 31
      b3=b1
      a3=a1
      go to 37
   33 if(yf-y) 32,32,31
   32 b3=b2
      a3=a2
   37 y3=b3
      y4=b3+a3*step
      call lcross(x1,y1,x2,y2,x3,y3,x4,y4,c,d)
   44 call tmf(c,d,c1,d1)
      if(idrp.le.0) goto 39
      if(ipen.gt.0) goto 41
      call tmf(x1,y1,x0,y0)
      call move(x0,y0,0)
   41 call move(c1,d1,1)
      xloc=c
      yloc=d
      go to 42
   39 xloc=c
      yloc=d
      call move(c1,d1,0)
      if(m.ne.1) goto 42
      call tmf(xf,yf,x0,y0)
      call move(x0,y0,1)
      xloc=xf
      yloc=yf
      go to 54
   31 if(m.eq.1) goto 47
      if(ifst.eq.0) goto 61
      c=(xp+xf)/2
      d=y1+a*(c-x1)
      go to 44
   42 if(m.eq.1) goto 53
      ipen=1
   61 xp=xf
      ifst=1
      yp=yf
      idrp=idrf
      m=1
      nl=nl+istep
      go to 26
   17 xf=x2
      yf=y2
      m=1
      go to 45
   47 if(idrp.ne.1) goto 54
      if(ipen.ne.0) goto 50
      call tmf(x1,y1,x0,y0)
      call move(x0,y0,0)
      go to 50
   53 if(idrp) 54,50,54
   50 call tmf(xf,yf,x0,y0)
      call move(x0,y0,1)
      xloc=xf
      yloc=yf
   54 step=abs(step)
      return
      end
