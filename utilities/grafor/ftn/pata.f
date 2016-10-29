       subroutine pata(c,a1,a2,n,i,p)
C.............o pe e  t  koop  hat  to k  (p) c  a ahh m homepom (i)
C.............ha pa  e ehho  ha n  acte   y"e okpy hoct  (c)
C.............a1 - ha a o  y"
C.............a2 - kohe   y"
C............. epba  to ka cob a aet c ha a om  y"
       dimension c(3),p(2)
       if(n.lt.1.or.n.lt.i) return
       if(a2-a1.gt.360.) return
      delta=(a2-a1)/(n-1)
       a=delta*(i-1)+a1
       call pac(c,a,p)
       return
       end
