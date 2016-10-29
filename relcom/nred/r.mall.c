/*
 *      Редактор RED. ИАЭ им. И.В. Курчатова, ОС ДЕМОС
 *
 * r.mall.c - Запрос/освобождение памяти.
 *            Программа взята из стандартной библиотеки,
 *            Изменен только режим отладки
 *      $Header: /home/sergev/Project/vak-opensource/trunk/relcom/nred/RCS/r.mall.c,v 3.1 1986/04/20 23:43:55 alex Exp $
 *      $Log: r.mall.c,v $
 *      Revision 3.1  1986/04/20 23:43:55  alex
 *      *** empty log message ***
 *
 */

#include "r.defs.h"
#ifndef WORK
#define debug
#endif

#ifdef debug
#define ASSERT(p) if(!(p))botch("p");else
botch(s)
char *s;
{
        printf("assertion botched: %s\n",s);
        fatal("Mallock DESTROIED\n");
}
#else
#define ASSERT(p)
#endif

/*      avoid break bug */
#ifdef pdp11
#define GRANULE 64
#else
#define GRANULE 0
#endif
/*      Монитор памяти
 *      Круговая стратегия размещения
 *      Работает с кусками свободной памяти, связанными в монотонно
 *      возрастающий список.
  *      Перед каждым блоком записан указатель на следующий блок.
  *      Блоки выравнены на границу слова (тип ALIGN)
  *      Младший бит указателя используется как признак "занято"
  *      (1 - занято, 0 - свободно).
  *      Дырки отмечаются как занятые блоки.
  *      Последний блок (на который указывает alloct) пуст и указывает
  *      на первый распределенный блок
  *
  *      ALIGN, NALIGN, BLOCK, BUSY, INT
  *      INT - целый тип, соответствыющий слову - указателю.
 */
#define INT int
#define ALIGN int
#define NALIGN 1
#define WORD sizeof(union store)
#ifdef pdp11
#define BLOCK 256      /* a multiple of WORD*/
#else
#define BLOCK 1024
#endif
#define BUSY 1
#undef NULL
#define NULL 0
#define testbusy(p) ((INT)(p)&BUSY)
#define setbusy(p) (union store *)((INT)(p)|BUSY)
#define clearbusy(p) (union store *)((INT)(p)&~BUSY)

union store { union store *ptr;
              ALIGN dummy[NALIGN];
              int calloc;       /*calloc clears an array of integers*/
};

static  union store allocs[2];  /*initial arena*/
static  union store *allocp;    /*search ptr*/
static  union store *alloct;    /*arena top*/
static  union store *allocx;    /*for benefit of realloc*/
char    *sbrk();

char *
malloc(nbytes)
unsigned nbytes;
{
        register union store *p, *q;
        register nw;
        static temp;    /*coroutines assume no auto*/

        if(allocs[0].ptr==0) {  /*first time*/
                allocs[0].ptr = setbusy(&allocs[1]);
                allocs[1].ptr = setbusy(&allocs[0]);
                alloct = &allocs[1];
                allocp = &allocs[0];
        }
        nw = (nbytes+WORD+WORD-1)/WORD;
        ASSERT(allocp>=allocs && allocp<=alloct);
        ASSERT(allock());
        for(p=allocp; ; ) {
                for(temp=0; ; ) {
                        if(!testbusy(p->ptr)) {
                                while(!testbusy((q=p->ptr)->ptr)) {
                                        ASSERT(q>p&&q<alloct);
                                        p->ptr = q->ptr;
                                }
                                if(q>=p+nw && p+nw>=p)
                                        goto found;
                        }
                        q = p;
                        p = clearbusy(p->ptr);
                        if(p>q)
                                ASSERT(p<=alloct);
                        else if(q!=alloct || p!=allocs) {
                                ASSERT(q==alloct&&p==allocs);
                                return(NULL);
                        } else if(++temp>1)
                                break;
                }
                temp = ((nw+BLOCK/WORD)/(BLOCK/WORD))*(BLOCK/WORD);
                q = (union store *)sbrk(0);
                if(q+temp+GRANULE < q) {
                        return(NULL);
                }
                q = (union store *)sbrk(temp*WORD);
                if((INT)q == -1) {
                        return(NULL);
                }
                ASSERT(q>alloct);
                alloct->ptr = q;
                if(q!=alloct+1)
                        alloct->ptr = setbusy(alloct->ptr);
                alloct = q->ptr = q+temp-1;
                alloct->ptr = setbusy(allocs);
        }
found:
        allocp = p + nw;
        ASSERT(allocp<=alloct);
        if(q>allocp) {
                allocx = allocp->ptr;
                allocp->ptr = p->ptr;
        }
        p->ptr = setbusy(allocp);
        return((char *)(p+1));
}

/*      freeing strategy tuned for LIFO allocation
*/
free(ap)
register char *ap;
{
        register union store *p = (union store *)ap;

        ASSERT(p>clearbusy(allocs[1].ptr)&&p<=alloct);
        ASSERT(allock());
        allocp = --p;
        ASSERT(testbusy(p->ptr));
        p->ptr = clearbusy(p->ptr);
        ASSERT(p->ptr > allocp && p->ptr <= alloct);
}

 /*      realloc(p, nbytes) reallocates a block obtained from malloc()
  *      and freed since last call of malloc()
  *      to have new size nbytes, and old content
  *      returns new location, or 0 on failure
 */

#if 0
 /* В red не встречается */
char *
realloc(p, nbytes)
register union store *p;
unsigned nbytes;
{
        register union store *q;
        union store *s, *t;
        register unsigned nw;
        unsigned onw;
        if(testbusy(p[-1].ptr))
                free((char *)p);
        onw = p[-1].ptr - p;
        q = (union store *)malloc(nbytes);
        if(q==NULL || q==p)
                return((char *)q);
        s = p;
        t = q;
        nw = (nbytes+WORD-1)/WORD;
        if(nw<onw)
                onw = nw;
        while(onw--!=0)
                *t++ = *s++;
        if(q<p && q+nw>=p)
                (q+(q+nw-p))->ptr = allocx;
        return((char *)q);
}
#endif

#ifdef debug
allock()
{
    register union store *p;
    int x;
    x = 0;
    for(p= &allocs[0]; clearbusy(p->ptr) > p; p=clearbusy(p->ptr)) {
        if(p==allocp)
            x++;
    }
    ASSERT(p==alloct);
    return(x==1|p==allocp);
}
#endif

#ifdef debug
static ptflag=0;
ptfree()
{ 
    register union store *p,*q;
    register int i=0;
    if (ptflag++) return;
    printf("\n FREE LIST:\n");
    ASSERT(allocp>allocs && allocp <= alloct);
    for (p=allocs; ;)   {
        if ( !testbusy(p->ptr) )
        {
            printf(" 0%o( %d)\t%c", p,
                ((int)p->ptr - (int)p) - (WORD), (++i%4?' ':012));
        }
        q = p;
        p = clearbusy(p->ptr);
        if ( p > q ) ASSERT( p <= alloct );
        else if ( q != alloct || p != allocs ) { 
            printf("Corrupt arena\n");
            return;
        }
        else {printf("\n"); return;}
    }
}
#endif
