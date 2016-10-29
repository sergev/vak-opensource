	real x (100), y (100), z (100)

	x (1) = -1.9
	y (1) = sin (x (1))
	z (1) = cos (x (1)) * 1.5
	do 5 i=2,90
		x (i) = x (i-1) + 0.1
		y (i) = sin (x (i))
		z (i) = cos (x (i)) * 1.5
   5            continue
c       call setmtf ('sample.mtf', 10)
	call setpen (2)
	call page (15., 20., 'PAGE', 4, 1)
	call setpen (3)
	call limits (-2., 7., -1.5, 1.5)
	call region (3., 3., 10., 15., 'REGION', 6, 1)
	call setpen (6)
	call axes ('X axis', 6, 1., 5, 'Y axis', 6, 0.3, 4, 0)
	call setpen (5)
	call linemo (x, y, 85, 2, 10)
	call broken (0.5, 0.2, 0.3, 0.2)
	call brline (x, z, 85)
	call endpg (0)
	end
