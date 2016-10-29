	real r (180), t (180)
c
	do 10 i = 1, 180
		t (i) = 2.*i - 2.
		r (i) = 1. + sin (t (i) * .01745329)
10              continue
	call setpen (2)
c       call setmtf ('card.mtf', 8)
	call page (30., 22., 'CARDIOIDE', 9, 1)
	call polreg (15., 11., 0., 10., 0., 360., 0, 0, 0., 0)
	call setpen (1)
	call limits (0., 2.2, 0., 360.)
	call poline (r, t, 180, -1, 1, 0, 1)
	call limits (0., 2.2, 360., 0.)
	call poline (r, t, 180, -1, 1, 0, 1)
	call setpen (6)
	call raxes ('RADIUS', 6, 0.5, 0.5, 1, 1, 0.)
	call setpen (3)
	call thaxes ('ANGLE', -5, 0.5, 30., 1, 1, 2.1)
	call setpen (5)
	call endpg ('kard')
	end
