       subroutine hallne(dx,dy,dz,nlines,n1,n2,nclust,nlndr,g,tol,ityp)
C ****************************************
C   haloed line effect with "devide plane" clusters (but without manual
C   clusters).
C ****************************************
       dimension dx(2,nlines),dy(2,nlines),dz(2,nlines)
      dimension nclust(nlndr)
C  ==========================================================
C   transform from 3d "world" to 3d "view plane" coord.system.
C   and then sort the end points of lines.
C  ==========================================================
       call coordt(dx,dy,dz,nlines,n1,n2,nclust,nlndr,ityp)
C  ==============================
C      use haloed line effect :
C  ==============================
       call haloed(dx,dy,dz,nlines,n1,n2,n1,n2,nclust,nlndr,g,tol,ityp)
       return
       end
