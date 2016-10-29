      subroutine storin(al,au)
      common/gfsv/te(15,2),nl,invsln
      common/gfeps/ eps,lengte
C ****************************************************************
C     save this invisible part of line "i" in array "te(15,2)".
C ****************************************************************
C++   print 301,nl,invsln
C##301   format(' s/p *storin *: nl=',i5,' invsln=',i5)
C++   print 300,((te(i,j),j=1,2),i=1,5)
C##300   format(' te(15,2) before:'/(2f10.5))
      if(nl.eq.0) goto 150
C
      do 102 i=1,nl
      if(au.le.(te(i,2)+eps)) goto 110
102   continue
C.....end of loop.
C
C =====================================================
C    "au" is greater than max of "te()"(i.e. te(nl,2)).
C =====================================================
      if(al.gt.(te(nl,2)+eps)) goto 104
      l=nl
      goto 120
C
104         continue
            if(nl.ge.lengte) goto 106
            nl=nl+1
            call svlseg(al,au,nl)
            return
106               continue
C..................concaten. of new element and element "nl".
                  te(nl,2)=au
C##   print 90
C##90                format(' ***overflow of te()***')
                  return
C
110   continue
C ===============================================
C    "au" is less than max of "i"-th element of "te()".
C ===============================================
      if(au.ge.(te(i,1)-eps)) goto 130
      if(i.eq.1) goto 111
      if(al.gt.(te(i-1,2)+eps)) goto 111
      go to 115
C
111         continue
C........... "au" is less than total "i"-th element.
            if(nl.ge.lengte) goto 112
C...........take new element of "te()".
            call shdown(i)
            call svlseg(al,au,i)
            return
112               continue
C.................concaten. of new element and element "i".
                  te(i,1)=al
C##   print 90
                  return
C
115   continue
      l=i-1
120   continue
      do 125 j=1,l
      k=l-j+1
C..... i.e. k=(i-1),...,1
      if(al.ge.(te(k,1)-eps)) goto 121
      if(k.eq.1) goto 122
      if(al.gt.(te(k-1,2)+eps)) goto 122
      call shup(k+1)
125   continue
C.....end of loop (normal exit of loop does not exist).
C
121         continue
            te(k,2)=au
            call testvs
            return
C
122         continue
            call svlseg(al,au,k)
            call testvs
            return
C
130   continue
C....."au" is inside the "i"-th element.
      if(al.ge.(te(i,1)-eps)) return
      if(i.eq.1) goto 131
      goto 132
C
131         continue
C...........leftest element - move to the left.
            te(1,1)=al
            call testvs
            return
C
132   continue
      l=i-1
      do 135 j=1,l
      k=l-j+1
C.....i.e. k=(i-1),...,"
      if(al.gt.(te(k,2)+eps)) goto 133
      if(al.ge.(te(k,1)-eps)) goto 134
      call shup(k+1)
135   continue
C.....end of loop.
C
      te(k,1)=al
      call testvs
      return
C
133         continue
C...........only extend the element.
            te(k+1,1)=al
            return
C
134         continue
C...........concatenation of two elements.
            te(k+1,1)=te(k,1)
            call shup(k+1)
            call testvs
            return
C
C
150   continue
C.....if "nl=0", set the first element of "te()".
      nl=1
      call svlseg(al,au,1)
      call testvs
      return
C
      end
