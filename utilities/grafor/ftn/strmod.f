       subroutine strmod(j)
C      subroutine strmod   01/06/75
       common/gfgel/gf(28)
       if(j.ne.0) goto 10
       gf(9)=sign(12.-abs(gf(9)),gf(9))
       return
 10    fj=float(j)
       i=iabs(j)
       goto(1,2,3,4),i
 1     gf(9)=sign(1.,fj)
       return
  2    gf(9)=sign(11.,fj)
       return
  3    gf(9)=sign(abs(gf(9)),fj)
       return
  4    do 16 i=1,8
       a=gf(i)
       gf(i)=gf(i+10)
       gf(i+10)=a
 16    continue
       return
       end
