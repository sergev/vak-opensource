      subroutine hldln(dx,dy,dz,nlines,i,nc1,nc2,nclust,nlndr,ityp)
C----- "nc1,nc2" - lines compared, if ityp=1;2; or clusters
C     compared, if ityp=3 - 8 (lines from ndr1 to ndr2 are
C     drawn in this case).
      common/gfsv/te(15,2),nl,invsln
      common/gflni/xi(2),yi(2),zi(2)
      common/gfeps/ eps,lengte
      common/gfgtol/g,tol,ndr1,ndr2
      dimension dx(2,nlines),dy(2,nlines),dz(2,nlines)
      dimension nclust(nlndr)
      dimension x(2),y(2),z(2),rc(2),xie(2),yie(2),zie(2)
      dimension an1(6,2),f1(6,2)
C
C  **************************
C..... define the "line i".
C  **************************
C**   print 299,i
C##299   format(//' ***i=',i5)
      xie(1)=dx(1,i)
      xie(2)=dx(2,i)
      yie(1)=dy(1,i)
      yie(2)=dy(2,i)
      zie(1)=dz(1,i)
      zie(2)=dz(2,i)
C**   print 300,i,xie,yie,zie
C##300   format(' ***line "i", i=',i4,6f10.3)
C.....for filtering operations (only |) we use extended line "i" |
      call extln(xie,yie,zie,g)
C**   print 300,i,xie,yie,zie
C
C.....we also use not extended line "i" |
      xi(1)=dx(1,i)
      xi(2)=dx(2,i)
      yi(1)=dy(1,i)
      yi(2)=dy(2,i)
      zi(1)=dz(1,i)
      zi(2)=dz(2,i)
C
      if(ityp.le.2) goto 51
53    j=ndr1
      jj=1
      go to 27
51    j=nc1
C ============================
C      begin of block "j".
C ============================
20    if(i.eq.j) goto 50
C  ************************
C..... define the "line j".
C  ************************
      x(1)=dx(1,j)
      x(2)=dx(2,j)
      y(1)=dy(1,j)
      y(2)=dy(2,j)
      z(1)=dz(1,j)
      z(2)=dz(2,j)
C**   print 301,j,x,y,z
C##301   format('    ===line "j", j=',i4,6f10.3)
C....extend the "line j" at both ends a distance "g" on the picture
C     plane.
      call extln(x,y,z,g)
C**   print 302,j,x,y,z
C##302   format(' extended line "j":',i5,6f10.3)
C
C
C ====================================
C    begin the filtering operations
C ====================================
      if(y(2).gt.yie(1)) goto 50
      if(y(1).lt.yie(2)) goto 50
C***  print 310
C##310   format(' "y" not filtered')
C....if both "x" coords of line "j" are less than these of line "i",-
C    go to the next "j".
      if(x(1).ge.xie(1)) goto 1
      if(x(1).ge.xie(2)) goto 1
      if(x(2).ge.xie(1)) goto 1
      if(x(2).ge.xie(2)) goto 1
      goto 50
1     continue
C***  print 311
C##311   format(' "x less" not filtered')
C....if both "x" coords of line "j" are greater than these of line "i",-
C     go to the next "j".
      if(x(1).le.xie(1)) goto 2
      if(x(1).le.xie(2)) goto 2
      if(x(2).le.xie(1)) goto 2
      if(x(2).le.xie(2)) goto 2
      goto 50
C
2     continue
C***  print 312
C##312   format(' "x greater" not filtered')
C....if both "z" coords of line "j" are greater than these of line "i",-
C     go to the next "j".
      if(z(1).le.zie(1)) goto 3
      if(z(1).le.zie(2)) goto 3
      if(z(2).le.zie(1)) goto 3
      if(z(2).le.zie(2)) goto 3
      goto 50
C
3     continue
C**   print 313
C##313   format(' "z" not filtered')
C
C -----------------------------------------------------------------
C    test to determine if the projection of line "j" crosses the
C     projection of line "i".
C -----------------------------------------------------------------
      a1=yie(2)-yie(1)
      b1=xie(1)-xie(2)
      a2=y(2)-y(1)
      b2=x(1)-x(2)
C
      d=a1*b2-a2*b1
C.....test: are the lines collinear to each other
C           _______________________________________
      if(abs(d).le.eps) goto 50
      c1=xie(2)*yie(1)-xie(1)*yie(2)
      c2=x(2)*y(1)-x(1)*y(2)
C.....compute the intersection point "rc":
      rc(1)=(c2*b1-b2*c1)/d
      rc(2)=(c1*a2-c2*a1)/d
C**   print 305,a1,b1,a2,b2,d,c1,c2,rc
C##305   format(' cross projection: a1,b1,a2,b2,d,c1,c2,rc(1,2):',9f8.3)
C ---------------------------------------------------------------------
C    test: are these line segments cross each other,
C          i. e. does the point "rc" belong to both line segments
C ---------------------------------------------------------------------
C
C..... test the line "i".
      call crossp(xie,yie,rc,ti)
C**   print 306,ti
C##306   format(' ti=',f10.4)
C    test: does the point "rc" belong to the line "i"
      if((ti.le.(-eps)).or.(ti.ge.(1.+eps))) goto 50
C
C..... test the line "j".
      call crossp(x,y,rc,tj)
C**   print 307,tj
C##307   format(' tj=',f12.4)
C     test: does the point "rc" belong to the line "j"
      if((tj.le.(-eps)).or.(tj.ge.(1.+eps))) goto 50
C -----------------------------------------------------------
C    compare depths of lines "i" and "j" at crossing point.
C -----------------------------------------------------------
      rc(1)=zie(1)+ti*(zie(2)-zie(1))
      rc(2)=z(1)+tj*(z(2)-z(1))
C**   print 308,rc
C##308   format(' depth of crossing:',2f10.3)
C    if line "j" isnot at least "tol" distance in front of line "i",-
C     go to next "j".
      if((rc(1)-rc(2)).lt.tol) goto 50
C =======================================
C     end of the filtering operations
C =======================================
C
C
C *******************************************************************
C    the halo of line "j" can influence the visibility of line "i".
C    the non extended line "i" is clipped against the 6-corner
C    halo of line "j".
C *******************************************************************
C
C -------------------------------------------------------------------
C    define inner normals and boundary points for halo around line "j".
C -------------------------------------------------------------------
C.... inner normals:
      a1=y(2)-y(1)
      b1=x(1)-x(2)
      d=sqrt(a1*a1+b1*b1)
C**   print 320,a1,b1,d
C##320   format(' norm. a1,b1,d=',3f8.3)
      an1(1,1)=a1/d
      an1(1,2)=b1/d
      an1(4,1)=-an1(1,1)
      an1(4,2)=-an1(1,2)
      an1(6,1)=an1(1,1)-an1(1,2)
      an1(6,2)=an1(1,1)+an1(1,2)
      an1(5,1)=-an1(6,2)
      an1(5,2)=an1(6,1)
      an1(2,1)=-an1(5,1)
      an1(2,2)=-an1(5,2)
      an1(3,1)=-an1(6,1)
      an1(3,2)=-an1(6,2)
C
C..... boundary points:
      a1=g*an1(1,1)
      b1=g*an1(1,2)
C**   print 321,a1,b1,g
C##321   format(' bound. a1,b1,g=',3f8.3)
      f1(1,1)=dx(1,j)-a1
      f1(1,2)=dy(1,j)-b1
      f1(6,1)=f1(1,1)
      f1(6,2)=f1(1,2)
      f1(2,1)=x(2)
      f1(2,2)=y(2)
      f1(3,1)=x(2)
      f1(3,2)=y(2)
      f1(4,1)=dx(1,j)+a1
      f1(4,2)=dy(1,j)+b1
      f1(5,1)=f1(4,1)
      f1(5,2)=f1(4,2)
C++   print 326,((an1(ii,jj),jj=1,2),ii=1,6)
C##326   format(' an1:'/(2f15.3))
C++   print 327,((f1(ii,jj),jj=1,2),ii=1,6)
C##327   format(' f1:'/(2f15.3))
C ---------------------------
C     clipping algorithm :
C ---------------------------
      d1=xi(2)-xi(1)
      d2=yi(2)-yi(1)
      al1=0.
      au1=1.
      do 73 k=1,6
      w1=xi(1)-f1(k,1)
      w2=yi(1)-f1(k,2)
      d=d1*an1(k,1)+d2*an1(k,2)
      w=w1*an1(k,1)+w2*an1(k,2)
C++   print 356,k,d,w,w1,w2,al1,au1,t
C##356   format('*clip:k=',i2,'d=',f12.2,'w=',f12.2,'w1=',2f8.2,'al,
C##     'au,t=',3f10.5)
      if(abs(d).le.eps) goto 69
      t=-w/d
C++   print 357,t
C##357   format(' t=',f15.2)
C.... (-eps,+eps) - is "zero" interval |
      if(d.gt.eps) goto 71
      if(t.lt.(-eps)) goto 86
      au1=amin1(t,au1)
      goto 73
69    if(w.lt.(-eps)) goto 86
      goto 73
71    if(t.gt.(1.+eps)) goto 86
      al1=amax1(t,al1)
C++   print 358,t,al1,au1
C##358   format(' t=',f15.2,' al1=',f10.5,' au1=',f10.5)
73    continue
C++   print 358,t,al1,au1
C....normal exit
      if(al1.ge.au1) goto 86
C**   print 359,al1,au1
C##359   format('invisible part of line"i"caused by line"j"is: al1=',
C##     ,f10.5,' au1=',f10.5)
C ==========================================================
C    save invisible parts of line "i" in array "te(15,2)":
C ==========================================================
      call storin (al1,au1)
C**   print 390,nl,invsln,((te(ii,jj),jj=1,2),ii=
C**  =1,5)
C##390   format('nl,invsln=',2i3,'a half of"te(15,2)"(after):'/(2f10.5))
C....test: is line "i" visible
C##390   format('nl,invsln=',2i3,'a half of"te(15,2)"(after):'/(2f10.5))
C....test: is line "i" visible
C     (if not, we don't test further lines "j" | )
      if(invsln.eq.0) return
      goto 50
86    continue
C....abnormal exit
C**   print 360,al1,au1
C##360   format('the line"j"cannot influence the visibility of line"i";
C##     ; al1=',f10.5,' au1=',f10.5)
50    continue
C =====================
C....end of loop "j".
C =====================
      if(ityp.gt.2) goto 23
C....regimes 1 - 2.
21    if(j.ge.nc2) return
      j=j+1
      goto 20
C.....regimes 3 - 8.
23    if(j.ge.ndr2) return
      j=j+1
      jj=jj+1
27    if((nclust(jj).eq.nc1).or.(nclust(jj).eq.nc2)) goto 20
      goto 23
      end
