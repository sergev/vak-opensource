	block data
C
	common  /gftab/ irdb (5), rdb (17)
C
	common  /gfatrm/ a (6), b (6), n
	common  /gfatrn/ r (6), is
C
	common  /gfblan/ jj, xx1, xx2, yy1, yy2, ncop,
     *                  jch, kch, xxx, yyy, ich
C
	common  /gfntch/ chek3 (50), nsign
	common  /gfgobs/ njch, nkch, jg, nris, nach (16)
C
	common  /gfgel/ gf (28)
C
	common  /gferr/ nn, iter (4)
C
	common  /gfbet/ kt, istep, size
C
	data            irdb    /3*0, 4, 4/
	data            rdb     /1000., 0., 32.767, 32.767, 12*0., 1000./
C
	data            a       /1., 3*0., 1., 0./
	data            b       /1., 3*0., 1., 0./
	data            n       /0/
	data            r       /1., 3*0., 1., 0./
	data            is      /0/
C
	data            jj      /0/
	data            xx1     /0./
	data            xx2     /0./
	data            yy1     /0./
	data            yy2     /0./
	data            kch     /0/
C
	data            nsign   /0/
	data            nkch    /0/
	data            jg      /0/
	data            nris    /1/
	data            nach    /16*8/
C
	data            gf      /8*0., 1., 18*0., 0.01745329/
C
	data            nn      /0/
C
	data            kt      /0/
	data            istep   /0/
	data            size    /0./
C
        end


	subroutine grinit
	return
        end


	subroutine grend
	return
        end
