      subroutine arcang(r,delx,dely,j,th0a,thfa)
C     subroutine    arcang    01/06/75
      rd=180./3.141593
      r1=abs(r)
      cl=sqrt(delx**2+dely**2)
      if(ivest(2*r1,cl,0.000001  )) 16,103,1
  103 a=0
      go to 2
    1 z=sqrt((r1-cl/2)/(r1+cl/2))
      a=2*rd*atan(z)
      if(ivest(abs(dely),0.,.000001  )) 3,22,2
   22 if(ivest(abs(delx),0.,.000001  )) 3,15,4
    2 if(dely) 5,4,5
    4 if(delx) 6,15,7
    6 th0=a
      thf=180.-a
      go to 100
    7 th0=180.-a
      thf=a
      go to 200
    5 if(delx.ne.0.) go to 11
      if(dely) 9,3,10
    9 th0=90.-a
      thf=270.+a
      go to 200
   10 th0=270.+a
      thf=90.-a
      go to 100
   11 z=abs(dely)/abs(delx)
      d=atan(z)*rd
      q1=a-d
      q2=a+d
      if(dely) 12,3,17
   12 if(delx) 20,3,21
   17 if(delx) 18,3,19
   18 th0=q1
      thf=180.-q2
      go to 100
   19 th0=180.-q1
      thf=q2
      go to 200
   20 th0=q2
      thf=180.-q1
      go to 100
   21 th0=180.-q2
      thf=q1
      go to 200
  100 if(j) 3,13,14
   13 if(r) 102,3,101
   14 if(r) 101,3,102
  200 if(j) 3,14,13
  101 th0a=th0
      thfa=thf
      go to 300
  102 th0a=thf+180.
      thfa=th0+180.
  300 if(ivest(th0a,0.,360.).eq.0)goto 302
      th0a=sign(abs(th0a)-360.,th0a)
  302 if(ivest(thfa,0.,360.).eq.0)goto 3
      thfa=sign(abs(thfa)-360.,thfa)
    3 return
   15 th0a=0.
      thfa=360.
      return
   16 r=0
      return
      end
