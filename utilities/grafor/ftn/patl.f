      subroutine patl(ph,pk,n,i,p)
C...............b  pat  ha otpe ke (pk-ph) to ky c i-t m homepom
C........ph - ha a  ha  to ka otpe ka
C........pk - kohe ha  to ka otpe ka
C        n -   c o to ek ha otpe ke
C........i - homep to k  , kotopy8 ha o  o y  t
C..............to k  hymepy8tc  ot ha a  ho  k kohe ho
C........p -  o y ehha  to ka
      dimension ph(2),pk(2),p(2)
      x=pk(1)-ph(1)
      y=pk(2)-ph(2)
      if(n.le.1.or.n.lt.i) return
      delx=x/(n-1)
      dely=y/(n-1)
      p(1)=ph(1)+delx*(i-1)
      p(2)=ph(2)+dely*(i-1)
      return
      end
