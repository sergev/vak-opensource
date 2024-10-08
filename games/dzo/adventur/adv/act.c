/*d* === act === */
/* evaluate internal code for: (condition) = (action) */
/* look for (condition) satisfied in 'adv:data' file */
/* and then evaluate (action) */
/* format: < (condition) 0 (action) 0 > 0 */
/* */
/* inp: indx   - start address for condition */
/*      object - об'ekt для которого проверять условия */
/* out: true if one of (condition) is satisfied */

#include "../common/adv_common"

int actfla;

act(indx,object) {
#   define not   128
#   define isobj 64
    int ntflag, obflag, condit, obimpl, cmnd, kod, obj, ncarry, i;

begin:
    iniget(indx);

L2000:
   for(;;) {                         /* проверка следующего условия */
	cmnd=get();
   if( cmnd==0 ) break;
	obimpl=object;

	for(;;) {                              /* проверка условий */
	    if( cmnd==0 )  goto L5000;     /*     условая выполнены */

	    kod=((cmnd)%(isobj))              /*  загрузил код условия */;
	    ntflag = (cmnd/not != 0)        /*  загрузил +/- флаг */;
	    obj=obimpl                      /*  загрузил об"ekt */;
	    if( (cmnd%not)/isobj != 0 )  obj=get();

	    switch(kod) {
		case (2-1):  goto L1;
		case (3-1):  goto L2;
		case (4-1):  goto L3;
		case (5-1):  goto L4;
		case (6-1):  goto L5;
		case (7-1):  goto L6;
		case (8-1):  goto L7;
		case (9-1):  goto L8;
		case (10-1):  goto L9;
		case (11-1):  goto L10;
		case (12-1):  goto L11;
		case (13-1):  goto L12;
	    }
	    goto L13;

L1:         obimpl=obj                           /* об"ekt=заданному? */;
	    condit = obj==object;
	    goto L100;

L2:         condit = at(obj)                     /* we are at object */;
	    goto L100;

L3:         condit = at(obj) || place(obj)==caried   /* object is here */;
	    goto L100;

L4:         condit = place(obj)==caried         /* we are toting object */;
	    goto L100;

L5:         condit = pct(obj)                   /* probability = n% */;
	    goto L100;

L6:         condit = (prop(obj)&0377)==get()-1    /* prop(obj)=n */;
	    goto L100;

L7:         condit = place(obj)<0               /* object is fixed */;
	    goto L100;

L8:         condit = 1                     /* 1 */;
	    goto L100;

L9:         condit = yes(obj)                   /* reply is "yes" */;
	    goto L100;

L10:        condit = loc==obj                   /* location = obj */;
	    goto L100;

L11:        ncarry=0                            /* carry > obj objects */;
	    for(i=1; i<=objt; ++i) {
		if( place(i)==caried )  ncarry=ncarry+1;
	    }
	    condit = ncarry>obj;
	    goto L100;

L12:        condit = (rndini>>2) & 037777;   /* init. probability = obj% */
	    condit = (rndini % 100) <obj;
	    goto L100;

L13:        condit=0                      /* one of next words */;
	    for(i=1; i<=kod-13; ++i) {
		if( get()==obj )  condit=1;
	    }
	    goto L100;

L100:

	if( (!condit) == (!ntflag) ) break;
	    cmnd=get();
	}

	do {                        /* условие he выполнено ==> */
	} while( get()!=0 );          /*      сканировать условие */
	do {
	} while( get()!=0 );             /*      сканировать действие */

    }
    return( 0 );                           /* подх. условие he найдено */

L5000:
    for(;;) {
	cmnd=get();
    if( cmnd==0 ) break;
	kod=cmnd%isobj                     /*  загрузил код условия */;
	obj=obimpl                              /*  загрузил обьект */;
	if( (cmnd%not)/isobj != 0 )  obj=get();

	switch( kod) {
	    case (15-14):  goto L101;
	    case (16-14):  goto L102;
	    case (17-14):  goto L103;
	    case (18-14):  goto L104;
	    case (19-14):  goto L105;
	    case (20-14):  goto L106;
	    case (21-14):  goto L107;
	    case (22-14):  goto L108;
	    case (23-14):  goto L109;
	    case (24-14):  goto L110;
	    case (25-14):  goto L111;
	    case (26-14):  goto L112;
	}
	fatal(101);

L101:   place(obj)=loc                           /* drop object here */;
	goto L200;

L102:   place(obj)=0                             /* destroy object */;
	goto L200;

L103:   place(obj)=caried                        /* carry object */;
	goto L200;

L104:   rspeak(obj);                         /* message #n (1<=n<256) */
	goto L200;
L105:   rspeak(obj+255);                     /* message #n ( 256<=n ) */
	goto L200;

L106:   prop(obj)=get()-1                        /* let prop(obj)=n */;
	goto L200;

L107:   chnloc(obj);                         /* change location to #n */
	goto L200;

L108:   specia(obj);                         /* special case #n */
	goto L200;

L109:   place(obj)=get()-1;                     /* throw to location #n */
	goto L200;

L110:   prop(obj)=prop(obj)+1                    /* add 1 to prop(obj) */;
	goto L200;

L111:   indobj();                              /* indicate objects */
	goto L200;

L112:   object = obj;                   /* указание об═екта по умолчанию */
	goto begin;

L200:;
    }
    if( actfla==1 )  goto L2000;              /* оценить след.условие */

    return( 1 );                                 /* ok. all is ready */
}
