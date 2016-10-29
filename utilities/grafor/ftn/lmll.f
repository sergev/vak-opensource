      subroutine lmll(lm,l1,l2)
       common/gfgmp/p1(2),p2(2)
        real lm(3),l1(3),l2(3),lx(3)
       dimension  c(3),p(2),p3(2)
        data lx/0.,1.,0./, r/10./
      if(lm(1).eq.0.and.l1(1).eq.0.) go to 2
      call pill(lm,l1,p,n)
      if(n.eq.0) go to 1
      c(1)=p(1)
      c(2)=p(2)
      c(3)=r
         call pilc(l1,c,n)
      call pmlp(lm,p1,p3)
      call lpp(p,p3,l2)
      return
  1   call pill(l1,lx,p,n)
      call pmlp(lm,p,p1)
      call lparlp(lm,p1,l2)
      return
  2   l2(1)=0.
      l2(2)=l1(2)
      l2(3)=2*lm(3)-l1(3)
      return
       end
