       subroutine hcsurf(x,y,z,nrow,ncol,ista,ifn,jsta,jfn,
     *                    lnt,movx,movy,amxmn,ar)
C      subroutine hcsurf 01.09.78
       dimension z(nrow,ncol),x(1),y(1),amxmn(1),ar(1)
       common/gfsur/nar,dx,ngrd,ncall
      ist=iabs(ista)
      jst=iabs(jsta)
      ix=jst
      iy=ist
      if(movx.le.0)ix=jfn
      if(movy.le.0)iy=ifn
      if(ista.lt.0)goto 13
      if(ista.eq.0)goto 14
      if(jsta.lt.0)goto 13
      if(jsta.eq.0)goto 14
      do 1 i=1,ngrd
      amxmn(i)=0.
      j=i+ngrd
      amxmn(j)=1.
    1 continue
   13 if(lnt.lt.0)goto 3
      if(lnt.eq.0)goto 14
      n=nrow*movx
      igo=1
      j1=jst
      i1=iy
      nline=ifn-ist+1
      i=jfn-jst+1
      iy=-iy
      goto 4
    3 n=movy
      igo=2
      i1=ist
      j1=ix
      nline=jfn-jst+1
      i=ifn-ist+1
      ix=-ix
    4 np=isign(i,ista)
       do 5 i=1,nline
       ncall=i
       call tdsect(z(i1,j1),x,y,ix,iy,np,n,amxmn,ar)
      if(igo.eq.2)goto 7
      i1=i1+movy
      iy=iy-movy
      goto 5
    7 j1=j1+movx
      ix=ix-movx
    5 continue
   14 return
      end
