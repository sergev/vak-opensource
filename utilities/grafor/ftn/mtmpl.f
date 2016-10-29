      subroutine mtmpl(al1,bl1,cl1,al2,bl2,cl2,
     cat1,bt1,ct1,at2,bt2,ct2,ae1,be1,ce1,ae2,be2,ce2)
      ae1=at1*al1+bt1*al2
      be1=at1*bl1+bt1*bl2
      ce1=at1*cl1+bt1*cl2+ct1
      ae2=at2*al1+bt2*al2
      be2=at2*bl1+bt2*bl2
      ce2=at2*cl1+bt2*cl2+ct2
      return
      end
