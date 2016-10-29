      subroutine  thaxes(name,n,h,ut,kt,mt,r)
C...subroutine     thaxes    01/09/73
C...subroutine             thaxes        01/09/78
      common /gftab/ irdb(5),rdb(17)
      dimension name(1)
      equivalence(a(1),rdb(7))
      dimension a(10)
      if (irdb(2).ne.2) go to 1
      call  signa(name,n,h,a(1),a(2),a(4),ut,kt,a(6)/3.6,a(8)/3.6,bs,
     1  mk,kd,q,s,k,r6,ar,c)
      call tpf(r,a(6),-1,rf,p,x1,y1)
      tb=q*(a(10)-a(9))/(a(8)-a(6))
      if (rf.le.0.) go to 17
      r4=rf*tb*.017
      r4=amin1(.4*c,1.5*r4/(r6+1.))
      r6=r6*r4/3.
       t2=a(6)-amod(a(6),s)
      if((t2-a(6))*s) 98,59,59
   98 t2=t2+s
   59 if((a(8)-t2)*s.le.0.) go to 21
      call tpf(r,t2,-1,rf,p1,x,y)
      if (h.ge.0.) call circle(x1,y1,p,p1,rf,rf,1)
      x1=x
      y1=y
      p=p1
      r2 = abs(amod(t2,bs))
      r2 = r2*(bs-r2)-ar
      b=.1*c
      if (r2.le.0.) b=.2*c
      e=(rf+b)/rf
      x=x-a(1)
      y=y-a(2)
      tp=a(1)+e*x
      rc=a(2)+e*y
      if (h.lt.0.) go to 42
      call move(tp,rc,1)
      call move(x1,y1,1)
   42 if (kt.lt.0) go to 98
      if (r2.gt.0.) go to 98
      if (abs(t2-a(8)).gt.ar) go to 27
      if (a(10)-a(9).gt.356.) go to 98
   27 call  number(tp-r6*y/rf,rc+r6*x/rf,r4,t2*10.**(-mk),kd,p-90.)
       go to 98
   21 if (h.ge.0.) call circle(x1,y1,p1,a(10),rf,rf,1)
 17   if(mt.le.0) go to 1
      s=q/mt
       tp=1.
      t2=a(8)-amod(a(8),s)
      if((t2-a(8))*s+ar)73,73,47
   47  t2=t2-s
   73 if((t2-a(6))*s+ar)1,77,77
   77 c1=c/tb
      r2 = abs(amod(t2,bs))
      r2 = r2*(bs-r2)-ar
      if (r2.gt.0.) c1=c1*mt
      rc=amax1(a(3),c1)
      rc=(a(4)+rc)/2.
      if (a(4).le.rc) go to 54
      call tpf(a(7),t2,-1,rf,p,x,y)
      p1=rc/a(4)
      x1=a(1)+(x-a(1))*p1
      y1=a(2)+(y-a(2))*p1
      x=x-x1
      y=y-y1
      call  move(x1+tp*x,y1+tp*y,0)
      call  move(x1-tp*x,y1-tp*y,1)
   54 tp=-tp
      go  to  47
    1 return
      end
