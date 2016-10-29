      subroutine linnum(x,y,kt)
C...subroutine linnum 01/09/78
      dimension x(kt),y(kt)
      common/gflsz/fn
      common/gflook/ntoc
      common/gflmn/sz,ks,kp,met
      common/gfisol/nc,const
      n=kt
      ft=fn
      if(met.ne.999) goto 2
      ft=const
      go to 151
2     if(met.ne.666) goto 15
      ft=float(nc)
      go to 151
15    if(met.ne.333) goto 9
151   d=(0.666667*ks+0.222222)*sz
      dd=0
      do13 l=2,kt
      call tmf(x(l),y(l),a,b)
       call tmf(x(l-1),y(l-1),c,r)
       dl=sqrt((a-c)**2+(b-r)**2)
13    dd=dd+dl
      if(dd.le.d*8) goto 9
      g=4.
      k=2
       ka=kt-2
      do 6 i=3,ka
      call tmf(x(i-1),y(i-1),xm,ym)
      call tmf(x(i),y(i),xi,yi)
      call tmf(x(i+1),y(i+1),xp,yp)
      if((yi-ym)*(xi-xm)*(yp-yi)*(xp-xi).eq.0) goto 6
      te=atan2(yi-ym,xi-xm)
      tet=atan2(yp-yi,xp-xi)
      gr=abs(te-tet)
      if(ntoc.eq.1) goto 32
      if(abs(te)-2.)35,35,6
32    if(abs(te)-1.)6,35,35
35    if(g.le.gr) goto 6
      k=i
      g=gr
6     continue
      call tmf(x(k),y(k),xk,yk)
11    call brline(x,y,k)
       kk=k
12    k=k+1
      if(kt.gt.k) goto 25
       k=kk
       go to 20
  25   continue
       call tmf(x(k),y(k),xl,yl)
      dm=sqrt((xk-xl)**2+(yk-yl)**2)
      if(dm.lt.d) goto 12
      the=atan2(yl-yk,xl-xk)
      xn=xk+sz/2*sin(the)
      yn=yk-sz/2*cos(the)
      t=the*57.2958
      if(ntoc.ne.1) goto 30
      t=t+180
      xn=xl-sz/2*sin(the)
      yn=yl+sz/2*cos(the)
  30   continue
       call number(xn,yn,sz,ft,kp,t)
20    n=kt-k+1
      do 7 j=1,n
      x(j)=x(k+j-1)
7     y(j)=y(k+j-1)
9     call brline(x,y,n)
      return
      end
