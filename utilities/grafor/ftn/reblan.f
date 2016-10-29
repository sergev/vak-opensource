      subroutine reblan
C     subroutine    reblan    28/10/73
C...subroutine  reblan    01/09/78
      common/gfatrn/aa1,bb1,cc1,aa2,bb2,cc2,is
      common/gfblan/jj,xx1,xx2,yy1,yy2,ncop,jch,kch,xxx,yyy,ich
      jch=1
      if(kch.le.0) goto 1
      kch=kch-1
      if(kch.ne.0) goto 1
      jj=0
      if(ncop.le.0) goto 1
      iss=is
      is=0
      call move(xx1,yy1,0)
      is=iss
      ncop=0
    1 return
      end
