	r = 9.
	xc = 30.
	yc = 22.
	yl = -r / 2.
	xl = yl / sqrt (3.)
	call setpen (5)
c       call setmtf ('bant.mtf', 8)
	call page (xc, yc, 'FLOWER', 6, 1)
	call transl (xc/2., yc/2.)
	call beglev
	do 10 i = 1, 3
		call beglev
		do 20 j = 1, 2
			call beglev
			do 30 k = 1, 20
				call move (0., 0., 0)
				call setpen (2)
				call move (0., -r, 1)
				call setpen (3)
				call move (-sqrt(3.)/2. * r, -r/2, 1)
				call setpen (4)
				call move (0., 0., 1)
				call pscale (xl, yl, .8)
				call rotate (xl, yl, -8.)
30                              continue
			call endlev
			call lscale (0., 0., 0., 1., -1.)
20                      continue
		call endlev
		call rotate (0., 0., 120.)
10              continue
	call endlev
	call setpen (1)
	call endpg ('bant')
	end
