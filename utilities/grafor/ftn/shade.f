      subroutine shade(x,y,n,step,eps,beta)
C	shade 01/11/80
      dimension alx(40),aldx(40),aly(40)
      dimension ysort(250),nsort(250),x(n),y(n)
      dimension xtek(2),ytek(2),xf(2),yf(2)
C
C              ha a  ha  yctahobka.
C
      if (n.lt.3)goto 999
      lend=n
      if(lend.gt.250)lend=250
      ksort=1
C      ipntal=0
      step1=abs(step)
      alrad=0.017453293*beta
      eps1=eps
      sa=sin(alrad)
      ca=cos(alrad)
C       Вставка
	call tmf(0.,0.,cx0,cy0)
	call tmf(1.,1.,cx,cy)
	cx=cx-cx0
	cy=cy-cy0
	eps1=eps1/cy
	ca=ca/cx
	sa=sa/cy
	cmd=sqrt(ca*ca+sa*sa)
	ca=ca/cmd
	sa=sa/cmd
	step1=abs(step/(cx*cy*cmd))
C       При переходе от математических координат к физическим
C       искажаются угол и шаг штриховки. Во избежание этого
C       сделано обратное преобразование.
C       Д.А.Мартынов
      idir=1
C ****************************************************
C ***
C              ta :1) obopot. 2)c  cok bep  h.
C              obopot ha y o  beta. atem copt pobka c  cka bep  h  o
C	       bo pactah 8 y-koop  hat.pe y  tat - c  cok ysort
C	       y- ha eh   bep  h.c  cok nsort - c  cok  op  kob x
C	       homepob  t x bep  h.
C
       ysort(1)=(-x(1)*sa+y(1)*ca)-eps1
       nsort(1)=1
       do 12 k=2,lend
	  r=(-x(k)*sa+y(k)*ca)-eps1
	  do 10 j=2,k
	    i=k-j+2
	    if(ysort(i-1).le.r)goto 11
	      ysort(i)=ysort(i-1)
	      nsort(i)=nsort(i-1)
  10	  continue
	  i=1
  11	  ysort(i)=r
	  nsort(i)=k
  12  continue
C
C	        epba    h    tp xobk
C
       ys=step1*(aint((ysort(1)-1e-6)/step1)+sign(0.5,ysort(1))+0.5)
C	       n- a ahhoe k-bo bep  h
C	       l- op  kob   homep bep  h
C	       nsort-c  cok  op  kob x homepob bep  h ( o bo p.
C	       y-koop .)
C	       ksort- homep bep  h  b c  ckax nsort   ysort
C
C	        ta   aheceh   b c  cok hob x pe ep.
C	       otcopt pobahh   c  cok bep  h ysort  pocmatp baetc  c to"
C	       mecta, " e  pocmotp      akoh eh b  pe   . pa (ksort).
C	       ko" a c  cok bep  h  c ep ah, pa ota  po"pamm   abep aetc
C	       ec   y-koop . o epe ho   pocmatp baemo  bep  h  'b  e'
C	       'ys', ta   abep aetc .
C	       ec   he 'b  e',   taemc   ahect  b c  cok pe ep 2 pe pa,
C	       b xo    e     to  bep  h .
  31   if(ksort.gt.lend)goto 999
	   y0=ysort(ksort)
	if(abs(ys-y0).lt.1e-6)go to 777
	if(y0.gt.ys)go to 45
  777	   l= nsort(ksort)
	   x0=(x(l)*ca+y(l)*sa)
	   ksort=ksort+1
	   ln=l-1
	   if(ln.eq.0)ln=lend
	   assign 32 to line
	  goto 399
 32	  ln=l+1
	  if(ln.gt.lend)ln=1
	  assign 31 to line
C **************************************************
C***
C	       ect  pe po c y-koop  hatam  y0 - ha a a   yt - koh a.pe p
C	       he      ahect  b c  cok pe ep, ec  :
C	       1)pe po y e  aheceho
C	       2)oho  apa  e  ho  tp x-  h
C	       3)yt 'h  e'ys
C
 399  if(ipntal.lt.40)goto 398
      ksort=ksort-1
      goto 45
 398  yt=(-x(ln)*sa+y(ln)*ca)-eps1
      if(abs(yt-y0).lt.1e-6)goto 371
	if(abs(ys-yt).lt.1e-6)go to 555
      if(yt.lt.ys)goto 371
  555	 xt=(x(ln)*ca+y(ln)*sa)
	 cotang=(xt-x0)/(yt-y0)
	 ipntal=ipntal+1
	 aldx(ipntal)=cotang*step1
	 alx(ipntal)= x0 +((ys-y0)*cotang)
	 aly(ipntal)=yt
 371  goto line,(31,32)
C *****************************************************
C ***
C	         ta  copt pobk   o x.
C	       ipntal-homep  oc e he   a o h. ctpok  c  cka pe ep.
C
C	       c  cok pe ep copt pyetc   o bo pactah 8 x-koop .
C
 45    if(ipntal.le.1)goto 51
       do 43 k=2,ipntal
	if(abs(alx(k-1)-alx(k)).lt.1e-6)go to 43
	  if(alx(k-1).le.alx(k))goto 43
	     rx=alx(k)
	     ry=aly(k)
	     rdx=aldx(k)
	     j=k
  41	     j=j-1
	     alx(j+1)=alx(j)
	     aly(j+1)=aly(j)
	     aldx(j+1)=aldx(j)
	     if(j.eq.1)goto 42
	if(abs(rx-alx(j-1)).lt.1e-6)go to 42
	     if(rx.lt.alx(j-1))goto 41
  42	     alx(j)=rx
	     aly(j)=ry
	     aldx(j)=rdx
  43	continue
C ***
C *********************************************************************
C	        ta   tp xobk .
C	       idir-ha p.  b  .  epa (1-b pabo)
C	       j,jmax-ha a o   kohe  c  cka x- ha eh   pe ep, epecek.
C	        ahho   tp x-  h e
C	       mount- o h to     epo(1-o y eho)
C	       kl- etho    k-bo pe ep,k-p e  epecek a  ahha   tp x-  h
C	        b "a c  b ha pab eh   idir
C
C	        oka c  cok j-jmax he  c ep ah:
C	       jp  p cba baem  ha . j,  em  o c  cky pe po j takoe, to
C	       x(j) he= x(jp).bo bpem   o cka  p  hak kl meh etc   p
C	        epece eh   ka  o"o pe pa.ec   kl=0, pobo  m otpe ok
C	        tp x-  h   x(jp) - x(j).
C	        atem jp=j,  po o  aem  o ck  o c  cky.
C	       ec   kl=1,jp=j, po o  aem
C	        o ck  o c  cky.
C	        p  hak mount kohtpo  pyet  o o eh e  epa.
C	       ko" a c  cok j-jmax  c ep ah:
C	       1)meh etc  ha p.  b  .  epa (idir),
C	       2) po cxo  t  epexo  k c e .  tp x-  h  (ys=ys+step)
C
   51 if(idir.eq.1)goto 511
	 j=ipntal
	 jmax=1
      goto 512
 511	 j=1
	 jmax=ipntal
 512   mount=0
      kl=1
      jp=j
  505	if(jp.eq.jmax) go to 501
	 j=jp+idir
	 kl=1-kl
 503	 if(abs(alx(jp)-alx(j)).ge.1e-6)goto 502
      if(j.eq.jmax) goto 501
	    j=j+idir
	    kl=1-kl
	    goto 503
 502	 if(kl.ne.0) goto 504
	   el=alx(jp)
	   er=alx(j)
	   jp=j
	   if(mount.eq.0)goto 5031
	     xf(1)=xf(2)
	     yf(1)=yf(2)
	   goto 5032
5031	     xtek(1)=(el*ca-(ys+eps1)*sa)
	     ytek(1)=(el*sa+(ys+eps1)*ca)
	     call tmf(xtek(1),ytek(1),xf(1),yf(1))
	     call move(xf(1),yf(1),0)
	     mount=1
5032	   xtek(2)=(er*ca-(ys+eps1)*sa)
	   ytek(2)=(er*sa+(ys+eps1)*ca)
	   call tmf(xtek(2),ytek(2),xf(2),yf(2))
	   call move(xf(2),yf(2),1)
	   goto 505
 504	 jp=j
	 if(mount.ne.0)mount=0
	 goto 505
 501   idir=-idir
       ys=ys+step1
C******************************************
C****
C	        ta  b  epk bah   'otpa otahh x' pe ep.
C	       ishift - c b "  o c  cky pe ep.
C
C	          c  cka b  epk ba8tc  pe pa,bep  h  k-p x 'h  e' ys.
C	       c  cok  pocmatp baetc   oc e obate  ho.
C
      is=0
      do 63 j=1,ipntal
	if(abs(ys-aly(j)).lt.1e-6)go to 666
	 if(ys.gt.aly(j))goto 63
  666	    is=is+1
	    aly(is)=aly(j)
	    alx(is)=alx(j)+aldx(j)
	    aldx(is)=aldx(j)
  63   continue
       ipntal=is
C	       idir-ha p.  b  .  epa (1-b pabo)
C *********************************************************************
C***
       goto 31
C **************************************************
 999  return
      end
