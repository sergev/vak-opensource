      subroutine cell(phi,n1,n2)
C...subroutine cell 01/09/78
      dimension phi(n1,n2)
      common/gfcell/i1,j1,i2,j2,d1,d2,kent,qq,const
      fact=1.e-9
      go to(611,612,613,614),kent
  611 i3=i1+1
      i4=i2+1
      j3=j1
      j4=j2
      go to 301
  612 i3=i1
      i4=i2
      j3=j1+1
       j4=j2+1
      go to 301
  613 i3=i1-1
      i4=i2-1
      j3=j1
      j4=j2
      go to 301
  614 i3=i1
      i4=i2
      j3=j1-1
      j4=j2-1
  301 d3=phi(i3,j3)-const
      d4=phi(i4,j4)-const
  302 if(d3*d4)304,303,305
  303 d3=d3+fact
      d4=d4+fact
      go to 302
  304 d1=d3
      d2=d4
      i1=i3
      i2=i4
      j1=j3
      j2=j4
      go to 699
  305 if(d2*d4.ge.0) goto 307
      d1=d4
      i1=i4
      j1=j4
       kent=mod(kent,4)+1
      go to 699
  307 d2=d3
      i2=i3
      j2=j3
       kent=mod(kent+2,4)+1
699   return
      end
