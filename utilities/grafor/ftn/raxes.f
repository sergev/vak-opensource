      subroutine  raxes(name,n,h,ur,kr,m,t)
C...subroutine     raxes     01/09/73
C...subroutine          raxes             01/09/78
      common /gftab/ irdb(5),rdb(17)
      dimension name(1)
      equivalence(a(1),rdb(7))
      dimension a(10)
      if (irdb(2).ne.2) return
      call  signa(name,n,h,a(1),a(2),a(4),ur,kr,2.*a(5),2.*a(7),bs,mk,kd
     1  ,q,s,k,r6,ar,c)
      call tpf(a(5),t,-1,rf,p,x,y)
      t1=.01745329*p
      psi=cos(t1)
      fi=sin(t1)
      l=1
      if (h.lt.0.) l=0
      r1=1.
      t3=p
      if (psi.ge.0.) go to 68
      r1=-1.
      t3=p+180.
   68 call  move(x,y,0)
      t4=.01745329*a(9)
       akc=10.**(-mk)
      cs=cos(t4)
      sn=sin(t4)
      r4=q*(a(4)-a(3))/(a(7)-a(5))
      r4=amin1(.4*c,r4*1.5/(r6+1.))
      r6=r6*r4/3.*r1
      r=a(5)-amod(a(5),s)
      sot = sign(1.,q)
      if(sot*(r-a(5))) 95,95,59
   95 r=r+s
   59 if (sot*(a(7)-r).le.0.) go to 21
      call tpf(r,t,-1,rf,p,x,y)
      call  move(x,y,l)
       r2 = abs(amod(r,bs))
       r2 = r2*akc*(bs-r2)-ar
  82   t1=.1*c*r1
      l1=0
      if (r2.gt.0.) go to 79
      t1=2.*t1
      l1=1
   79 call  move(x+t1*fi,y-t1*psi,l)
      call move(x,y,l)
      if (kr.lt.0) go to 48
      if (r2.gt.0.) go to 48
      r5=r*akc
      t1=t1+r1*(r4+.1*c)
      call  number(x+fi*t1-r6*psi,y-t1*psi-r6*fi,r4,r5,kd,t3)
   48 if(abs((r-a(5))*akc*(a(7)-r))-ar) 42,42,50
   50 if(m-1)42,60,61
   60 if(l1)42,42,61
   61 if (p.gt.a(10)) go to 101
      call circle(x,y,p,a(10),rf,rf,l1)
   14 call  circle(a(1)+rf*cs,a(2)+rf*sn,a(9),p,rf,rf,l1)
      go to  42
  101 if (p.lt.(180.+(a(10)+a(9))/2.)) go to 14
      call circle(x,y,p,a(10),rf,rf,l1)
   42 call  move(x,y,0)
      go to 95
   21  call  move(a(1)+a(4)*psi,a(2)+a(4)*fi,l)
    1 return
      end
