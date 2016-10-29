      function angler(delx,dely)
C     function      angler    01/06/75
      rd=180./3.141593
      if(delx) 1,2,3
    3 z=abs(dely/delx)
      if(dely.ge.0.)goto 5
      th=360.-atan(z)*rd
      go to 8
    5 th=atan(z)*rd
      go to 8
    2 if(dely) 6,8,7
    6 th=270.
      go to 8
    7 th=90.
      go to 8
    1 z=abs(dely/delx)
      if(dely.gt.0.)goto 10
      th=180.+atan(z)*rd
      go to 8
   10 th=180.-atan(z)*rd
    8 angler=th
      return
      end
