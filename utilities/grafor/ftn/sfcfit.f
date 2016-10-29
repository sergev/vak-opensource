      subroutine sfcfit(lx,ly,x,y,z,mx,my,nu,nv,u,v,w)
C... sfcfit 01/09/78
      dimension x(lx),y(ly),z(lx,ly),u(nu),v(nv),w(nu,nv)
      dimension za(4,2),zb(5),zab(2,3),zx(2),zy(2),zxy(2)
      equivalence (z3a2,za(1,1)),(z3a3,za(2,1)),(z3a4,za(3,1)),
     * (z3a5,za(4,1)),(z4a2,za(1,2)),(z4a3,za(2,2)),(z4a4,za(3,2)),
     * (z4a5,za(4,2)),(z4b1,zb(1)),(z4b2,zb(2)),(z4b3,zb(3)),
     * (z4b4,zb(4)),(z4b5,zb(5)),(za3b2,zab(1,1)),
     * (za4b2,zab(2,1)),(za3b3,zab(1,2)),(za4b3,zab(2,2)),
     * (za3b4,zab(1,3)),(za4b4,zab(2,3)),(zx43,zx(1)),
     * (zx44,zx(2)),(zy43,zy(1)),(zy44,zy(2)),
     * (zxy43,zxy(1)),(zxy44,zxy(2)),(p00,z33),(p01,zy33),
     * (p10,zx33),(p11,zxy33)
      equivalence (ixm1,jx),(ixml,jy),(du,dv,dx,dy),
     * (fmx,rmx,fmy,rmy,sw,e),(w2,wy2,a,q0),(w3,wy3,b,q1),
     * (wx2,c,q2),(wx3,d,q3),(z3a2,p02),(z4a2,p03),
     * (z4b1,p12),(z4b2,p13),(z4b4,p20),(z4b5,p21),
     * (za3b2,p22),(za3b4,p23)
       lx0=lx
      lxm1=lx0-1
       lxm2=lxm1-1
       ly0=ly
       lym1=ly0-1
       lym2=lym1-1
       mx0=mx
       mxp1=mx0+1
       mxm1=mx0-1
       my0=my
       myp1=my0+1
       mym1=my0-1
       nu0=nu
       nv0=nv
       if(lxm2.lt.0) go to 400
      if(lym2.lt.0) go to 410
      if(mxm1.le.0) go to 420
      if(mym1.le.0)go to 430
      if(nu0.ne.lxm1*mx0+1) go to 440
      if(nv0.ne.lym1*my0+1) go to 450
       ix=2
      if(x(1)-x(2)) 10,460,30
   10  do 20 ix=3,lx0
        if(x(ix-1)-x(ix)) 20,460,470
   20   continue
      go to 50
   30  do 40 ix=3,lx0
        if(x(ix-1)-x(ix)) 470,460,40
   40 continue
   50  iy=2
      if(y(1)-y(2)) 60,490,80
   60  do 70 iy=3,ly0
        if(y(iy-1)-y(iy)) 70,490,500
   70 continue
      go to 100
   80 do 90 iy=3,ly0
        if(y(iy-1)-y(iy)) 500,490,90
   90 continue
  100  fmx=mx0
       rmx=1.0/fmx
       ku=1
      x4=x(1)
      u(1)=x4
       do 120 ix=2,lx0
         x3=x4
        x4=x(ix)
        du=(x4-x3)*rmx
         do 110 jx=1,mxm1
       ku=ku+1
          u(ku)=u(ku-1)+du
  110   continue
       ku=ku+1
        u(ku)=x4
  120  continue
       fmy=my0
       rmy=1.0/fmy
       kv=1
      y4=y(1)
      v(1)=y4
       do 140 iy=2,ly0
       y3=y4
        y4=y(iy)
        dv=(y4-y3)*rmy
       do 130 jy=1,mym1
       kv=kv+1
          v(kv)=v(kv-1)+dv
  130   continue
       kv=kv+1
          v(kv)=y4
  140 continue
       jymx=my0
       kv0=0
       do 390 iy=2,ly0
       iym2=iy-2
       iym3=iym2-1
       iyml=iy-ly0
       iyml1=iyml+1
       ix6=0
        if(iyml.eq.0) jymx=myp1
       jxmx=mx0
       ku0=0
       do 380 ix=1,lx0
       ixm1=ix-1
       ixml=ix-lx0
          if(ixml.eq.0) jxmx=mxp1
          if(ixm1.ne.0) go to 150
          y3=y(iy-1)
          y4=y(iy)
          b3=1.0/(y4-y3)
       b3sq=b3*b3
          if(iym2.gt.0) b2=1.0/(y3-y(iy-2))
          if(iym3.gt.0) b1=1.0/(y(iy-2)-y(iy-3))
      if  (iyml.lt.0) b4=1.0/(y(iy+1)-y4)
          if(iyml1.lt.0) b5=1.0/(y(iy+2)-y(iy+1))
          go to 180
  150  z3a2=z3a3
       z4a2=z4a3
       x3=x4
       z33=z43
       z3b3=z4b3
       a3=a4
       a3sq=a3*a3
       z3a3=z3a4
       z4a3=z4a4
       za3b2=za4b2
       za3b3=za4b3
       za3b4=za4b4
  160   x4=x5
       z43=z53
       z4b1=z5b1
       z4b2=z5b2
       z4b3=z5b3
       z4b4=z5b4
       z4b5=z5b5
       a4=a5
       z3a4=z3a5
       z4a4=z4a5
       za4b2=za5b2
       za4b3=za5b3
       za4b4=za5b4
  170    x5=x6
       z53=z63
       z54=z64
       z5b1=z6b1
       z5b2=z6b2
       z5b3=z6b3
       z5b4=z6b4
       z5b5=z6b5
  180   ix6=ix6+1
          if(ix6.gt.lx0) go to 260
          x6=x(ix6)
          z63=z(ix6,iy-1)
          z64=z(ix6,iy)
          z6b3=(z64-z63)*b3
          if(lym2.eq.0) go to 200
          if(iym2.eq.0) go to 190
          z62=z(ix6,iy-2)
          z6b2=(z63-z62)*b2
          if(iyml.ne.0) go to 190
       z6b4=z6b3+z6b3-z6b2
          go to 210
 190     z65=z(ix6,iy+1)
          z6b4=(z65-z64)*b4
          if(iym2.ne.0) go to 210
       z6b2=z6b3+z6b3-z6b4
          go to 210
  200   z6b2=z6b3
       z6b4=z6b3
  210     if(iym3.le.0) go to 220
          z6b1=(z62-z(ix6,iy-3))*b1
          go to 230
  220   z6b1=z6b2+z6b2-z6b3
  230     if(iyml1.ge.0) go to 240
          z6b5=(z(ix6,iy+2)-z65)*b5
          go to 250
  240   z6b5=z6b4+z6b4-z6b3
  250     if(ix6.eq.1) go to 170
          a5=1.0/(x6-x5)
          z3a5=(z63-z53)*a5
          z4a5=(z64-z54)*a5
          za5b2=(z6b2-z5b2)*a5
          za5b3=(z6b3-z5b3)*a5
          za5b4=(z6b4-z5b4)*a5
          if(ix6.eq.2) go to 160
          go to 280
  260     if(lxm2.eq.0) go to 270
       z3a5=z3a4+z3a4-z3a3
       z4a5=z4a4+z4a4-z4a3
       if(ixml.eq.0) go to 290
       za5b2=za4b2+za4b2-za3b2
       za5b3=za4b3+za4b3-za3b3
       za5b4=za4b4+za4b4-za3b4
          go to 290
  270   z3a5=z3a4
       z4a5=z4a4
          if(ixml.eq.0) go to 290
       za5b2=za4b2
       za5b3=za4b3
       za5b4=za4b4
  280     if(ixm1.ne.0) go to 290
       z3a3=z3a4+z3a4-z3a5
       z3a2=z3a3+z3a3-z3a4
       z4a3=z4a4+z4a4-z4a5
       z4a2=z4a3+z4a3-z4a4
       za3b2=za4b2+za4b2-za5b2
       za3b3=za4b3+za4b3-za5b3
       za3b4=za4b4+za4b4-za5b4
          go to 300
  290   zx33=zx43
       zx34=zx44
       zy33=zy43
       zy34=zy44
       zxy33=zxy43
       zxy34=zxy44
  300   do 350 jy=1,2
            w2=abs(za(4,jy)-za(3,jy))
            w3=abs(za(2,jy)-za(1,jy))
       sw=w2+w3
            if(sw.eq.0.0) go to 310
       wx2=w2/sw
       wx3=w3/sw
            go to 320
  310   wx2=0.5
       wx3=0.5
  320       zx(jy)=wx2*za(2,jy)+wx3*za(3,jy)
            w2=abs(zb(jy+3)-zb(jy+2))
            w3=abs(zb(jy+1)-zb(jy))
       sw=w2+w3
            if(sw.eq.0.0) go to 330
       wy2=w2/sw
       wy3=w3/sw
            go to 340
  330   wy2=0.5
       wy3=0.5
  340   zy(jy)=wy2*zb(jy+1)+wy3*zb(jy+2)
            zxy(jy)=wy2*(wx2*zab(1,jy)+wx3*zab(2,jy))+
     *      wy3*(wx2*zab(1,jy+1)+wx3*zab(2,jy+1))
  350     continue
          if(ixm1.eq.0) go to 380
          zx3b3=(zx34-zx33)*b3
          zx4b3=(zx44-zx43)*b3
          zy3a3=(zy43-zy33)*a3
          zy4a3=(zy44-zy34)*a3
       a=za3b3-zx3b3-zy3a3+zxy33
       b=zx4b3-zx3b3-zxy43+zxy33
       c=zy4a3-zy3a3-zxy34+zxy33
       d=zxy44-zxy43-zxy34+zxy33
       e=a+a-b-c
          p02=(2.0*(z3b3-zy33)+z3b3-zy34)*b3
          p03=(-2.0*z3b3+zy34+zy33)*b3sq
          p12=(2.0*(zx3b3-zxy33)+zx3b3-zxy34)*b3
          p13=(-2.0*zx3b3+zxy34+zxy33)*b3sq
          p20=(2.0*(z3a3-zx33)+z3a3-zx43)*a3
          p21=(2.0*(zy3a3-zxy33)+zy3a3-zxy43)*a3
          p22=(3.0*(a+e)+d)*a3*b3
          p23=(-3.0*e-b-d)*a3*b3sq
          p30=(-2.0*z3a3+zx43+zx33)*a3sq
          p31=(-2.0*zy3a3+zxy43+zxy33)*a3sq
          p32=(-3.0*e-c-d)*b3*a3sq
          p33=(d+e+e)*a3sq*b3sq
       do 370 jy=1,jymx
       kv=kv0+jy
       dy=v(kv)-y3
            q0=p00+dy*(p01+dy*(p02+dy*p03))
            q1=p10+dy*(p11+dy*(p12+dy*p13))
            q2=p20+dy*(p21+dy*(p22+dy*p23))
            q3=p30+dy*(p31+dy*(p32+dy*p33))
       do 360 jx=1,jxmx
       ku=ku0+jx
              dx=u(ku)-x3
              w(ku,kv)=q0+dx*(q1+dx*(q2+dx*q3))
  360       continue
  370     continue
       ku0=ku0+mx0
  380   continue
       kv0=kv0+my0
  390 continue
      return
  400 write (*,99999)
      go to 520
  410 write (*,99998)
      go to 520
  420 write (*,99997)
      go to 520
  430 write (*,99996)
      go to 520
  440 write (*,99995)
      go to 520
  450 write (*,99994)
      go to 520
  460 write (*,99993)
      go to 480
  470 write (*,99992)
  480 write (*,99991) ix,x(ix)
      go to 520
  490 write (*,99990)
      go to 510
  500 write (*,99989)
  510 write (*,99988) iy,y(iy)
  520 write (*,99987) lx0,mx0,nu0,ly0,my0,nv0
      return
99999 format(1x/23h  ***   lx = 1 or less./)
99998 format(1x/23h  ***   ly= 1 or less. /)
99997 format(1x/23h  ***   mx = 1 or less./)
99996 format(1x/23h  ***   my = 1 or less./)
99995 format(1x/26h  ***   improper nu value./)
99994 format(1x/26h  ***   improper nv value./)
99993 format(1x/27h  ***   identical x values./)
99992 format(1x/33h  ***   x values out of sequence./)
99991 format(7h   ix =,i6,10x,7hx(ix) =,e12.3)
99990 format(1x/27h  ***   identical y values./)
99989 format(1x/33h  ***   y values out of sequence./)
99988 format(7h   iy =,i6,10x,7hy(iy) =,e12.3)
99987 format(7h   ly =,i6,10x,4hmx =,i6,10x,4hnu =,i6/
     *7h   ly =,i6,10x,4hmy =,i6,10x,4hnv =,i6/6h error,
     *30h detected in routine    sfcfit)
      end
