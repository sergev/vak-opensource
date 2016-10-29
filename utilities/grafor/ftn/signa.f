      subroutine  signa(name,n,h,xo,yo,rfn,utr,ktr,a,b,bs,mk,kd,q,s,k,
     1  r6,ar,c)
C...subroutine     signa     01/09/73
C...subroutine           signa           01/09/78
      dimension name(1)
      common /gftab/ irdb(5),rdb(17)
      bs=utr
      call  astep(a,b,bs,mk,kd)
      psi=b-a
      q=sign(bs,psi)
      k=iabs(ktr)
      if (k*(13-k).le.0) k=1
      s=q/k
       ar=bs*10.**(-mk)*bs*.001
      bs=abs(bs)
      r6=alog10(bs)-mk
      r6=amax1(0.,r6)
      r6=r6+kd+3.
       c=rdb(17)/rdb(1)
      sa=abs(h)
      if (h.eq.0.) sa=.5*c
      psi=rfn+.5*c+sa
      if (n.ge.0) go to 16
      r=0.
      qa=1.
      rf=0.
      n1=-n
      go to  99
   16 r=90.
      qa=0.
      rf=1.
      n1=n
   99 fi=sa*n1/3.+sa
      x=xo+rf*psi-qa*fi
      y=yo-rf*fi-qa*psi
      call  symbol(x,y,sa,name,n1,r)
      if(mk.eq.0) go to 1
      call symbol(x+(n1+1)*sa*qa,y+(n1+1)*sa*rf,sa,'*10',3,r)
      call number((-rf)*sa,qa*sa,(-sa)/1.8,float(mk),0,r)
    1 return
      end
