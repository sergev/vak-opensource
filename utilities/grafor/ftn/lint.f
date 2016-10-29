      subroutine lint(x,y,xx,yy,n1,n2,kt,ikop)
C...subroutine lint 01/09/78
      common/gfcell/i1,j1,i2,j2,d1,d2,kent,qq,const
       common/gfint/ is
       dimension x(n1),y(n2),xx(kt),yy(kt)
      go to(531,532,531,532),kent
  531 rd=x(i1)
      tt=y(j1)+(y(j2)-y(j1))/(1.-d2/d1)
       go to 203
532   rd=x(i1)+(x(i2)-x(i1))/(1.-d2/d1)
      tt=y(j1)
  203 xx(is)=rd
      yy(is)=tt
      if(ikop.ne.1) goto 2
       xx(is)=xpol(rd,tt)
      yy(is)=ypol(rd,tt)
    2 return
      end
