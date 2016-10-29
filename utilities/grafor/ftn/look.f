       subroutine look(n1,n2,x,y,phi,itagb,xx,yy,kt,ikop)
C...subroutine look 01/09/78
      dimension phi(n1,n2),itagb(n1,n2),x(n1),y(n2),xx(kt),yy(kt)
      common/gfcell/i1,j1,i2,j2,d1,d2,kent,qq,const
       common/gfint/ is
      common/gflook/ntoc
      ntoc=0
      if(d1.le.0) goto 1
      if((kent-1)*(kent-2).ne.0) goto 1
      ntoc=1
   1   continue
      go to(511,512,513,514),kent
  511 if(mod(itagb(i1,j1),10)-2)201,999,999
  512 if((mod(itagb(i2,j2),10)-1)*(mod(itagb(i2,j2),10)-3))201,999,201
  513 if(mod(itagb(i2,j2),10)-2)201,999,999
  514  if((mod(itagb(i1,j1),10)-1)*(mod(itagb(i1,j1),10)-3).
     *eq.0) goto 999
201    call zapit(itagb,n1,n2)
202    is=1
      call lint(x,y,xx,yy,n1,n2,kt,ikop)
       do 639 is=2,kt
      call cell(phi,n1,n2)
      call lint(x,y,xx,yy,n1,n2,kt,ikop)
      if(qq.ne.1.) goto 206
      if(itagb(i1,j1).lt.10) goto 206
      if(itagb(i2,j2).ge.10) goto 587
  206 go to(541,542,543,544),kent
  541 if(mod(itagb(i1,j1),10)-2)639,587,587
  542 if((mod(itagb(i2,j2),10)-1)*(mod(itagb(i2,j2),10)-3))639,587,639
  543 if(mod(itagb(i2,j2),10)-2)639,587,587
  544  if((mod(itagb(i1,j1),10)-1)*(mod(itagb(i1,j1),10)-3))639,587,639
  639  call zapit(itagb,n1,n2)
      call linnum(xx,yy,kt)
       go to 202
  587 call linnum(xx,yy,is)
       call zapit(itagb,n1,n2)
  999 return
      end
