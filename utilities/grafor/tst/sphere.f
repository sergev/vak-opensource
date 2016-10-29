	real x (41), y (41), a (41, 41), b (41, 41)
	real xf1 (82), yf1 (41), rmax (500), rmin (500)
c
	x (1) = -2.
	y (1) = -2.
	do 10 i = 2, 41
		x (i) = x (i-1) + 0.1
		y (i) = x (i)
10              continue
	do 60 i = 1, 41
		do 50 j = 1, 41
			d = x(i) * x(i) + y(j) * y(j) - 4.
			a (i, j) = 0.
			if (d .lt. 0.) a (i, j) = sqrt (-d)
			b (i, j) = - a (i, j)
50                      continue
60              continue
	call setpen (4)
c       call setmtf ('sphere.mtf', 10)
	call page (30., 22., 'SPHERE', 6, 1)
	call setpen (3)
	call region (5., 3., 20., 16., 0, 0, 0)
	call mnmx (-50., -75., 40., x, y, a, 41, 41, 1, 41, 1, 41, s)
	call mnmx (-50., -75., 40., x, y, b, 41, 41, -1, 41, 1, 41, s)
	call prsp (-50., -75., 40., x, y, a, 41, 41, 1, 41, -1, 41,
     *          xf1, yf1, rmax, rmin, 500)
	call prsp (-50., -75., 40., x, y, b, 41, 41, -1, 41, -1, 41,
     *          xf1, yf1, rmax, rmin, 500)
	call endpg ('    ')
	end
