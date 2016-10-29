      subroutine patc(c,a,n,i,j,p)
C.............o pe e  t  koop  hat  to k  (p) c  a ahh m homepom (i)
C.............ha pa  e ehho  ha n  acte  okpy hoct  (c)
C.............a1 - y"o  k oc  x ( a ah b "pa ycax),  o  kotop m
C.............haxo  tc  ha okpy hoct  i-a  to ka
C............j -  p  hak hymepa    to ek
C.............j > 0 - to k  hymepy8tc   o  acobo  ctpe ke
C............j < 0 - to k  hymepy8tc   pot b  acobo  ctpe k
      dimension c(3),p(2)
      if(n.le.0.or.n.lt.i) return
      delta=360./n
      a1=delta*(i-1)+a
      if(j.gt.0) a1=-delta*(i-1)+a
      call pac(c,a1,p)
      return
       end
