      subroutine quadra(itagb,ixbd,iybd,n1,n2,n5)
      dimension itagb(n1,n2),ixbd(n5),iybd(n5)
      do 1 n=1,n1
      ixbd(n)=n
      iybd(n)=1
      kn=n1+n2+n-2
      ixbd(kn)=n1-n+1
  1   iybd(kn)=n2
      do 2 l=1,n2
      kl=n1+l-1
      ixbd(kl)=n1
      iybd(kl)=l
      ll=2*n1+n2+l-3
      ixbd(ll)=1
   2  iybd(ll)=n2-l+1
      do 65 it=1,n1
      do 65 ip=1,n2
  65  itagb(it,ip)=0
      do 60 ig=1,n5
  60  itagb(ixbd(ig),iybd(ig))=10
      return
      end
