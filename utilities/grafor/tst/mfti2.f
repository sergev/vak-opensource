	parameter (nx=80, ny=40)
	parameter (nx2=nx*2)
	real z (nx, ny), zz (ny, nx), xx (nx), yy (ny)
	real a (nx2), ar (nx2)

	open (41, file='mfti.dat')
	read (41, *) x1, x2, y1, y2, xx, yy, z
	close (41)

	do 10 ix=1,nx
		do 10 iy=1,ny
10                      zz (iy, ix) = z (ix, iy)

c       call setmtf ('mfti2.mtf', 9)
	call init
	call isomet
	call setpen (4)
	call page (32.,22.,'DEMOS',5,1)
	call setpen (5)
	call tdlim (xx, yy, zz, ny, nx, 1, ny, 1, nx, s)
	call region (1., 1., 30., 20., 0, 0, 0)
	call threed (xx, yy, zz, ny, nx, 1, ny, 1, nx, -1, nx, a, ar)
	call endpg (0)
	end
