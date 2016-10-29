      subroutine  smtbvf(lp,lx,ly,x,y,z,n,u,v,w)
C...01/09/78
      dimension x(lx),y(ly),z(lx,ly),u(n),v(n),w(n)
      dimension  a(5),b(5),d(2),e(2),q(5,2)
       dimension ox(3),oy(3),o(3,3)
       equivalence (xm1,x10),(x2,y10),(x3,s1),(d2,s2),(d3,s3),(v2,s4),
     *(v3,s5),(r1,s6),(r2,s12),(cc1,s14),(c0,s23),(c1,s34),(c2,s25),
     *(c3,s45),(r0,s16),(r10,s36),(a(1),z00),(a(2),z01),(a(3),z10),
     *(a(4),z11),(a(5),z21),(b(1),z12),(b(2),z20),(b(3),z02),(b(4),z22),
     *(b(5),z001),(exy,z002)
      n0=n
      lx0=lx
      ly0=ly
      lp0=lp
      lp1=lp0+1
      lp2=lp1+1
      lxm1=lx0-1
      lym1=ly0-1
      lxm2=lxm1-1
      lym2=lym1-1
      if(lp0.lt.0)  lp0=0
      if(n0.le.0)  go to 500
      if(lp0.gt.3)  lp0=3
      if(lx0.lt.lp0+2)  go to 510
      if(ly0.lt.lp0+2)  go to 520
      do  10  ix=2,lx0
      if(x(ix-1)-x(ix))  10,530,540
10    continue
      do  20  iy=2,ly0
      if(y(iy-1)-y(iy))  20,550,560
20    continue
      do  30  n1=1,n0
      ux=u(n1)
      uy=v(n1)
      do  31  ix=1,lxm2
      if(ux-x(ix+1))  32,33,31
31    continue
      kx=lxm1
      go to  34
32    kx=ix
      go to 34
33    kx=ix+1
34    do  35  iy=1,lym2
      if(uy-y(iy+1))  36,37,35
35    continue
      ky=lym1
      go to  38
36    ky=iy
      go to 38
37    ky=iy+1
38      if(lp0.eq.1) go to 800
      d(1)=x(kx+1)-x(kx)
      d(2)=y(ky+1)-y(ky)
      e(1)=(ux-x(kx))/d(1)
      e(2)=(uy-y(ky))/d(2)
       if(lp0.ne.0)  go to 39
       exy=e(1)*e(2)
       w(n1)=z(kx,ky)*(exy+1.0-e(1)-e(2))+
     * z(kx,ky+1)*(e(2)-exy)+
     * z(kx+1,ky+1)*exy+z(kx+1,ky)*(e(1)-exy)
       go to 30
39     continue
50     if(lp0.gt.2)  go to 51
       if(kx.gt.1)  go to 52
55     if(ky.ne.lym1)  go to 60
56     ky=lym2
       go to 60
52     if(kx.lt.lxm1)  go to 53
       kx=lxm2
       go to 55
53     if(ky.gt.1)  go to 54
       go to 60
54     if(ky.lt.lym1)  go to 49
       go to 56
51     lym3=lym2-1
       lxm3=lxm2-1
       if(kx.gt.1)  go to 72
75     if(ky.lt.lym2)  go to 60
76     ky=lym3
       go to 60
72     if(kx.lt.lxm2)  go to 73
       kx=lxm3
       go to 75
73     if(ky.gt.1)  go to 74
       go to 60
74     if(ky.lt.lym2)  go to 49
       go to 76
49     i=1
       xm1=(x(kx-1)-x(kx))/d(1)
       x2=(x(kx+2)-x(kx))/d(1)
       x3=(x(kx+3)-x(kx))/d(1)
46     d2=d(i)*d(i)
       q(1,i)=1.0
       q(2,i)=(e(i)-xm1)*d(i)
       v2=e(i)*e(i)
       r1=e(i)-1.0
40     d3=d2*d(i)
       q(3,i)=e(i)*q(2,i)*0.5*d(i)
       r2=r1*r1
       v3=v2*e(i)
       if(lp0.gt.2)  go to 42
       q(4,i)=(x2-xm1)*v3*(r1-2*r2)/6.0*d3
      go to 77
42     q(4,i)=(e(i)-1.0)*q(3,i)/3.0*d(i)
       cc1=1.0+x2
       c0=x2-cc1+1
       c1=2.0*cc1-3.0*x2-1.0
       c2=6.0*x2-3.0*cc1+1.0
       c3=4.0*cc1-10.0*x2-2.0
       q(5,i)=(x3-xm1)*v3*e(i)*(c0+c1*r1+
     * (c2+c3*r1)*r2)/24.0*d3*d(i)
77    go to (44,45),i
44     xm1=(y(ky-1)-y(ky))/d(2)
       x2=(y(ky+2)-y(ky))/d(2)
       x3=(y(ky+3)-y(ky))/d(2)
       i=2
       go to 46
45       continue
100   continue
       r10=0.
       kx=kx-1
       ky=ky-1
       kxm1=kx-1
       kym1=ky-1
       do  41  ix=1,lp2
       call oper(ix,kx,x,a)
       do  43  iy=1,lp2
       call oper(iy,ky,y,b)
       r0=0.0
       do  47  i=1,ix
       iz=kxm1+i
       do  48  j=1,iy
       jz=kym1+j
48     r0=r0+z(iz,jz)*a(i)*b(j)
47     continue
43     r10=r10+r0*q(ix,1)*q(iy,2)
41     continue
       w(n1)=r10
       go to 30
60     q(1,1)=1.0
       q(1,2)=1.0
       q(2,1)=ux-x(kx)
       q(2,2)=uy-y(ky)
       q(3,1)=q(2,1)*(ux-x(kx+1))*0.5
       q(3,2)=q(2,2)*(uy-y(ky+1))*0.5
       if(lp0.eq.2)  go to 61
      q(4,1)=q(3,1)*(ux-x(kx+2))/3.0
       q(4,2)=q(3,2)*(uy-y(ky+2))/3.0
61       continue
101   continue
       r10=0.
       kxm1=kx-1
       kym1=ky-1
       do  62  ix=1,lp1
       call oper(ix,kx,x,a)
       do  63  iy=1,lp1
       call oper(iy,ky,y,b)
       r0=0.0
       do  64  i=1,ix
       iz=kxm1+i
       do  65  j=1,iy
       jz=kym1+j
65     r0=r0+z(iz,jz)*a(i)*b(j)
64     continue
63     r10=r10+r0*q(ix,1)*q(iy,2)
62     continue
       w(n1)=r10
       go to 30
800    continue
      ix=kx
       iy=ky
       ix1=ix+1
       iy1=iy+1
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
       o(1,2)=z(ixm1,1)
       o(1,3)=z(ixm1,2)
       tloc=(oy(1)-y(3))/dy
       do 905 loc=1,3
       loc1=ixm1-1+loc
905    o(loc,1)=z(loc1,3)+(z(loc1,2)-z(loc1,1))*tloc
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
       qy2=0.5*(1.0-ym1)*vdy2*(2.0-vy)
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
      w(n1)=z00*o(1,1)+z10*o(2,1)+z01*o(1,2)+z11*o(2,2)+
     +z22*o(3,3)+z20*o(3,1)+z02*o(1,3)+z21*o(3,2)+z12*o(2,3)
30     continue
       return
500    write (*,600) n0
       go to 590
510    write (*,610) lx0
       go to 590
520    write (*,620) ly0
       go to 590
530    ixm1=ix-1
       write (*,630) ixm1,ix
       go to 590
540    ixm1=ix-1
       write (*,640) ixm1,ix
       go to 590
550    iym1=iy-1
       write (*,650) iym1,iy
       go to 590
560    iym1=iy-1
       write (*,660) iym1,iy
       go to 590
570    write (*,670) n1,ux
       go to 590
580    write (*,680) n1,uy
590    write (*,690)
       return
600    format('N=',i2)
610    format('LX=',i3,' LESS LP0+2 ')
620    format('LY=',i3,' LESS LP0+2 ')
630    format('X(',i2,')=X(',i2,')')
640    format('X(',i2,')>X(',i2,')')
650    format('Y(',i2,')=Y(',i2,')')
660    format('Y(',i2,')>Y(',i2,')')
670    format('U(',i3,')=',e9.2)
680    format('V(',i3,')=',e9.2)
690    format('***   error in SMTBVF')
       end
