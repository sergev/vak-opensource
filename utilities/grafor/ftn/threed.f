       subroutine threed(x,y,z,nrow,ncol,ista,ifn,jsta,jfn,
     *	lntp,ngrd,amxmn,ar)
C...subroutine     threed    01/12/75
C...subroutine     threed    01/09/78
       dimension z(nrow,ncol),x(1),y(1),amxmn(1),ar(1)
      common/gfmatr/t(16),a(16),nt,nt1
       common/gfcrd/xp,yp,zp/gfsur/nar,dx,ngr,ncall
       common/gflim/xmn,xmx,zmn,zmx
      common/gfview/mac(4,5),xp1,yp1,zp1
      ist=iabs(ista)
      jst=iabs(jsta)
       nar=max0(nrow,ncol)
       if(ifn.lt.ist)goto 54
       if(jfn.lt.jst)goto 54
       mxmn=isign(1,jsta)
       if(lntp.eq.0)mxmn=1
       ivis=isign(1,ista)
      do 1 i=1,4
    1 mac(i,1)=-1
       if(ista.eq.0)goto 54
       if(ista.lt.0)goto 103
       ngr=ngrd
       dx=(xmx-xmn)/(ngrd-1)
      if(nt1.eq.1)goto 100
      if(nt1.eq.3)goto 102
      if(nt.eq.2)goto 100
 103  i=4
      goto 31
  100 sn=sin(5.*3.141592/180.)
       do 201 i=1,16
  201 a(i)=t(i)
       t(4)=0.
       t(8)=0.
       t(12)=0.
       i0=1
       if(lntp.eq.0)goto 203
       if(lntp.gt.0)goto 204
       i1=3
       goto 210
  203 i0=2
       i1=4
       goto 205
  204 i1=1
  205 k=1
       movy=1
       movx=1
       call hcncrd(0.,-1.,0.)
       if(yp.gt.0.) movy=-1
       r=yp*yp+zp*zp
       snr=sqrt(r/(r+xp*xp))
  206 if(snr.gt.sn)goto 212
      if(i1-2)209,213,211
 208   i0=1
       t(4)=0.
       t(8)=0.
       t(12)=0.
  209 i1=2
  210 k=-1
       movy=1
       movx=1
       call hcncrd(-1.,0.,0.)
        if(xp.gt.0.)movx=-1
        r=xp*xp+zp*zp
       snr=sqrt(r/(r+yp*yp))
       goto 206
  211 i1=2
       goto 205
  212 do 215  i=1,16
  215 t(i)=a(i)
      call hcsurf(x,y,z,nrow,ncol,ista,ifn,
     *    jsta,jfn,k,movx,movy,amxmn,ar)
        goto(54,208),i0
  213 do 214 i=1,16
  214 t(i)=a(i)
       goto 54
  102 xp=xp1
      yp=yp1
      zp=zp1
      i1=1
      i2=2
      if(xp.eq.x(jst))goto 8
      if(xp.gt.x(jst))goto 11
  2   if(yp.gt.y(ist))goto 4
  3   i=i1
   31 mac(i,1)=1
      mac(i,2)=ista
      mac(i,3)=ifn
      mac(i,4)=jsta
      mac(i,5)=jfn
      goto 21
    4 if(yp.lt.y(ifn)) goto 6
    5 i=i2
      goto 31
    6 iy=hcind(y,ist,ifn,yp)
      do 7 i=1,2
      ir1=i-1
      ir2=2-i
      j=ir1*i2+ir2*i1
      mac(j,1)=1
      mac(j,4)=jsta
      mac(j,5)=jfn
      mac(j,2)=(ir1*ist+ir2*iy)*ivis
    7 mac(j,3)=ir1*iy+ir2*ifn
      goto 21
    8 if(yp.lt.y(ist)) goto 3
      if(yp.eq.y(ist)) return
      if(yp.gt.y(ifn)) goto 5
      return
   11 if(xp.eq.x(jfn)) goto 20
      if(xp.gt.x(jfn)) goto 19
      jx=hcind(x,jst,jfn,xp)
      if(yp.gt.y(ist))goto 15
      if(yp.eq.y(ist))return
      isw=1
      ist1=ista
  131 ifn1=ifn
      i1=1
      i2=4
  132 do 14 i=1,2
      ir1=i-1
      ir2=2-i
      j=i1*ir2+i2*ir1
      mac(j,1)=1
      mac(j,2)=ist1
      mac(j,3)=ifn1
      mac(j,4)=(ir1*jst+ir2*jx)*mxmn
   14 mac(j,5)=ir1*jx+ir2*jfn
      goto(21,18),isw
   15 if(yp.lt.y(ifn)) goto 17
      if(yp.eq.y(ifn)) return
      ifn1=ifn
  161 ist1=ista
      isw=1
      i1=2
      i2=3
      goto 132
   17 iy=hcind(y,ist,ifn,yp)
      ist1=iy*ivis
      isw=2
      goto 131
   18 ifn1=iy
      goto 161
   19 i1=4
      i2=3
      goto 2
   20 i1=4
      i2=3
      goto 8
   21 if(lntp.eq.0) goto 29
      isw2=1
      k=lntp
      goto 231
   29 isw2=2
      k=1
  231 do 24 i=1,4
      if(mac(i,1).le.0) goto 24
      movx=1
      movy=1
      if(i.lt.2) goto 28
      if(i.gt.2) goto 27
 26   movy=-1
      goto 28
   27 movx=-1
      if(i.lt.4) goto 26
      if(i.gt.4) goto 24
  28  continue
       call hcsurf(x,y,z,nrow,ncol,mac(i,2),mac(i,3),mac(i,4),
     *   mac(i,5),k,movx,movy,amxmn,ar)
   24 continue
      if(isw2.eq.1)goto 54
      k=-1
      isw2=1
      goto 231
   54 return
      end
