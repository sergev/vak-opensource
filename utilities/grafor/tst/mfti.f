	parameter (nx=80, ny=40, nl=7)
	parameter (nb=nx*2+ny*2-3)
	integer ix (nb), iy (nb), iarea (nx,ny)
	common z (nx,ny), xx (nx), yy (ny), cont (nl)

	open (41, file='mfti.dat')
	read (41, *) x1, x2, y1, y2, xx, yy, z
	close (41)

c       call setmtf ('mfti.mtf', 8)
	call setpen (4)
	call page (30., 22., 'DEMOS', 5, 1)
	call setpen (2)
	call region (1., 1., 28., 20., 0, 0, 1)
	call limits (x1, x2, y1, y2)
	call quadra (iarea, ix, iy, nx, ny, nb)
	call levfun (z, nx, ny, cont, nl)
	call setpen (3)
	call isolin (nx, ny, xx, yy, z, iarea, nb, ix, iy, nl, cont, 0)
	call endpg (0)
	end
