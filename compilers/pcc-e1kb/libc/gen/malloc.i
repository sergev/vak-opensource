




































































































































union store {
	union store *ptr;

	int calloc;		
};

extern char *sbrk(), *memcpy();

static union store allocs[2];	
static union store *allocp;	
static union store *alloct;	
static union store *allocx;	
static union store *allocend;	


char *
malloc(nbytes)
unsigned nbytes;
{
	register union store *p, *q;
	register int nw;
	register unsigned int temp;
	register unsigned int incr = 0;
	unsigned int sav_temp;

	if(allocs[0].ptr == 0) {		
		allocs[0].ptr = (union store *)((int)(&allocs[1])|1);
		allocs[1].ptr = (union store *)((int)(&allocs[0])|1);
		alloct = &allocs[1];
		allocp = &allocs[0];
		allocend = alloct;
	}
	nw = (nbytes+sizeof(union store)+sizeof(union store)-1)/sizeof(union store);
	;
	;
	for(p=allocp; ; ) {
		for(temp=0; ; ) {
			if(!((int)(p->ptr)&1)) {
				while(!((int)((q=p->ptr)->ptr)&1)) {
					;
					allocp = p;
					p->ptr = q->ptr;
					if (allocend == q) allocend = p;
				}
				if(q >= p+nw && p+nw >= p)
					goto found;
			}
			q = p;
			p = (union store *)((int)(p->ptr)&~1);
			if(p > q)
				;
			else if(q != alloct || p != allocs) {
				;
				return(0);
			} else if(++temp > 1)
				break;
		}
		
		p = allocend;
		q = (union store *)sbrk(0);
		if (q != alloct+1)  {
			



			temp = ((nw+1024/sizeof(union store) - 1)/(1024/sizeof(union store)));
			temp = temp * 1024;
			if (((int)q%sizeof(union store)) != 0)  {
				incr = (sizeof(union store)-(int)q%sizeof(union store));
				q = (union store *)((char *)q + incr);
				temp += incr;
			} 
		}  else  {
			temp = nw - (alloct - allocend);
			temp = ((temp+1024/sizeof(union store))/(1024/sizeof(union store)));
			temp = temp * 1024;
		}
		if(((unsigned int)q)+temp+0 < (unsigned int)q) {
			return(0);
		}
		
		sav_temp = temp;
		if (temp > MAXALLOC)  {
			if ((int)sbrk(MAXALLOC) == -1)  {
				return 0;
			}
			temp -= MAXALLOC;
		}
		if((int)sbrk(temp) == -1) {
			brk(q);   	
			return(0);
		}
		allocend = q;
		;
		alloct->ptr = q;
		

		q->ptr = (union store *)
			   ((unsigned char *)q + sav_temp - incr) - 1;
		if(q != alloct+1)
			alloct->ptr = (union store *)((int)(alloct->ptr)|1);
		alloct = q->ptr;
		alloct->ptr = (union store *)((int)(allocs)|1);
		q = p;
	}
found:
	allocp = p + nw;
	;
	if(q > allocp) {
		allocx = allocp->ptr;
		allocp->ptr = p->ptr;
	}
	p->ptr = (union store *)((int)(allocp)|1);
	
	if (allocend == p)  allocend = allocp;
	return((char*)(p+1));
}



void
free(ap)
register char *ap;
{
	register union store *p = (union store *)ap;

	;
	;
	allocp = --p;
	;
	
	p->ptr = (union store *)((int)(p->ptr)&~1);
	if (p->ptr == alloct)  allocend = p;
	;
	;
}








char *
realloc(p, nbytes)
char	*p;
unsigned nbytes;
{
	register char *q;
	register union store *ap, *aq;
	register unsigned nw;
	unsigned onw;

	ap = (union store *)p;
	if(((int)(ap[-1].ptr)&1))
		free(p);
	onw = ap[-1].ptr - ap;
	q = malloc(nbytes);
	if(q == 0 || q == p)
		return(q);
	nw = (nbytes+sizeof(union store)-1)/sizeof(union store);
	if(nw < onw)
		onw = nw;
	aq = (union store *) memcpy(q, p, onw * sizeof(union store));
	if(aq < ap && aq+nw >= ap)
		(aq+(aq+nw-ap))->ptr = allocx;
	return(q);
}










