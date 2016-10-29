       subroutine smtone(lx,ly,x,y,z,mx,my,nu,nv,u,v,w)
C...01/09/78
       dimension x(lx),y(ly),z(lx,ly),u(nu),v(nv),w(nu,nv)
      dimension z1(2),ox(3),oy(3),o(3,3)
       lx0=lx
       ly0=ly
       lxm1=lx0-1
       lym1=ly0-1
       lxm2=lxm1-1
       lym2=lym1-1
       mx0=mx
       my0=my
       nu0=nu
       nv0=nv
       mxp1=mx+1
       myp1=my+1
       mxm1=mx-1
       mym1=my-1
       if(mxm1.le.0) go to 420
       if(mym1.le.0) go to 430
       if(nu0.ne.lxm1*mx0+1) go to 440
       if(nv0.ne.lym1*my0+1) go to 450
       if(lx0.le.2) go to 510
       do 10 ix=2,lx0
       if(x(ix-1)-x(ix)) 10,530,540
10     continue
       if(ly0.le.2) go to 520
       do 20 iy=2,ly0
       if(y(iy-1)-y(iy)) 20,550,560
20     continue
       rmx=1.0/mx0
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
110    continue
       ku=ku+1
       u(ku)=x4
120    continue
       rmy=1.0/my0
       kv=1
       y4=y(1)
       v(1)=y4
       do 141 iy=2,ly0
       y3=y4
       y4=y(iy)
       dv=(y4-y3)*rmy
       do 130 jy=1,mym1
       kv=kv+1
       v(kv)=v(kv-1)+dv
130    continue
       kv=kv+1
       v(kv)=y4
141    continue
       jmx=mx0
       do 30 ix=1,lxm1
       if(ix.eq.lxm1) jmx=mxp1
       jmy=my0
       nx0=(ix-1)*mx0
       do 30 iy=1, lym1
       ny0=(iy-1)*my0
       if(iy.eq.lxm1) jmy=myp1
       do 30 jx=1,jmx
       nx=nx0+jx
       ux=u(nx)
       do 30 jy=1,jmy
       ny=ny0+jy
       uy=v(ny)
       ix1=ix+1
       iy1=iy+1
       ix2=ix1+1
       iy2=iy1+1
       ixm1=ix-1
       iym1=iy-1
       xix=x(ix)
       yiy=y(iy)
       dx=x(ix1)-xix
       dy=y(iy1)-yiy
       vx=(ux-xix)/dx
       vy=(uy-yiy)/dy
       qx1=vx*dx
       qy1=vy*dy
       vdx2=qx1*qx1
       vdy2=qy1*qy1
       ox(1)=xix
       ox(2)=x(ix1)
       oy(1)=yiy
       oy(2)=y(iy1)
       o(1,1)=z(ix,iy)
       o(1,2)=z(ix,iy1)
       o(2,1)=z(ix1,iy)
       o(2,2)=z(ix1,iy1)
       if(ix.eq.lxm1) go to 801
       if(iy.eq.lym1) go to 802
       ox(3)=x(ix2)
       oy(3)=y(iy2)
       o(1,3)=z(ix,iy2)
       o(2,3)=z(ix1,iy2)
       o(3,1)=z(ix2,iy)
       o(3,2)=z(ix2,iy1)
       o(3,3)=z(ix2,iy2)
       go to 703
801    ox(3)=x(lx0)-x(ixm1)+xix
       tloc=(x(ixm1)-ox(3))/dx
       if(iy.eq.lym1) go to 803
       oy(3)=y(iy2)
       o(1,3)=z(ix,iy2)
       o(2,3)=z(ix1,iy2)
       do 804 loc=1,3
       loc1=iym1+loc
804    o(3,loc)=z(ixm1,loc1)+(z(ix,loc1)-z(lx0,loc1))*tloc
       goto 703
802    ox(3)=x(ix2)
       oy(3)=y(ly0)-y(iym1)+yiy
       o(3,1)=z(ix2,iy)
       o(3,2)=z(ix2,iy1)
       tloc=(y(iym1)-oy(3))/dy
       do 805 loc=1,3
       loc1=ixm1+loc
805    o(loc,3)=z(loc1,iym1)+(z(loc1,iy)-z(loc1,ly0))*tloc
       go to 703
803    oy(3)=y(ly0)-y(iym1)+yiy
       o(3,1)=z(ixm1,iy)+(z(ix,iy)-z(lx0,iy))*tloc
       o(3,2)=z(ixm1,ly0)+(z(ix,ly0)-z(lx0,ly0))*tloc
       tloc=(y(iym1)-oy(3))/dy
       o(1,3)=z(ix,iym1)+(z(ix,iy)-z(ix,ly0))*tloc
       o(2,3)=z(lx0,iym1)+(z(lx0,iy)-z(lx0,ly0))*tloc
       o(3,3)=(o(3,2)+o(2,3))*0.5
703    qx2=0.5*vdx2*(vx-1.0)*(ox(3)-ox(1))/dx
       qy2=0.5*vdy2*(vy-1.0)*(oy(3)-oy(1))/dy
       i=1
      go to 170
140    ox(2)=xix
       ox(3)=x(ix1)
       oy(2)=yiy
       oy(3)=y(iy1)
       o(2,2)=z(ix,iy)
       o(2,3)=z(ix,iy1)
       o(3,2)=z(ix1,iy)
       o(3,3)=z(ix1,iy1)
       if(ix.eq.1) go to 901
       if(iy.eq.1) go to 902
       ox(1)=x(ixm1)
       oy(1)=y(iym1)
       o(1,1)=z(ixm1,iym1)
       o(1,2)=z(ixm1,iy)
       o(1,3)=z(ixm1,iy1)
       o(2,1)=z(ix,iym1)
       o(3,1)=z(ix1,iym1)
       go to 710
901    ox(1)=x(1)-x(3)+x(2)
       tloc=(ox(1)-x(3))/dx
       if(iy.eq.1) go to 903
       oy(1)=y(iym1)
       o(2,1)=z(1,iym1)
       o(3,1)=z(2,iym1)
       do 904 loc=1,3
       loc1=iym1-1+loc
904    o(1,loc)=z(3,loc1)+(z(2,loc1)-z(1,loc1))*tloc
       go to 710
902    ox(1)=x(ixm1)
       oy(1)=y(1)-y(3)+y(2)
       go to 710
903    oy(1)=y(1)+y(2)-y(3)
       o(1,2)=z(3,1)+(z(2,1)-z(1,1))*tloc
       o(1,3)=z(3,2)+(z(2,2)-z(1,2))*tloc
       tloc=(oy(1)-y(3))/dy
       o(2,1)=z(1,3)+(z(1,2)-z(1,1))*tloc
       o(3,1)=z(2,3)+(z(2,2)-z(2,1))*tloc
       o(1,1)=(o(1,2)+o(2,1))*0.5
710    xm1=(ox(1)-ox(2))/dx
       ym1=(oy(1)-oy(2))/dy
       qx1=(vx-xm1)*dx
       qy1=(vy-ym1)*dy
       qx2=0.5*(1.0-xm1)*vdx2*(2.0-vx)
       qy2=0.5*(1.0-ym1)*vdy2*(2.0-vy       )
       i=2
170    x10=ox(2)-ox(1)
       y10=oy(2)-oy(1)
       s3=qx2/(ox(3)-ox(1))
       s4=qy2/(oy(3)-oy(1))
       s5=s3/(ox(3)-ox(2))
       s6=s4/(oy(3)-oy(2))
       s3=s3/x10
       s4=s4/y10
       s1=qx1/x10
      s2=qy1/y10
      s12=s1*s2
      s14=s1*s4
      s23=s2*s3
      s34=s3*s4
      s25=s2*s5
      s45=s4*s5
      s16=s1*s6
      s36=s3*s6
      z21=s25+s25-4.*s45
      z12=s16+s16-4.*s36
      z20=s5+s5-z21
      z02=s6+s6-z12
      z22=4.*s5*s6
      z21=z21-z22
      z00=s12-s14-s14-s23-s23+4.*s34
      z001=s2-s4-s4
      z002=s1-s3-s3
      z01=z001-z00-z02
      z10=z002-z00-z20
      z11=z00-z21-z12
      z12=z12-z22
      z00=z00-z001-z002+1.
      z1(i)=z00*o(1,1)+z10*o(2,1)+z01*o(1,2)+z11*o(2,2)+
     +z22*o(3,3)+z20*o(3,1)+z02*o(1,3)+z21*o(3,2)+z12*o(2,3)
       if(i.eq.1) go to 140
       w(nx,ny)=0.5*(z1(1)+z1(2))
30    continue
      return
420    write (*,620)
       go to 590
430    write (*,630)
       go to 590
440    write (*,640)
       go to 590
450    write (*,650)
       go to 590
510   write (*,310)
      go to 590
520   write (*,320)
      go to 590
530   write (*,330)
      go to 590
540   write (*,340)
      go to 590
550   write (*,350)
      go to 590
560   write (*,360)
590   write (*,390)
310   format('   ***  LX LESS 3')
390   format('   ***  ERROR DETECTED IN ROUTINE   SMTONE')
320   format('   ***  LY LESS 3')
 330     format('   ***  X(I) EQUAL X(I+1)')
340   format('   ***  X(I) GREATER X(I+1)')
 350     format('   ***  Y(I) EQUAL Y(I+1)')
360   format('   ***  Y(I) GREATER Y(I+1)')
620    format('   ***  MX LESS TWO')
630    format ('   ***  MY LESS TWO')
 640    format('   ***  NU NOT EQUAL (LX-1)*NX+1')
 650    format('   ***  NV NOT EQUAL (LY-1)*NY+1')
      return
       end
