       subroutine zapit(itagb,n1,n2)
       dimension itagb(n1,n2)
      common/gfcell/i1,j1,i2,j2,d1,d2,kent,qq,const
      go to(521,522,523,524),kent
  521 itagb(i1,j1)=itagb(i1,j1)+2
      go to 202
  522 itagb(i2,j2)=itagb(i2,j2)+1
      go to 202
  523 itagb(i2,j2)=itagb(i2,j2)+2
      go to 202
  524 itagb(i1,j1)=itagb(i1,j1)+1
202   return
       end
