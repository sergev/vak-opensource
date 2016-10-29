	parameter (n=200, w=3.14, h=1.0e10)
	dimension x (n), y (n)
c
	call setpen (2)
	call page (32.0, 22.0, 'PAGE GRAFOR', 11, 1)
	call setpen (7)
	call region (2.0, 2.0, 28.0, 18.0, 'KIAE', 4, 1)
	call limits (-w, w, -h, h)
	call setpen (3)
	call axes ('X', 1, 0.0, 5, 'Y', 1, 0.0, 5, 11)
	call setpen (5)
	do 5 j=1,2
		do 10 i=1,n
			x (i) = w * (2. * (i-1)/(n-1) - 1)
			y (i) = h * sin (x (i) * j)
10                      continue
		call lineo (x, y, n)
5               continue
	call endpg (0)
	end
