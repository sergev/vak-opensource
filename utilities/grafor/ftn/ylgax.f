      subroutine ylgax (xo,name,n,j,k,m)
C...subroutine     ylgax     01/09/73
C...subroutine             ylgax           01/09/78
      common /gftab/ irdb(5),rdb(17)
      dimension name(1)
      if (irdb(2).ne.2) go to 1
      if (irdb(3).le.0) go to 1
      k1=iabs(k)
      call tmlgf (xo,rdb(12),-k1-k,1,x,y)
      b1=rdb(8)
      if (y.gt.b1) go to 1
      if (k1*(k1-11).ge.0) k1=1
      c=.2*rdb(17)/rdb(1)
      l=0
      r1=alog10(rdb(12))
      r2=alog10(rdb(14))
      e=r2-r1
      b2=rdb(10)
      b3=b1+b2
      bs=sign(float(j),e)
      t1=amin1(r1,r2)
      t2=1.83333
      if (t1.ge.0.) go to 9
      t1=t1-1.
      t2=t2+.5
 9    if(j) 6,5,6
 5    bs=sign(1.,e)
 6    bsf=bs/e*b2
      if (bsf.ge.4.*c) go to 15
      k1=1
      l=1
      bsf=1.5*bsf
   15 bsf=amin1(2.*c,(bsf-1.5*c)/t2)
      rt=r1-amod(r1,bs)
      if ((rt-r1)*bs.le.(-0.001)) rt=rt+bs
      if ((r2-rt)/bs.ge.0.) go to 19
      if (abs(bs).gt.1.) go to 5
      k1=9
      l=-1
      bsf=amin1(2.*c,.04*b2/abs(e))
   19 rr=10.**bs-1.
      call symbol(rdb(7)+rdb(9)+4.*c,b1+.5*b2-.6666*c*n,2.*c,name,n,90.)
      if (l*abs(t1).gt.(-1.)) go to 20
      call symbol(0.,0.,2.*c,'*10',-3,90.)
      call number (-1.5*c,0.,-1.5*c,t1,0,90.)
   20 i=0
   30 call move (x,y,0)
   40 i=i+1
      y	= b2*(rt-bs-r1+alog10(1.0+rr*i/k1))/e+b1
      if ((y-b1)*(k1-i).lt.0.) go to 40
      call move(x,amin1(y,b3),1)
      if (y.gt.(b3+.1*c)) go to 1
      t1=x-.5*c
      t2 = x
      if((k1-i)*(l+1)) 1,67,68
   67 t2=m*(rdb(7)-x+c)+x-c
      t1=t2+m*(rdb(9)-2.*c)+2.*c
   68 call move (t1,y,1)
      call move (t2,y,1)
      if(l) 69,88,42
   88 if(k1-i) 1,44,40
   42 l=l-(-1)**l
      if (l.eq.1) go to 90
      if (l.eq.2) go to 44
   69 call number (x-c-.6666*bsf,y-.5*bsf,bsf,5.5+(i-4.5)*abs(e)/e,0,0.)
      go to 30
   44 call symbol(x-c-2.3333*bsf,y-.45*bsf,bsf,'10',2,0.)
      call number(0.,.75*bsf,-.75*bsf,rt,0,0.)
   90 rt=rt+bs
      go to 20
    1 return
      end
