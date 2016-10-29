      subroutine  itplbv(lx,ly,x,y,z,n,u,v,w)
C... itplbv 01/09/78
      dimension x(lx),y(ly),z(lx,ly),u(n),v(n),w(n)
      dimension  za(5,2),zb(2,5),zab(3,3),zx(4,4),zy(4,4),zxy(4,4)
C      equivalence (z3a1,za(1)),(z3a2,za(2)),(z3a3,za(3)),(z3a4,za(4)),
C     *(z3a5,za(5)),(z4a1,za(6)),(z4a2,za(7)),(z4a3,za(8)),(z4a4,za(9)),
C     *(z4a5,za(10))
C
      equivalence (z3b1,zb(1,1)),(z3b2,zb(1,2)),(z3b3,zb(1,3)),
     *(z3b4,zb(1,4)),(z3b5,zb(1,5)),(z4b1,zb(2,1)),
     *(z4b2,zb(2,2)),(z4b3,zb(2,3)),
     *(z4b4,zb(2,4)),(z4b5,zb(2,5)),(za2b2,zab(1,1)),(za3b2,zab(2,1)),
     *(za4b2,zab(3,1)),
     *(za2b3,zab(1,2)),(za3b3,zab(2,2)),(za4b3,zab(3,2)),
     *(za2b4,zab(1,3)),
     *(za3b4,zab(2,3)),(za4b4,zab(3,3)),(zx33,zx(2,2)),(zx43,zx(3,2)),
     *(zx34,zx(2,3)),(zx44,zx(3,3)),(zy33,zy(2,2)),(zy43,zy(3,2)),
     *(zy34,zy(2,3)),(zy44,zy(3,3)),(zxy33,zxy(2,2)),(zxy43,zxy(3,2)),
     *(zxy34,zxy(2,3)),(zxy44,zxy(3,3)),(p00,z33),
     *(p01,zy33),(p10,zx33),(p11,zxy33)
      equivalence  (lx0,zx(1,1)),(lxm1,zx(4,1)),(lxm2,zx(1,4)),
     *(lyp1,zy(4,4)),(ix,zxy(1,1)),(iy,zxy(4,1)),
     *(ixpv,zxy(1,4)),(lxp1,zx(4,4)),
     *(ly0,zy(1,1)),(lym1,zy(4,1)),(lym2,zy(1,4)),
     *(iypv,zxy(4,4)),(imn,jx),(imx,jy),
     *(jxm2,jx1), (jym2,jy1), (uk,dx), (vk,dy),
     *(a1,a5,b1,b5,zx(2,1),a,q0),(a2,zx(1,2),b,q1),
     *(a4,zx(4,2),c,q2), (b2,zy(2,1),d,q3),
     *(b4,zy(2,4),e), (x2,zx(3,1),a3sq), (x4,zx(1,3)),
     *(x5,zx(4,3)), (y2,zx(2,4)),
     *(y4,zy(3,1),b3sq),(y5,zx(3,4),p02), (z23,zy(1,4),p03),
     *(z24,zy(4,2),p12),(z32,zy(1,3) ,p13),
     *(z34,zy(3,4),p20),(z35,zy(3,4),p21),
     *(z42,zxy(2,1),p22),(z43,zxy(1,2),p23),
     *(z44,zxy(3,1),p30),(z45,zxy(4,2),p31),
     *(z53,zxy(1,3),p32),(z54,zxy(4,3),p3 ),
     *(w2,wy2,w4),(w3,wy3,w1,w5),
     *(wx2,zxy(2,4)),(wx3,zxy(3,4))
      lx0=lx
      lxm1=lx0-1
      lxm2=lxm1-1
      lxp1=lx0+1
      ly0=ly
      lym1=ly0-1
      lym2=lym1-1
      lyp1=ly0+1
      n0=n
      if(lxm2.lt.0) go to 710
      if(lym2.lt.0) go to 720
      if(n0.lt.1) goto 730
      do 10 ix=2,lx0
      if(x(ix-1)-x(ix)) 10,740,750
   10 continue
      do 20 iy=2,ly0
      if(y(iy-1)-y(iy))  20,770,780
   20 continue
      do 700 k=1,n0
      uk=u(k)
      vk=v(k)
      if(lxm2.eq.0) goto 80
      if(uk.ge.x(lx0)) go to 70
      if(uk.lt.x(1)) go to 60
      imn=2
      imx=lx0
   30 ix=(imn+imx)/2
      if(uk.ge.x(ix)) go to 40
      imx=ix
      go to 50
   40 imn =ix+1
   50 if(imx.gt.imn) go to 30
      ix=imx
        go to 90
   60 ix=1
        go to 90
   70 ix =lxp1
        go to 90
   80 ix=2
   90 if(lym2.eq.0) go to 150
      if(vk.ge.y(ly0)) go to  140
      if(vk.lt.y(1)) go to 130
      imn=2
      imx=ly0
  100 iy=(imn+imx)/2
        if (vk.ge.y(iy)) go to 110
        imx=iy
        go to 120
  110   imn=iy+1
  120   if (imx.gt.imn) go to 100
        iy=imx
        go to 160
  130   iy=1
        go to 160
  140   iy=lyp1
        go to 160
  150   iy=2
  160   if(ix.eq.ixpv.and.iy.eq.iypv) go to 690
        ixpv=ix
        iypv=iy
        jx=ix
        if(jx.eq.1) jx=2
        if(jx.eq.lxp1) jx=lx0
        jy=iy
        if(jy.eq.1) jy=2
        if (jy.eq.lyp1) jy=ly0
        jxm2=jx-2
        jxml=jx-lx0
        jym2=jy-2
        jyml=jy-ly0
        x3=x(jx-1)
        x4=x(jx)
        a3=1.0/(x4-x3)
        y3=y(jy-1)
        y4=y(jy)
        b3=1.0/(y4-y3)
        z33=z(jx-1,jy-1)
        z43=z(jx,jy-1)
        z34=z(jx-1,jy)
        z44=z(jx,jy)
        za(3,1)=(z43-z33)*a3
        za(3,2)=(z44-z34)*a3
        z3b3=(z34-z33)*b3
        z4b3=(z44-z43)*b3
        za3b3=(z4b3-z3b3)*a3
        if (lxm2.eq.0) go to 230
        if (jxm2.eq.0) go to 170
        x2=x(jx-2)
        a2= 1.0/(x3-x2)
        z23= z(jx-2,jy-1)
      z24=z(jx-2,jy)
        za(2,1)=(z33-z23)*a2
         za(2,2)=(z34-z24)*a2
        if (jxml.eq.0)go to 180
  170   x5=x(jx+1)
        a4=1.0/(x5-x4)
        z53=z(jx+1,jy-1)
        z54=z(jx+1,jy)
        za(4,1)=(z53-z43)*a4
        za(4,2)=(z54-z44)* a4
        if(jxm2.ne.0) go to 190
        za(2,1)=za(3,1)+za(3,1)-za(4,1)
        za(2,2)=za(3,2)+za(3,2)-za(4,2)
        go to 190
  180   za(4,1)=za(3,1)+za(3,1)-za(2,1)
        za(4,2)=za(3,2)+za(3,2)-za(2,2)
  190   za2b3=(za(2,2)-za(2,1))*b3
        za4b3=(za(4,2)-za(4,1))*b3
        if (jx.le.3) go to 200
        a1=1.0/(x2-x(jx-3))
        za(1,1)=(z23-z(jx-3,jy-1))*a1
        za(1,2)=(z24-z(jx-3,jy))*a1
        go to 210
  200   za(1,1)=za(2,1)+za(2,1)-za(3,1)
        za(1,2)=za(2,2)+za(2,2)-za(3,2)
  210 if(jx.ge.lxm1) go to 220
      a5=1.0/(x(jx+2)-x5)
        za(5,1)=(z(jx+2,jy-1)-z53)*a5
      za(5,2)=(z(jx+2,jy)-z54)*a5
        go to 240
  220   za(5,1)=za(4,1)+za(4,1)-za(3,1)
        za(5,2)=za(4,2)+za(4,2)-za(3,2)
        go to 240
  230   za(2,1)=za(3,1)
         za(2,2)=za(3,2)
        go to 180
  240   if(lym2.eq.0) go to 310
        if(jym2.eq.0) go to 250
        y2=y(jy-2)
        b2=1.0/(y3-y2)
        z32=z(jx-1,jy-2)
         z42=z(jx,jy-2)
        z3b2=(z33-z32)*b2
        z4b2=(z43-z42)*b2
         if(jyml.eq.0) go to 260
  250   y5=y(jy+1)
        b4=1.0/(y5-y4)
        z35=z(jx-1,jy+1)
        z45=z(jx,jy+1)
        z3b4=(z35-z34)*b4
        z4b4=(z45-z44)*b4
        if(jym2.ne.0) go to 270
        z3b2=z3b3+z3b3-z3b4
        z4b2=z4b3+z4b3-z4b4
        go to 270
  260   z3b4=z3b3+z3b3-z3b2
        z4b4=z4b3+z4b3-z4b2
  270   za3b2=(z4b2-z3b2)*a3
        za3b4=(z4b4-z3b4)*a3
        if(jy.le.3)go to 280
        b1 =1.0/(y2-y(jy-3))
        z3b1=(z32-z(jx-1,jy-3))*b1
        z4b1=(z42-z(jx,jy-3))*b1
        go to 290
  280   z3b1=z3b2+z3b2-z3b3
        z4b1=z4b2+z4b2-z4b3
  290   if(jy.ge.lym1) go to 300
        b5=1.0/(y(jy+2)-y5)
        z3b5=(z(jx-1,jy+2)-z35)*b5
        z4b5=(z(jx,jy+2)-z45)*b5
        go to 320
  300   z3b5=z3b4+z3b4-z3b3
        z4b5=z4b4+z4b4-z4b3
        go to 320
  310   z3b2=z3b3
        z4b2=z4b3
        go to 260
  320   if(lxm2.eq.0) go to 400
        if(lym2.eq.0) go to 410
        if(jxml.eq.0) go to 350
        if(jym2.eq.0) go to 330
        za4b2=((z53-z(jx+1,jy-2))*b2-z4b2)*a4
        if(jyml.eq.0)go to 340
  330   za4b4=((z(jx+1,jy+1)-z54)*b4-z4b4)*a4
        if(jym2.ne.0) go to 380
        za4b2=za4b3+za4b3-za4b4
        go to 380
  340   za4b4=za4b3+za4b3-za4b2
        go to 380
 350    if(jym2.eq.0)go to 360
        za2b2=(z3b2-(z23-z(jx-2,jy-2))*b2)*a2
        if(jyml.eq.0) go to 370
  360   za2b4=(z3b4-(z(jx-2,jy+1)-z24)*b4)*a2
        if(jym2.ne.0) go to 390
        za2b2=za2b3+za2b3-za2b4
        go to 390
  370   za2b4=za2b3+za2b3-za2b2
      go to 390
  380   if(jxm2.ne.0) go to 350
        za2b2=za3b2+za3b2-za4b2
        za2b4=za3b4+za3b4-za4b4
        go to 420
  390   if(jxml.ne.0) go to 420
        za4b2=za3b2+za3b2-za2b2
        za4b4=za3b4+za3b4-za2b4
        go to 420
  400   za2b2=za3b2
        za4b2=za3b2
        za2b4=za3b4
        za4b4=za3b4
        go to 420
  410   za2b2=za2b3
        za2b4=za2b3
        za4b2=za4b3
        za4b4=za4b3
  420   do 480 jy=2,3
         do 470 jx=2,3
         w2=abs(za(jx+2,jy-1)-za(jx+1,jy-1))
         w3=abs(za(jx,jy-1)-za(jx-1,jy-1))
         sw=w2+w3
         if(sw.eq.0.0) go to 430
         wx2=w2/sw
         wx3=w3/sw
         go to 440
  430 wx2=0.5
         wx3=0.5
  440    zx(jx,jy)=wx2*za(jx,jy-1)+wx3*za(jx+1,jy-1)
         w2=abs(zb(jx-1,jy+2)-zb(jx-1,jy+1))
         w3=abs(zb(jx-1,jy)-zb(jx-1,jy-1))
         sw=w2+w3
         if(sw.eq.0.0)  go to 450
         wy2=w2/sw
         wy3=w3/sw
         go to 460
  450    wy2=0.5
         wy3=0.5
  460    zy(jx,jy)=wy2*zb(jx-1,jy)+wy3*zb(jx-1,jy+1)
         zxy(jx,jy)=wy2*(wx2*zab(jx-1,jy-1)+wx3*zab(jx,jy-1))+
     *    wy3*(wx2*zab(jx-1,jy)+wx3*zab(jx,jy))
  470   continue
  480 continue
       if(ix.eq.lxp1) go to 530
       if(ix.ne.1) go to 590
       w2=a4*(3.0*a3+a4)
       w1=2.0*a3*(a3-a4)+w2
       do 500 jy=2,3
         zx(1,jy)=(w1*za(1,jy-1)+w2*za(2,jy-1))/(w1+w2)
        zy(1,jy)=zy(2,jy)+zy(2,jy)-zy(3,jy)
         zxy(1,jy)=zxy(2,jy)+zxy(2,jy)-zxy(3,jy)
         do 490 jx1=2,3
           jx=5-jx1
           zx(jx,jy)=zx(jx-1,jy)
           zy(jx,jy)=zy(jx-1,jy)
           zxy(jx,jy)=zxy(jx-1,jy)
  490    continue
  500  continue
         x3=x3-1.0/a4
       z33=z33-za(2,1)/a4
       do 510 jy=1,5
      zb(2,jy)=zb(1,jy)
  510  continue
      do 520 jy=2,4
         zb(1,jy)=zb(1,jy)-zab(1,jy-1)/a4
  520  continue
       a3=a4
       jx=1
       go to 570
  530  w4=a2*(3.0*a3+a2)
       w5=2.0*a3*(a3-a2)+w4
       do 550 jy=2,3
         zx(4,jy)=(w4*za(4,jy-1)+w5*za(5,jy-1))/(w4+w5)
         zy(4,jy)=zy(3,jy)+zy(3,jy)-zy(2,jy)
         zxy(4,jy)=zxy(3,jy)+zxy(3,jy)-zxy(2,jy)
           do 540 jx=2,3
             zx(jx,jy)=zx(jx+1,jy)
             zy(jx,jy)=zy(jx+1,jy)
             zxy(jx,jy)=zxy(jx+1,jy)
  540      continue
  550    continue
        x3=x4
        z33=z43
        do 560 jy=1,5
           zb(1,jy)=zb(2,jy)
  560    continue
         a3=a2
         jx=3
  570    za(3,1)=za(jx+1,1)
         do 580 jy=1,3
           zab(2,jy)=zab(jx,jy)
  580    continue
  590    if(iy.eq.lyp1)go to 630
         if(iy.ne.1) go to 680
         w2=b4*(3.0*b3+b4)
        w1=2.0*b3*(b3-b4)+w2
         do 620 jx=2,3
           if (jx.eq.3.and.ix.eq.lxp1) go to 600
           if (jx.eq.2.and.ix.eq.1) go to 600
           zy(jx,1)=(w1*zb(jx-1,1)+w2*zb(jx-1,2))/(w1+w2)
           zx(jx,1)=zx(jx,2)+zx(jx,2)-zx(jx,3)
           zxy(jx,1)=zxy(jx,2)+zxy(jx,2)-zxy(jx,3)
  600      do 610 jy1=2,3
            jy=5-jy1
            zy(jx,jy)=zy(jx,jy-1)
            zx(jx,jy)=zx(jx,jy-1)
            zxy(jx,jy)=zxy(jx,jy-1)
  610      continue
  620    continue
         y3=y3-1.0/b4
         z33=z33-z3b2/b4
       za(3,1)=za(3,1)-za3b2/b4
         z3b3=z3b2
         za3b3=za3b2
         b3=b4
         go to 670
  630    w4=b2*(3.0*b3+b2)
         w5=2.0*b3*(b3-b2)+w4
         do 660 jx=2,3
           if(jx.eq.3.and.ix.eq.lxp1) go to 640
           if (jx.eq.2.and.ix.eq.1) go to 640
           zy(jx,4)=(w4*zb(jx-1,4)+w5*zb(jx-1,5))/(w4+w5)
           zx(jx,4)=zx(jx,3)+zx(jx,3)-zx(jx,2)
       zxy(jx,4)=zxy(jx,3)+zxy(jx,3)-zxy(jx,2)
  640      do 650 jy=2,3
           zy(jx,jy)=zy(jx,jy+1)
           zx(jx,jy)=zx(jx,jy+1)
           zxy(jx,jy)=zxy(jx,jy+1)
  650    continue
  660  continue
        y3=y4
        z33=z33+z3b3/b3
       za(3,1)=za(3,1)+za3b3/b3
       z3b3=z3b4
       za3b3=za3b4
       b3=b2
  670  if(ix.ne.1.and.ix.ne.lxp1) go to 680
       jx=ix/lxp1+2
       jx1=5-jx
       jy=iy/lyp1+2
       jy1=5-jy
       zx(jx,jy)=zx(jx1,jy)+zx(jx,jy1)-zx(jx1,jy1)
       zy(jx,jy)=zy(jx1,jy)+zy(jx,jy1)-zy(jx1,jy1)
       zxy(jx,jy)=zxy(jx1,jy)+zxy(jx,jy1)-zxy(jx1,jy1)
  680  zx3b3=(zx34-zx33)*b3
       zx4b3=(zx44-zx43)*b3
       zy3a3=(zy43-zy33)*a3
       zy4a3=(zy44-zy34)*a3
       a=za3b3-zx3b3-zy3a3+zxy33
       b=zx4b3-zx3b3-zxy43+zxy33
       c=zy4a3-zy3a3-zxy34+zxy33
       d=zxy44-zxy43-zxy34+zxy33
       e=a+a-b-c
       a3sq=a3*a3
       b3sq=b3*b3
       p02=(2.0*(z3b3-zy33)+z3b3-zy34)*b3
       p03=(-2.0*z3b3+zy34+zy33)*b3sq
       p12=(2.0*(zx3b3-zxy33)+zx3b3-zxy34)*b3
       p13=(-2.0*zx3b3+zxy34+zxy33)*b3sq
       p20=(2.0*(za(3,1)-zx33)+za(3,1)-zx43)*a3
       p21=(2.0*(zy3a3-zxy33)+zy3a3-zxy43)*a3
       p22=(3.0*(a+e)+d)*a3*b3
       p23=(-3.0*e-b-d)*a3*b3sq
       p30=(-2.0*za(3,1)+zx43+zx33)*a3sq
       p31=(-2.0*zy3a3+zxy43+zxy33)*a3sq
       p32=(-3.0*e-c-d)*b3*a3sq
       p33=(d+e+e)*a3sq*b3sq
  690  dy=vk-y3
       q0=p00+dy*(p01+dy*(p02+dy*p03))
       q1=p10+dy*(p11+dy*(p12+dy*p13))
       q2=p20+dy*(p21+dy*(p22+dy*p23))
       q3=p30+dy*(p31+dy*(p32+dy*p33)   )
       dx=uk-x3
      w(k)=q0+dx*(q1+dx*(q2+dx*q3))
  700 continue
       return
  710 write (*,99999)
      go to 800
  720 write (*,99998)
      go to 800
  730 write (*,99997)
      go to 800
  740 write (*,99996)
      go to 760
  750 write (*,99995)
  760  ixx=ix
      write (*,99994) ixx,x(ixx)
      go to 800
  770 write (*,99993)
      go to 790
  780 write (*,99992)
  790  ixx=iy
      write (*,99991) ixx,y(ixx)
  800 write (*,99990) lx0,ly0,n0
      return
99999 format(1x/23h  ***  lx=1  or  less./)
99998 format(1x/23h  ***   ly=1  or  less./)
99997 format(1x/22h  ***   n = 0 or less./)
99996 format(1x/27h  ***   identical x values./)
99995 format(1x/33h  ***   x values out of sequence./)
99994 format(7h   ix =,i6,10x,7hx(ix) =,e12.3)
99993 format(1x/27h  ***   identical y values./)
99992 format(1x/33h  ***   y values out of sequence./)
99991 format(7h   iy =,i6,10x,7hy(iy) =,e12.3)
99990 format(7h   lx =,i6,10x,4hly =,i6,10x,3hn =,i7/
     * 36h error detected in routine    itplbv)
      end
