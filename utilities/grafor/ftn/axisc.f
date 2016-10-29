	subroutine axisc (name, nc, mmin, ms, nm)
C.......subroutine axisc 01/09/78
	integer name (1)
	character*3 month (12)
	common /gftab/ irdb (5), rdb (17)
	data month /'QNW', 'FEW', 'MAR', 'APR', 'MAJ', 'I@N',
     *          'I@L', 'AWG', 'SEN', 'OKT', 'NOQ', 'DEK'/
c
	if (irdb (2) .ne. 2) return
	nac = iabs (nc)
	x = rdb (7)
	sig = 1.0
	y1 = rdb (8)
	if (nm) 1,50,2
  1     sig = -1.0
	y1 = rdb (8) + rdb (10)
  2     nam = iabs (nm)
	xb = x
	f = rdb (17) / rdb (1)
	ya = y1 + 0.25 * f * sig
	call move (x, ya, 0)
	do 20 i = 1, nam
		call move (xb, y1, 1)
		xl = rdb (9)
		fl = xl / nam
		xc = xb + fl
		call move (xc, y1, 1)
		call move (xc, ya, 1)
  20            xb = xc
	h = fl / 4.0
	h = amin1 (h, 0.4 * f)
	if (nm) 23, 50, 24
  23    ya = y1 - 0.45 * f
	go to 220
  24    ya = y1 + 0.15 * f
 220    xa = xb - 0.125 * f
	idy = ms
	iy = mmin
	imin = iy + (nam-1) * idy
 200    if (imin .le. 12) goto 21
	imin = imin - 12
	go to 200
  21    nset = irdb (1)
c       set 1 symbol set
	irdb (1) = 1
	do 25 i = 1, nam
		xx = xa - 2 * h
		call symbol (xx, ya, h, month (imin), 3, 0.0)
		imin = imin - idy
		if (imin .le. 0) imin = 12 + imin
 25             xa = xa - fl
c       restore symbol set
	irdb (1) = nset
	tnc = nac
	xa = x + xl/2.0 - 0.1778*f*tnc
	ya = y1 + (-0.07 + sig*0.4) * 2.54 * f
	h1 = 0.5 * f
	call symbol (xa, ya, h1, name, nac, 0.0)
  50    return
	end
