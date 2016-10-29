       subroutine astep(an,ax,bs,mk,kd)
	common/gftab/irdb(5),rdb(17)
      l1=irdb(4)-1
      c3=10.**5
      as=abs(bs)
       dx=abs(ax-an)
      if(dx-20.*as) 1,3,2
    1 if(dx-as)	2,3,3
    2 k=0
      go to 6
    4 dx=dx*.1
      k=k+1
    6 if(dx-10.) 5,4,4
    7 dx=dx*10.
      k=k-1
    5 if(dx-1.)	7,8,8
   8  if(dx-3.)	9,10,10
   9  a=0.2
      go to  13
   10 if(dx-7.)	11,12,12
   11 a=0.5
      go to  13
   12 a=1.
   13 as=a*10.**k
      bs=as
    3 mk=0
      if(as-.9995) 45,25,25
   25 as=as+.0005
      x=amax1(abs(an),abs(ax))
      if(as-50.) 26,27,27
   26 r=as*10.
      ar=aint(r)
      if(as-10.) 28,29,29
   28 if(r-ar-.1) 29,30,30
   30 bs=.01*aint(r*10.)
      kd=2
      go to 50
   29 r=bs
      bs=.1*ar+.0005
      if(r-aint(bs)-.1)	35,31,31
   31 kd=1
      go to 50
  35  bs=aint(as+.0005)
      kd=0
      go to 50
   34 as=as*.1
      mk=mk+1
   27 if(as-c3)	36,34,34
   36 if (x*.1**mk-c3) 35,33,33
   33 if (as-10.) 35,34,34
   45 if(as-.001) 46,47,47
   47 as=as+.000005
      r=as*1000.
      if(r-aint(r)-.009) 43,49,49
   46 as=as*10.
      mk=mk-1
   49 if(as-.05) 46,48,48
   48 as=as+.0005
   43 kd=3
      bs=.001*aint(as*1000.)
       r=bs*100.+.05
      ar=aint(r)
      if(r-ar-0.09) 44,50,50
   44 kd=2
      bs=.01*aint(r)
       r=bs*10.+.05
      ar=aint(r)
      if(r-ar-0.09) 42,50,50
   42 kd=1
      bs=.1*ar
  50  bs=bs*10.**mk
       return
      end
