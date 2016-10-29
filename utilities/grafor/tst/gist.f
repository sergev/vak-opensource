	dimension yv(12)
	data yv/23.,10.,17.,10.,5.,36.,14.,32.,19.,21.,27.,8./
	call minmax(yv,12,ymn,ymx)
	y0=(ymn+ymx)/2
	sm=(ymn+ymx)/10
	call setpen(4)
	call page(32.,22.,'HYSTOGRAMME',11,1)
	call region(2.,2.,28.,18.,0,0,0)
	call limits(0.,12.,ymn,ymx)
	call setpen(3)
	call yaxis(0.,'LEVEL',5,sm,0,0,1)
	call axisc('1973/1974',9,1,2,-12)
	call setpen(2)
	call histgm(0.,1.,y0,yv,-12,4)
	call endpg(0)
	end
