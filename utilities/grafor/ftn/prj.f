      subroutine prj(it,x,y,z)
      common/gfmt/t(4,4),tt(4,4),tr(4,4),dpr1(3),itpprj
      dimension dpr(3)
C**   print 10,it,x,y,z
C##10    format(' **prj param.:',i5,3f10.3)
C..... we use right coord. system.
      eps=1.e-8
         dpr1(1)=x
         dpr1(2)=y
         dpr1(3)=z
         itpprj=it
C.....normalization of "vector of direct. of proj.".
      d=sqrt(x*x+y*y+z*z)
      if(abs(d).le.eps) goto 1
      dpr(1)=x/d
      dpr(2)=y/d
      dpr(3)=z/d
C   ====================================================================
C     rotate the view plane so that "vector x,y,z" lies along the z-axis
C      (and the view plane coinsides with xy-plane).
C   ====================================================================
      s=sqrt(dpr(1)*dpr(1)+dpr(3)*dpr(3))
C.....test: are "x" and "z" both equal to zero
      if(s.gt.eps) goto 3
C
C     "s" equals zero.
      call hcinit(t)
      goto 4
C
3     continue
C     "s" - non equal zero.
C.....rotate the "dpr vector" about the y-axis into the yz-plane.
      call strot3(2,dpr(3)/s,dpr(1)/s,t)
C++   print 55,((t(i,j),j=1,4),i=1,4)
C##55    format(' t(4,4):'/(4f11.3))
C.....rotate about the x-axis so the "dpr vector" lies along the z-axis.
4     call strot3(1,s,-dpr(2),tr)
C++   print 55,((tr(i,j),j=1,4),i=1,4)
      call mxmult(t,tr)
C++   print 55,((t(i,j),j=1,4),i=1,4)
C
C.....test: parallel or perspective projection
      if(it.eq.0) goto 5
C.....the matrix for persp.("on orthogonal plane") projection.
C    we must do:  call strow3(0.,0.,-1./d,tr); call mxmult(t,tr);
C                call sclg3d(0.,0.,-1.,0.,tr); call mxmult(t,tr).
C    but more efficiently:
      call hcinit(tr)
      tr(3,4)=-1./d
      tr(3,3)=-1.
C++   print 55,((tr(i,j),j=1,4),i=1,4)
      call mxmult(t,tr)
C++   print 55,((t(i,j),j=1,4),i=1,4)
      return
5     continue
C.....the matrix for parallel (axonometric) projection.
      return
1     continue
      call hcinit(t)
C##   print 2
C##2     format('   ***erroneous "x,y,z"')
      return
      end
