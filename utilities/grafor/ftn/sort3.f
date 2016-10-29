       subroutine sort3(pa,pb,k)
       dimension pa(2,4),pb(2,4)
       nn=1
       do 4 ijk=1,4
       n=1
       do 3 j=1,3
       if(abs(pa(k,n)-pa(k,j+1)).gt.1.e-5) go to 1
       kk=1
       if(k.eq.1) kk=2
       if(pa(kk,n).gt.pa(kk,j+1)) go to 3
       go to 2
  1    if(pa(k,n).gt.pa(k,j+1)) go to 3
  2    n=j+1
  3    continue
       pb(1,nn)=pa(1,n)
       pb(2,nn)=pa(2,n)
       nn=nn+1
       pa(1,n)=-99999.
       pa(2,n)=-99999.
  4    continue
       return
       end
