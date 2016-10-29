      subroutine arc(x1,y1,x2,y2,x3,y3,j)
C...subroutine     arc       01/09/73
C...subroutine    arc     01/09/78
       c=57.29578
       v1x=x3-x2
       v1y=y3-y2
       v3x=x1-x2
       v3y=y1-y2
       sp=v1x*v3x+v1y*v3y
       d=v1x*v3y-v3x*v1y
      if(abs(sp).ge.0.000001) goto 9
      an=180.
       ab=(x1-x3)/2.
       or=(y1-y3)/2.
       go to 10
    9  an=atan(abs(d/sp))*c*2.
      if(an.lt.2.) go to 1
      bm1x=(x2+x3)/2.
       bm1y=(y2+y3)/2.
       bm3x=(x1+x2)/2.
       bm3y=(y1+y2)/2.
       r1=v1x*bm1x+v1y*bm1y
       r3=v3x*bm3x+v3y*bm3y
       cx=(r1*v3y-r3*v1y)/d
       cy=(r3*v1x-r1*v3x)/d
       ab=x1-cx
       or=y1-cy
10    if(abs(ab).ge.0.000001) goto 5
      a0 = 90.
      if(or.lt.0.) a0=-a0
       go to 7
    5  a0 = atan(or/ab)*c
      if(ab.lt.0.) a0=a0+180.
    7  r = sqrt(ab**2+or**2)
      if(sp.gt.0.) an=360.-an
      san = 360.-an
      if(d.ge.0.)  go to 15
      an = -an
       san=-san
   15  an=an+a0
       san=san+an
       if(j) 13,16,17
   16  call circle(x1,y1,a0,an,r,r,1)
   13  call circle(x3,y3,an,san,r,r,1)
       return
   17  call circle(x1,y1,a0,an,r,r,1)
       return
    1  call move(x1,y1,0)
       call move(x2,y2,1)
       call move(x3,y3,1)
       return
      end
