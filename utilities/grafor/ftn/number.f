      subroutine number(x,y,z,fnum,n,th)
	character in
      dimension in(10)
      common /gftab/irdb(5),rdb(17)
      data in/'0','1','2','3','4','5','6','7','8','9'/
      h	= abs(z)
      f	= rdb(17)/rdb(1)
      if(h.lt.0.1*f) return
      if(z.ge.0.) call symbol(x,y,0.1*f,' ',1,th)
      if(fnum.lt.0.) call symbol (0.,0.,h,'-',-1,th)
      f	= abs(fnum)
      nn = n
      q	= 10.**(-nn)
      if(nn.ge.0) goto 1
      i	= (f + 1e-6)/q
      nn = 0
      goto 2
 1    f	= f +0.5*q
      i	= int(f)
      f	= f - i
C  i - tselaja chast chisla
 2    l	= alog10(float(i) + 1e-6) + 1.0 + 1e-6
      if(l.lt.0) l=1
      do 3 j=1,l
	m = i/10**(l-j)
	m = mod(m,10)+1
	call symbol(0.,0.,h,in(m),-1,th)
 3	continue
      if(nn.eq.0) return
C  f - drobnaja chast chisla
      call symbol(0.,0.,h,'.',-1,th)
      do 4 j=1,nn
	f = f*10.
	m = f
	f = f-m
	call symbol(0.,0.,h,in(m+1),-1,th)
 4	continue
      return
      end
