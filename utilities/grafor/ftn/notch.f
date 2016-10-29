	subroutine notch (x, y, n, nsize, step, j, itr)
	common  /gfgobs/ njch, nkch, jg, nris, nach (16)
	common  /gfnu/ inp (16, 4), anp (16)
	dimension x (n), y (n)
c
	jg = 1
	nkch = nkch+1
	if (nkch.eq.17) goto 2
	inp (nkch,1) = iadr (x)
	inp (nkch,2) = iadr (y)
	inp (nkch,3) = n
	inp (nkch,4) = iadr (nsize)
	nach (nkch) = nach (nkch) + 2*itr + 4*j
	anp (nkch) = step
	if (j.eq.0) nris = 0
	nsize = 1
	return
 2      nkch = 16
	call gffals (4)
	return
	end
