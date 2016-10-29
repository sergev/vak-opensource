      subroutine trgrid(x,y,n0,net,ibound,nodes,net0,nt1)
C...01/09/78
      dimension x(1),y(1),net(1),net0(1),nodes(1),ibound(2,1),l(2)
      k=3*nt1
      do 7 i=1,n0
    7 ibound(1,i)=0
      do 30 nt=1,nt1
      k=3*nt
      do 32 i=1,3
      j=net(k-i+1)
      if(ibound(1,j).eq.0) ibound(1,j)=nt
   32 ibound(2,j)=nt
   30 continue
      nodes(1)=1
      do 9 nom=1,n0
      id=nodes(nom)-1
      im1=ibound(1,nom)
      im2=ibound(2,nom)
      do 10 nt=im1,im2
      k=3*nt
      if(net(k-2).ne.nom) goto 11
      l(1)=net(k-1)
      l(2)=-net(k)
      go to 17
   11 if(net(k-1).ne.nom) goto 13
      l(1)=net(k)
      l(2)=-net(k-2)
      go to 17
   13 if(net(k).ne.nom) goto 10
      l(1)=net(k-2)
      l(2)=-net(k-1)
   17 do 18 j=1,2
      k=l(j)
      k1=nodes(nom)
      if(id.lt.k1) goto 19
      do 21 i=k1,id
      if(iabs(k).ne.iabs(net0(i))) goto 21
      net0(i)=iabs(net0(i))
      go to 18
   21 continue
   19 id=id+1
      net0(id)=k
   18 continue
   10 continue
      nodes(nom+1)=id+1
      k=nodes(nom)
      if((id-k).le.1) goto 25
      k1=id-2
      do 27 ib=k,k1
 27   call pnordr(x,y,net0,nom,ib,id)
 25   continue
    9 continue
      return
      end
