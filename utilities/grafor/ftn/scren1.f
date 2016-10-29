      subroutine scren1(iscr,scrn,idim1,idim2,y,n,ind)
C...01/09/78
      dimension iscr(1),scrn(idim1,idim2,2)
      j=iscr(n)
      if(j.eq.0) go to 5
      ind=0
      do 6 i=1,j
      if(y.le.scrn(n,i,1)) goto 5
      if(y.le.scrn(n,i,2)) goto 9
    6 continue
    5 ind=1
    9 return
      end
