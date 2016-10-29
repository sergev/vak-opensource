      subroutine spherv(r,nfi,nteta,dx,dy,dz,nlines,nbeg)
           dimension dx(2,nlines),dy(2,nlines),dz(2,nlines)
C  ***************************************************
C    "nfi" semi-circles per "nteta" lines.
C    the total number of lines is "nfi*nteta".
C  ***************************************************
      k=nbeg
      dteta=3.14159265/nteta
      dfi=2.*3.14159265/nfi
C
      fi=0.
      do 4 i=1,nfi
      teta=0.
      do 3 j=1,nteta
      dz(1,k)=r*sin(teta)*cos(fi)
      dx(1,k)=r*sin(teta)*sin(fi)
      dy(1,k)=r*cos(teta)
      teta=teta+dteta
      dz(2,k)=r*sin(teta)*cos(fi)
      dx(2,k)=r*sin(teta)*sin(fi)
      dy(2,k)=r*cos(teta)
      k=k+1
3      continue
      fi=fi+dfi
4      continue
      return
      end
