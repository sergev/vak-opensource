      subroutine gffals(n)
C... gffals  01/12/76  01/09/81
      common/gfgobs/njch,nkch,jg,nris,nach(16)
      n1=n+1
      go to (1,2,3,4,5,6,7,8),n1
      return
    1 write (*,101)
      go to 200
    2 write (*,102)
  200 call endpg('gffals')
      return
    3 write (*,103) njch
      return
    4 write (*,104)
      return
    5 write (*,105)
      return
    6 write (*,106)
      return
    7 write (*,107)
      return
    8 write (*,108)
      return
  101	format(6x,28hgrafor: sli{kom mnogo skobok)
  102	format(6x,29hgrafor: newernoe ~islo skobok)
  103	format(6x,19hgrafor: bank nomer ,i2,10hperepolnen)
  104	format(6x,40hgrafor: ~islo |kranow bolx{e dopustimogo)
  105	format(6x,39hgrafor: ~islo bankow bolx{e dopustimogo)
  106	format(6x,23hgrafor: ne te parametry)
  107	format(6x,37hgrafor: ne tot geometri~eskij |lement)
  108	format(6x,24hgrafor: wse to~ki wydany)
      end
