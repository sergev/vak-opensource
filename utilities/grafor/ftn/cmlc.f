      subroutine cmlc(lm,c1,c2)
C.....okpy hoct  (c2),  o y ehha  oto pa eh em okpy hoct  (c1)
C.....othoc te  ho  p mo  (lm)
      real c1(3),c2(3),lm(3)
      call pmlp(lm,c1,c2)
      c2(3)=c1(3)
      return
      end
