      subroutine spherg(r,nfi,nteta,dx,dy,dz,nlines,nbeg)
           dimension dx(2,nlines),dy(2,nlines),dz(2,nlines)
C  ***************************************************************
C   "nteta-1" gorizontal circles per "nfi" lines.
C   the total number of lines is "nfi*(nteta-1)".
C   "nfi,nteta" - the number of lines per circle or semi-circle.
C  ***************************************************************
      k=nbeg
      dteta=3.14159265/nteta
      dfi=2.*3.14159265/nfi
C
      teta=0.
      nteta1=nteta-1
      do 2 j=1,nteta1
      teta=teta+dteta
      fi=0.
      do 1 i=1,nfi
      dz(1,k)=r*sin(teta)*cos(fi)
      dx(1,k)=r*sin(teta)*sin(fi)
      dy(1,k)=r*cos(teta)
      fi=fi+dfi
      dz(2,k)=r*sin(teta)*cos(fi)
      dx(2,k)=r*sin(teta)*sin(fi)
      dy(2,k)=r*cos(teta)
      k=k+1
1      continue
2      continue
      return
      end
