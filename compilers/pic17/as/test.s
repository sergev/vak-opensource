#	.config FOSC0,FOSC1,WDTPS0,WDTPS1,PM0,PM1,PM2

WREG    .equ    0xA
x       .data   2
s       .const  33,44,55,66,77,88,0

	.org    0
main:   nop
	sleep
	awake
	goto    doit
	ret

	.org    0x10
intr:
	a--?
	aw
	a++?
	a++z?
	a--z?
	a?
	az
	z?
	as
	c?
	call    main
	reti

doit:   bt      x,1&7
	bz?     x,2*2
	bs      x,1|2
	bs?     x,4/2
	bz      x,5%2
	rtx     WREG,x
	xtr     x,WREG

	atx     x+(~10)
	a-xc    x-(010^1)
	x-ac    x
	a-x     x
	x-a     x
	x--a    x
	x--     x
	a|x     x
	x|a     x
	a&x     x
	x&a     x
	a^x     x
	x^a     x
	a+x     x
	x+a     x
	a+cx    x
	x+ca    x
	xca     x
	xc      x
	x++a    x
	x++     x
	x--a?   x
	x--?    x
	xc>>a   x
	xc>>x   x
	xc<<a   x
	xc<<x   x
	xwa     x
	xw      x
	x++a?   x
	x++?    x
	x>>a    x
	x>>x    x
	x<<a    x
	x<<x    x
	x++az?  x
	x++z?   x
	x--az?  x
	x--z?   x
	xza     x
	xz      x
	xsa     x
	xs      x
	anax    x
	anx     x
	adax    x
	adx     x
	a>=x?   x
	a!=x?   x
	a<=x?   x
	x?      x
	a*x     x
	xta     x
	llx     x
	lhx     x
	xll     x
	xhl     x
	plx     x
	pl++x   3f
	xhp     x
	xhp++   x
	cta     x
	a+c     x
	c-a     x
	a|c     x
	a^c     x
3:      a&c     -2
2:      retc    x
	lcall   x
	reg     x
	dat     x
	a*c     x
	goto    2b
