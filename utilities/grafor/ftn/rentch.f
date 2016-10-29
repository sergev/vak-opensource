      subroutine rentch
C     rentch   01/02/74  01/09/70  01/09/79
      common/gfgobs/njch,nkch,jg,nris,nach(16)
      if(nkch.eq.0) return
      j=nkch
      nach(j)=8
      nkch=nkch-1
      if(nkch.eq.0) goto 1
      nris=0
      do 3 j=1,nkch
    3 nris=nris+nach(j)/4-2*(nach(j)/8)
      if(nris.ge.nkch) goto 5
      nris=0
      return
    1 jg=0
    5 nris=1
      return
      end
