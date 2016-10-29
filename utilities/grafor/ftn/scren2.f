      subroutine scren2(iscr,scrn,idim1,idim2,x,y,nl,step,xbeg,a1,b1,
     1a2,b2,ind)
C...01/09/78
      dimension iscr(1),scrn(idim1,idim2,2),k(10)
      nr=nl+1
      l=iscr(nl)
      ir=iscr(nr)
      if(l*ir.ne.0) goto 3
      ind=0
      go to 30
    3 if(l.le.ir) goto 5
      nl=nr
      nr=nr-1
      i=l
      l=ir
      ir=i
    5 xbeg=step*(nl-1)
      xfin=xbeg+step*(nr-nl)
      n=ir-l
      k1=0
   17 if(k1.ge.n) goto 8
      do 9 i=1,ir
      if(k1.le.0) goto 12
      do 11 j=1,k1
      if(i.eq.k(j)) goto 9
   11 continue
      go to 12
    9 continue
   12 k1=k1+1
      k(k1)=i
      r=scrn(nr,i,2)-scrn(nr,i,1)
      do 13 i=1,ir
      do 15 j=1,k1
      if(i.eq.k(j)) goto 13
   15 continue
      r1=scrn(nr,i,2)-scrn(nr,i,1)
      if(r.le.r1) goto 13
      r=r1
      k(k1)=i
   13 continue
      go to 17
    8 k2=0
      do 29 i=1,l
   18 k2=k2+1
      if(k1.eq.0) goto 119
      do 19 j=1,k1
      if(k2.eq.k(j)) goto 18
   19 continue
  119 continue
      do 29 j=1,2
      a1=a2
      b1=b2
      b2=scrn(nl,i,j)
      a2=(scrn(nr,k2,j)-b2)/(xfin-xbeg)
      r=b2+a2*(x-xbeg)
      if(r.lt.y) goto 21
      if(i*j.ne.1) goto 23
      ind=1
      go to 30
   23 ind=2
      if(j.ne.1) ind=-2
      go to 30
   21 if((i*j-2*l).ne.0) goto 29
      ind=3
      go to 30
   29 continue
   30 i=1
      if(nr.ge.nl) goto 32
      nl=nr
   32 return
      end
