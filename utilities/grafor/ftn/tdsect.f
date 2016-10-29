       subroutine tdsect(z,x,y,ix,iy,m,n,amxmn,ar)
C      subroutine tdsect 01.09.78
        dimension z(1),x(1),y(1),amxmn(1),ar(1)
      common/gfmatr/t(34)
       common/gfcrd/xp,yp,zp/gfsur/nar,dx,ngrd,ncall
       common/gflim/xmn,xmx,zmn,zmx
      m1=iabs(m)
      mxn2=0
      mxn1=0
      npl=0
      mxmn=0
      nsb=0
      igo=1
      if(m)2,264,1
    1 if(ncall-1)264,11,12
 11   igo=3
      goto 2
   12 igo=2
    2 ii=iabs(ix)
      jj=iabs(iy)
      i=1
      is=1
      if(n.eq.0)goto 264
      if(n.gt.0)goto 4
      i=i+(m1-1)*iabs(n)
      is=-1
    4 igo3=1
      call hcncrd(x(ii),y(jj),z(i))
      xm1=xp
      z11=zp
      z2=z11
      xx=xm1
      ivis=1
      if(igo.eq.1)goto 631
      if(igo.eq.3)goto 61
      isw5=1
      xp1=xp
      goto 80
  501 il1=il
      ir1=ir
      igo1=1
      goto 100
    6 ns1=nsos
      goto(61,62,62,63,64,64),ns1
   61 mxn2=1
  611 ivis=1
      goto 65
   62 mxmn=2
  621 mxn1=1
      goto 611
   63 ivis=-1
  631 mxmn=3
      mxn1=1
      goto 65
   64 mxmn=1
      goto 621
   65 if(npl-1)651,652,653
  652 iwk=npl+nar
       cl=ar(npl)-xx
       al=ar(iwk)-z2
       bl=ar(npl)*z2-xx*ar(iwk)
  651 npl=npl+1
      goto 654
  653 if(cl.eq.0.)goto 6532
      if(abs(al/cl*xx+bl/cl-z2).le.1.e-5)goto 654
      goto 652
 6532 if(abs(cl/al*z2-bl/al-xx).gt.1.e-5)goto 652
  654 iwk=npl+nar
       ar(iwk)=z2
       ar(npl)=xx
      if(igo3.eq.2)goto 57
      if(igo3.eq.3)goto 163
      ij=2
  707 if(ix)7072,264,7073
 7072 if(iy)264,264,709
 7073 if(iy)708,264,7091
  709 jj=jj+is
      goto 710
 7091 jj=jj+is
  708 ii=ii+is
  710 i=i+n
      call hcncrd(x(ii),y(jj),z(i))
      if(igo.ne.2)goto 901
      isw5=2
       xp1=xp
      goto 80
  901 xx=xp
      z2=zp
      igo3=2
       goto(631,8,61),igo
  8   il2=il
      ir2=ir
      igo1=2
      goto 100
  902 ns2=nsos
   50 goto(51,52,53,54,55,56),ns1
   51 goto(61,61,61,61,61,61),ns2
   52 goto(61,62,62,162,162,162),ns2
   53 goto(61,151,151,154,154,161),ns2
   54 goto(61,162,63,63,63,161),ns2
   55 goto(61,162,157,157,159,159),ns2
  56  goto(61,161,161,161,64,64),ns2
   57 if(ij.eq.m1)goto 59
      ij=ij+1
      if(igo.ne.2)goto 707
      il1=il2
      ir1=ir2
      nsb=ns1
      ns1=ns2
      xm1=xp
      z11=zp
      goto 707
   59 igo2=1
      goto 350
  151 if(nsb)264,62,152
  152 if(nsb.le.3)goto 62
      igo2=2
      goto 350
  154 if(nsb)264,63,155
  155 if(nsb.gt.3)goto 63
  156 igo2=3
      goto 350
  157 if(nsb)264,63,158
  158 if(nsb.ge.5)goto 156
      goto 63
  159 if(nsb)264,64,160
  160 if(nsb.ge.5)goto 64
 1531 igo2=4
         goto 350
  161 ig9=1
      ns3=ns2
      ns2=5
      goto 200
  165 igo3=3
      goto 50
  163 nsb=ns1
      ns1=ns2
      ns2=ns3
      xp1=xx
      isw5=5
      goto 80
  164 il1=il
      ir1=ir
      xm1=xx
      z11=z2
      xx=xp
      z2=zp
      igo3=2
      goto 50
  162 ig9=2
      ns3=ns2
      ns2=3
  200 if(xp-xm1)201,203,202
  201 ind1=ir1
      ns=-1
      ind2=il2
      goto 2071
  202 ind1=il1
      ns=1
      ind2=ir2
      goto 2071
  203 if(il1.ne.ir1)goto 202
      ind1=il1
      iwk=il1+1
      iwk1=iwk+ngrd
      if(amxmn(iwk).gt.amxmn(iwk1)-.5)goto 206
      iwk=il1-1
  206 ind2=iwk
 2071 iwk=iabs(ind1-ind2)-1
      if(iwk)264,207,2072
 2072 xrm=xx
      zrm=z2
c(((((do 2073 iii=1,iwk
      iii = 0
2073  continue
      iii = iii + 1
c)))))
      il=ind1+ns*iii
      ir=il
      xx=xmn+(il-1)*dx
      z2=zint(xm1,z11,xp,zp,xx)
      igo1=3
      goto 100
 2074 if(ns1.eq.nsos)goto 2073
      ind1=il
      ind2=ind1-ns
      goto 2076
c(((((2073 continue
      if (iii.lt.iwk) goto 2073
c)))))
 2076 xx=xrm
      z2=zrm
  207 xgr1=xmn+(ind1-1)*dx
      xgr2=xmn+(ind2-1)*dx
        if(ig9.eq.1)goto 209
      ind1=ind1+ngrd
      ind2=ind2+ngrd
  209 wk=xgr1-xgr2
      a11=(amxmn(ind1)-amxmn(ind2))/wk
      a12=(xgr1*amxmn(ind2)-xgr2*amxmn(ind1))/wk
      if(xp.eq.xm1)goto 211
      wk=xm1-xp
      a21=(z11-zp)/wk
      a22=(xm1*zp-xp*z11)/wk
       wk=a11-a21
        if(wk.eq.0.)goto 2102
      xx=(a22-a12)/wk
        goto 212
 2102 xx=(xgr1+xgr2)/2.
         goto 212
  211 xx=xp
  212 z2=a11*xx+a12
      goto 165
  350 call hcline(ar(1),ar(nar+1),npl*ivis)
      if(mxmn-3)354,352,264
  354 xp1=ar(1)
      isw5=3
      goto 80
  362 jl1=il
      jr1=ir
       iwk=npl+nar
       xp1=ar(npl)
      isw5=6
      goto 80
 3621 if(ncall-1)264,3622,3626
 3622 xmn1=ar(1)
       xmx1=ar(npl)
       zmn1=ar(nar+1)
       zmx1=ar(iwk)
      if(xmn1-xmx1)3623,264,3624
 3623 imn1=jr1
      imx1=il
      goto 3625
 3624 wk=xmn1
      xmn1=xmx1
      xmx1=wk
      wk=zmn1
      zmn1=zmx1
       zmx1=wk
      imn1=ir
      imx1=jl1
      goto 3625
 3626 if(mxn2)264,3625,3261
 3261 if(ar(1)-xmn1)3627,3628,3630
 3627 igg=1
       xr1=ar(1)
       zr1=ar(nar+1)
      ind1=jr1
      ind2=imn1-1
      xr2=xmn1
      zr2=zmn1
      imn1=jr1
       xmn1=xr1
       zmn1=zr1
      goto 3640
 3628 if(ar(npl).le.xmx1)goto 3632
      igg=2
       xr1=ar(npl)
       zr1=ar(iwk)
      ind1=imx1+1
      ind2=il
      xr2=xmx1
      zr2=zmx1
      imx1=il
       xmx1=xr1
       zmx1=zr1
      goto 3640
 3630 if(ar(1).le.xmx1)goto 3628
      igg=1
       xr1=ar(1)
       zr1=ar(nar+1)
      ind1=imx1+1
      ind2=jl1
      xr2=xmx1
      zr2=zmx1
      imx1=jl1
       xmx1=xr1
       zmx1=zr1
      goto 3640
 3632 if(ar(npl).ge.xmn1)goto 3625
      igg=2
      xr1=ar(npl)
      zr1=ar(iwk)
      ind1=ir
      ind2=imn1-1
      xr2=xmn1
      zr2=zmn1
      imn1=ir
       xmn1=xr1
       zmn1=zr1
 3640 if(ind1.gt.ind2)goto 3647
      do 3646 inr=ind1,ind2
      k1=inr+ngrd
      zin=zint(xr1,zr1,xr2,zr2,xmn+(inr-1)*dx)
      if(amxmn(inr).gt.amxmn(k1)-.5)goto 3643
      amxmn(k1)=zin
      goto 3644
 3643 k2=k1
      if(mxmn.eq.1)goto 3645
 3644 k2=inr
 3645 amxmn(k2)=zin
 3646 continue
 3647 if(igg.eq.1)goto 3628
 3625 continue
c(((((do 361 inp=2,npl
      inp = 1
361   continue
      inp = inp + 1
c)))))
       iwk=nar+inp
       xp1=ar(inp)
      isw5=4
      goto 80
  363 if(ar(inp).lt.ar(inp-1))goto 365
      jr=il
      jl=jr1
      goto 366
  365 jl=ir
      jr=jl1
  366 iw=(jr-jl)
      if(iw)384,3678,3674
 3678 if(ar(inp).ne.ar(inp-1))goto 3674
      k=jl
       zz1=ar(iwk)
      goto 3676
 3674 k=jl
 3679 xx1=xmn+(k-1)*dx
       zz1=zint(ar(inp-1),ar(iwk-1),ar(inp),ar(iwk),xx1)
 3676 if(mxn2)264,373,376
  373 k1=k
         s=1.
      if(mxmn.eq.1)goto 3741
      k1=k+ngrd
         s=-1.
 3741 if(s*(zz1-amxmn(k1)).le.0.)goto 368
  375 amxmn(k1)=zz1
      goto 368
  376 k1=k+ngrd
         if(amxmn(k).gt.amxmn(k1)-.5)goto 379
         amxmn(k)=zz1
         amxmn(k1)=zz1
         goto 368
  379 if(mxn1)264,380,373
  380 if(zz1-amxmn(k))382,368,381
  381 amxmn(k)=zz1
      goto 368
  382 if(zz1.lt.amxmn(k1))goto 375
 368  if(k.eq.jr)goto 384
      k=k+1
      goto 3679
  384 jl1=il
      jr1=ir
c(((((361 continue
      if (inp.lt.npl) goto 361
c)))))
  352 goto(264,353,264),igo
  353 mxn2=0
      mxn1=1
      nsb=0
       ar(1)=ar(npl)
       iwk=nar+npl
       ar(nar+1)=ar(iwk)
      npl=1
         goto(264,62,63,64),igo2
   80 il=(xp1-xmn)/dx+1
      ir=il
      if((xp1-xmn-(il-1)*dx).ne.0.) ir=ir+1
      goto(501,901,362,363,164,3621),isw5
  100 k1=il+ngrd
       ig=2
      if(il.ne.ir)ig=1
      z3=amxmn(il)
      z4=amxmn(k1)
      if(ig.eq.1)k2=ir+ngrd
      nsos=1
      if(z3.le.z4-.5)goto 113
      if(ig.eq.2)goto 106
      if(amxmn(ir).le.amxmn(k2)-.5)goto 113
      xl=xmn+(il-1)*dx
      xr=xl+dx
      z3=zint(xl,amxmn(il),xr,amxmn(ir),xx)
  106 nsos=6
      if(z2-z3)108,107,113
  107 nsos=5
      goto 113
  108 nsos=4
      if(ig.eq.2)goto 110
      z4=zint(xl,amxmn(k1),xr,amxmn(k2),xx)
  110 if(z2-z4) 111,112,113
  111 nsos=2
      goto 113
  112 nsos=3
  113 goto(6,902,2074),igo1
 264  return
      end
