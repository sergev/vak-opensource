	subroutine mova(dl,th,j)
	common/gfbftr/xtr,ytr
	th1=th*3.141593/180.
	dx=dl*cos(th1)
	dy=dl*sin(th1)
	call move(xtr+dx,ytr+dy,j)
	return
	end
