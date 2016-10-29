      subroutine tpf(rm,tm,kn,rf,p,x,y)
C...subroutine     tpf       01/09/73
C...subroutine        tpf           01/09/78
      common /gftab/ irdb(5),rdb(17)
      equivalence(a(1),rdb(7))
      dimension a(10)
       nk=kn
      if (irdb(3).le.0) go to 1
      if (irdb(2).le.1) go to 1
      p=tm
      if (iabs(nk).gt.1) p=p/.01745329
      rf=a(3)+(a(4)-a(3))*(rm-a(5))/(a(7)-a(5))
      p=amod(p-a(6),360.)+a(6)
      if (p.lt.a(6)) p=p+360.
      p=a(9)+(p-a(6))*(a(10)-a(9))/(a(8)-a(6))
      p=amod(p-a(9),360.)+a(9)
      if (p.lt.a(9)) p=p+360.
      if(kn) 10,1,11
   11 rf=amax1(amin1(rf,a(4)),a(3))
      if (p.gt.(180.+(a(10)+a(9))/2.)) p=a(9)
      p=amin1(p,a(10))
   10 t=.01745329*p
      x=rf*cos(t)+a(1)
      y=rf*sin(t)+a(2)
    1 return
       end
