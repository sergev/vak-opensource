	subroutine prjhl(it,x,y,z)
	common/gfmt/t(4,4),tt(4,4),tr(4,4),dpr1(3),itpprj
	dimension dpr(3)

	call hcinit (t)
	d = sqrt(x*x+y*y+z*z)
	if (abs(d).le.1.e-8) return

	dpr1(1) = x
	dpr1(2) = y
	dpr1(3) = z
	itpprj = it

	dpr(1) = x/d
	dpr(2) = y/d
	dpr(3) = z/d
C
C       rotate the view plane so that "vector x,y,z" lies along the z-axis
C       (and the view plane coinsides with xy-plane).
C
C.......test: are "x" and "z" both equal to zero
	s = sqrt(dpr(1)*dpr(1)+dpr(3)*dpr(3))
C.......rotate the "dpr vector" about the y-axis into the yz-plane.
	if (s.gt.1.e-8) call strot3(2,dpr(3)/s,dpr(1)/s,t)
C.......rotate about the x-axis so the "dpr vector" lies along the z-axis.
	call strot3(1,s,-dpr(2),tr)
	call mxmult(t,tr)
C.......test: parallel or perspective projection
	if (it.ne.0) then
C...............the matrix for persp.("on orthogonal plane") projection.
C               we must do:
C                       call strow3(0.,0.,-1./d,tr)
C                       call mxmult(t,tr)
C                       call sclg3d(0.,0.,-1.,0.,tr)
C                       call mxmult(t,tr)
C               but more efficiently:
		call hcinit(tr)
		tr(3,4) = -1./d
		tr(3,3) = -1.
		call mxmult(t,tr)
	endif
	return
	end
