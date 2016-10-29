	parameter (g = 0.05)
	parameter (ityp = 5)
	parameter (m = 3)
	parameter (n = 4)
	parameter (n1 = 110)
	parameter (nn = 8)
	parameter (tol = 0.2)

	parameter (n2 = nn * n1)

	real dx (2, n2), dy (2, n2), dz (2, n2)
	integer nclust (n2)

	h1 = 3.5 / nn
	h2 = 4.5 / nn
	fi = h1
	psi = h2
	do 10 j = 1, n2
		fi = fi + h1 / n1
		psi = psi + h2 / n1
		th = 6.283 * (j-1) / (n1-1)
		x = sin (th)
		y = 1.2 * sin (m*th + fi)
		z = 1.5 * sin (n*th + psi)
		if (j .ne. 1) then
			dx (2, j-1) = x
			dy (2, j-1) = y
			dz (2, j-1) = z
		endif
		if (j .ne. n2) then
			dx (1, j) = x
			dy (1, j) = y
			dz (1, j) = z
		endif
  10    continue

	call setpen (4)
	call page (32., 22., 0, 0, 1)
	call region (6., 1., 20., 20., 0, 0, 0)
	call prjhl (1, 15., 8., 3.)
	call vplim (dx, dy, dz, n2, 1, n2, s)
	call setpen (6)
	call hallne (dx, dy, dz, n2, 1, n2, nclust, n2, g, tol, ityp)
	call endpg (' ')
	end
