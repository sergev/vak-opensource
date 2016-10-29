      subroutine ilimtn(l,ir,ibound,nb)
C...01/09/78
      dimension ibound(2,1)
      do 2 i=1,nb
      if(iabs(ibound(1,i)).ne.l) goto 2
      if(ibound(2,i)-ir) 2,4,2
    2 continue
      return
    4 ibound(1,i)=ibound(1,nb)
      ibound(2,i)=ibound(2,nb)
      nb=nb-1
      return
      end
