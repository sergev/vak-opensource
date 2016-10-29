      subroutine lpax(p,a,l)
      real l(3),p(2),x(3)
      data x/0.,1.,0./
       if(abs(a).eq.90..or.abs(a).eq.270.) go to 1
       th=a*0.01745329
       tn=sin(th)/cos(th)
       l(1)=tn
      l(2)=-1.
       l(3)=p(2)-tn*p(1)
      return
  1    call lperlp(x,p,l)
      return
       end
