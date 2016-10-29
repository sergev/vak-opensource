      subroutine appoly (x,y,ro,n,cfit,k1,cof1)
C...subroutine     appoly    01/09/73
C...subroutine     appoly     01/09/78
      dimension x(1),y(1),ro(1),cfit(k1),cof1(1)
      na=iabs(n)
      cof1(1)=1.0
      mk=2*k1
      ron=1.
      do 1 jr = 2,mk
    1  cof1(jr)= 0.
      do  10  is=1,k1
      scv1 = 0.
      ai=0.
       sori=0.
      do 2 new=1,na
      call pval (ypol,x(new),cof1,is)
      if (n.ge.0) ron=ro(new)
   2  scv1=scv1+ypol*ypol*ron
      scv1 = sqrt(scv1)
       do 5 i=1,is
    5 cof1(i) = cof1(i)/scv1
      do 6 new=1,na
       xi= x(new)
       call pval (ypol,xi,cof1,is)
      if (n.ge.0) ron=ro(new)
      ai=ai+xi*ypol*ypol*ron
   6  sori=sori+y(new)*ypol*ron
      cfit(is) = 0.0
      do  15  i=1,is
   15 cfit(i)= cfit(i) + sori * cof1(i)
      if(is.eq.k1) goto 11
      isc1=is+1
      cz = 0.0
      do  10  j= 1,isc1
      xtbl = cof1(j)
      cof1(j) = cz - ai*cof1(j) - cof1(k1+j)*scv1
      cof1(k1+j)=xtbl
      cz =xtbl
   10 continue
  11  return
      end
