      subroutine bita(itagb,ixbd,iybd,n1,n2,n5)
C...subroutine bita 01/09/78
      dimension itagb(n1,n2)
      dimension ixbd(1),iybd(1)
      do65 it=1,n1
      do65 ip=1,n2
   65 itagb(it,ip)=0
       do63 ig=1,n5
63    itagb(ixbd(ig),iybd(ig))=10
       m=n1-1
       n=n2-1
       do59 ibx=1,m
      kg=(itagb(ibx,1)+itagb(ibx,2))/20
       imb=10-itagb(ibx,1)
      itagb(ibx,1)=10
      do60 iby=2,n
       if(itagb(ibx,iby).ne.10) goto 61
       if(itagb(ibx,iby+1).ne.0) goto 67
       if(kg.ne.1) goto 6
      kg=0
       do 101 nb=1,n5
       if(iabs(ixbd(nb)-ibx)+iabs(iybd(nb)-iby))101,102,101
101   continue
102    imb=0
       if((ixbd(nb+1)-ibx+1)*(ixbd(nb-1)-ibx-1).eq.0) imb=10
       go to 60
67    kg=1
       go to 60
6      imb=10-imb
       go to 60
61     itagb(ibx,iby)=imb
60    continue
59    itagb(ibx,n2)=10
       do 70 j=1,n2
70    itagb(n1,j)=10
      return
      end
