	subroutine notch1
	common /gfntch/ chekx (16), cheky (16), chek (16), xx3, yy3, nsign
	common /gfgobs/ njch, nkch, jg, nris, nach (16)
	common /gfnu/ inp (16,4), anp (16)
c
	jax = inp (njch, 1)
	jay = inp (njch, 2)
	jansz = inp (njch, 4)
	n = inp (njch, 3)
	step = anp (njch)
	if (n.le.0) return
	if (nach (njch) / 8 .le. 0) return
	if (nach (njch) - 2*(nach (njch) / 2) .le. 0) goto 3
	if (njch.ge.nkch) goto 7
	ll = nkch - njch
	lbank = 0
	do 12 l=1,ll
   12           lbank = lbank + nach(njch+l)/4 - (nach(njch+l)/8) * 2
	if (lbank.lt.ll) return
    7   rad = sqrt((xx3-chekx(njch))**2+(yy3-cheky(njch))**2)
	if (rad+chek(njch)-abs(step).lt.0.0000001) then
		chek (njch) = chek (njch) + rad
		chekx (njch) = xx3
		cheky (njch) = yy3
		return
	endif
	if (step.ge.0) goto 2
	if (nsign.le.0) goto 2
	chekx(njch) = chekx(njch)-(step+chek(njch))*(xx3-chekx(njch))/rad
	cheky(njch) = cheky(njch)-(step+chek(njch))*(yy3-cheky(njch))/rad
	chek(njch) = 0
	iknp = ipeek (jansz, 1) + 1
	call ipoke (jansz, 1, iknp)
	nsize = iknp
	if (nsize.gt.n) goto 4
	call poke (jax, nsize, chekx (njch) * nsign)
	call poke (jay, nsize, cheky (njch) * nsign)
	go to 7
    2   iknp = ipeek (jansz, 1) + 1
	call ipoke (jansz, 1, iknp)
	nsize = iknp
	if (nsize.gt.n) goto 4
    3   nsize = ipeek (jansz, 1)
	call poke (jax, nsize, xx3 * nsign)
	call poke (jay, nsize, yy3 * nsign)
	chek (njch) = 0.
	chekx (njch) = xx3
	cheky (njch) = yy3
	return
c
    4   call gffals (2)
	call ipoke (jansz, 1, n)
	nach (njch) = nach (njch) - 8
	return
	end
