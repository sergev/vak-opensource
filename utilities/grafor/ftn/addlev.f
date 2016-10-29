       subroutine addlev(c,ncn,is,cad,nad)
       dimension c(ncn),cad(nad)
       nn=ncn-1
       do 1 ic=1,nn
       do 1 ia=1,is
 1     cad((ic-1)*is+ia)=c(ic)+(c(ic+1)-c(ic))/(is+1)*ia
       return
       end
