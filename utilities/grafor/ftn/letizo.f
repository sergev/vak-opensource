      subroutine letizo(xi,yi,ilm,ziz,niz,kind)
      dimension xi(ilm),yi(ilm)
      common /gfbet/ kt,istep,size
      common /gfisol/ nc,c
      nc=niz
      c=ziz
      if(kind.eq.0) go to 1
      xi(ilm)=xi(1)
      yi(ilm)=yi(1)
  1   continue
      if (kt.ne.0) call transf(ziz,xi,yi,ilm)
      if(istep.gt.0) call markbe(xi,yi,ilm)
      call linnum(xi,yi,ilm)
      return
      end
