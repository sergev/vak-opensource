	subroutine isolin(n1,n2,x,y,phi,itagb,na,ixbd,iybd,kc,cont,ikop)
	dimension itagb(n1,n2),phi(n1,n2)
	dimension x(1),y(1),ixbd(1),iybd(1),cont(1)
	dimension xx(50),yy(50)
	common/gfcell/i,j,ib,jb,d1,d2,kent,qq,const
	common/gfisol/nc,c
c
	fact=1.e-9
	kt=50
	n5=iabs(na)
	ncn=iabs(kc)
	if (na.eq.0) return
	if (na.gt.0) goto 44
	ic=0
41      do 42 ik=1,kt
		ic=ic+1
		rd=x(ixbd(ic))
		tt=y(iybd(ic))
		xx(ik)=rd
		yy(ik)=tt
		if(ikop.ne.1) goto 52
			xx(ik)=xpol(rd,tt)
			yy(ik)=ypol(rd,tt)
52              if(n5-ic-1)47,42,43
43              if(iabs(ixbd(ic)-ixbd(ic+1))+
     *                  iabs(iybd(ic)-iybd(ic+1)).ne.1)
     *                  goto 46
42              continue
	call lineo(xx,yy,kt)
	ic=ic-1
	go to 41
46      call lineo(xx,yy,ik)
	go to 41
47      call lineo(xx,yy,ik)
44      do 6 nc=1,ncn
		if (kc.eq.0) return
		if (kc.lt.0) goto 24
			do 21 in=1,n1
			do 21 jn=1,n2
				if(itagb(in,jn)-10) 22, 21, 23
22                              itagb(in,jn)=0
				go to 21
23                              itagb(in,jn)=10
21                              continue
24                      continue
		const=cont(nc)
		c=const
		qq=1
		do 2 k=2,n5
			i=ixbd(k)
			j=iybd(k)
			ib=ixbd(k-1)
			jb=iybd(k-1)
			d1=phi(i,j)-const
			d2=phi(ib,jb)-const
			if(iabs(i-ib)+iabs(j-jb).ne.1)goto 2
33                      if(d1*d2)32,31,2
31                      d1=d1+fact
			d2=d2+fact
			go to 33
32                      kent=1
			if(ib-i)37,34,39
34                      if(jb-j)38,2,11
37                      kent=2
			go to 11
38                      kent=3
			go to 11
39                      kent=4
11                      call look(n1,n2,x,y,phi,itagb,xx,yy,kt,ikop)
2                       continue
12              qq=2
		na1=n1-1
		na2=n2-1
		do 3 k=2,na1
		do 3 l=2,na2
			i=k
			ib=k
			j=l
			jb=l+1
			if(itagb(i,j).lt.10) goto 15
			if(itagb(ib,jb).ge.10) goto 3
15                      kent=1
			d1=phi(i,j)-const
			d2=phi(ib,jb)-const
18                      if(d1*d2)16,17,3
17                      d1=d1+fact
			d2=d2+fact
			go to 18
16                      call look(n1,n2,x,y,phi,itagb,xx,yy,kt,ikop)
3                       continue
6               continue
	return
	end
