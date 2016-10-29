	real x (41), y (41), z (41, 41), a (200), ar (82)
c
	call surf (x, y, z)
	call init
	call isomet
	call setpen (2)
c       call setmtf ('pup.mtf', 7)
	call page (30., 22., 'CAP', 3, 1)
	call setpen (5)
	call tdlim (x, y, z, 41, 41, 1, 41, 1, 41, s)
	call region (5., 7., 20., 8., 0, 0, 0)
	call threed (x, y, z, 41, 41, 1, 41, 1, 41, 0, 100, a, ar)
	call setpen (3)
	call endpg ('pup ')
	end


	subroutine surf (x, y, z)
	real x (41), y (41), z (41, 41)
c
	do 20 i = 1, 41
		x (i) = i
		di = i - 21
		di = di * di
		do 10 k = 1, 41
			y (k) = k
			dk = k - 21
			dk = dk * dk
			p = (di + dk) / 41.
			zz = 1
			if (p .gt. 0.0001) zz = sin (p) / p
			z (i, k) = zz * 41
10                      continue
20              continue
	return
	end
