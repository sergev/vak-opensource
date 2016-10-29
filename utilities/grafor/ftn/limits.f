	subroutine limits (xmin,xmax,ymin,ymax)
C...    subroutine limits 01/09/78
	common /gftab/ irdb (5), rdb (17)
c
	xn = xmin
	xx = xmax
	if (xx.ne.xn) go to 10
	xx = xx+1
	xn = xn-1
  10    yn = ymin
	yx = ymax
	if (yx.ne.yn) go to 20
	yx = yx+1
	yn = yn-1
  20    rdb (11) = xn
	rdb (12) = yn
	rdb (13) = xx
	rdb (14) = yx
	irdb (3) = 1
	return
	end
