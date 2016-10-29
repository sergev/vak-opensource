	parameter (n=660, nc=280)
	real dx (2,n), dy (2,n), dz (2,n)
	integer ncl (nc)
c
	call cilind (0.4, -1., 0.9, 12, 20, dx, dy, dz, n, 1)
	call tranh (1.7, 0., 0., dx, dy, dz, n, 1, nc)
	call spherg (1., 20, 10, dx, dy, dz, n, nc+1)
	call spherv (1., 20, 10, dx, dy, dz, n, nc+1+180)

	call setpen (4)
	call page (32., 22., 'CYLINDER', 8, 1)

	call pscale (3., 3., 0.4)
	call lscale (5., 1., 3., 3., 3.)
	call rotate (8., 6., -30.)

	call region (10., 5., 12., 12., 0, 0, 0)
	call prjhl (1, 10., 5., -3.)
	call vplim (dx, dy, dz, n, 1, n, s)
	call setpen (3)
	call coordt (dx, dy, dz, n, 1, nc, ncl, nc, 3)
	call haloed (dx, dy, dz, n, 1, nc, 1, nc, ncl, nc, 0.1, 0.3, 3)
	call setpen (6)
	call coordt (dx, dy, dz, n, nc+1, n, ncl, 1, 1)
	call haloed (dx, dy, dz, n, nc+1, n, 1, n, ncl, 1, 0.15, 0.5, 1)
	call endpg (0)
	end
