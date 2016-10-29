/* = common blocks for adventure === include file === */
/* Ко мне этот файл попал с помороченными комментариями. С.Л. */

/* - parameters --- current limits: */

                        /* === 5ЮГЛЕ5х ЛЮууХБНБ === */
#define vocw 550        /* 5ЮГЛЕ5 уКНБЮ5й */
#define loct 254        /* locations */
#define objt 127        /* objects */
#define fixt 200        /* locations for fixed objects */
#define sttt (objt*2)   /* different states of objects */
#define actw 60         /* "action" verbs */
#define ranm 400        /* random messages */
#define plcl 12         /* different player classifications. */

                        /* === юХОх уКНБ === */
#define movewr 0        /* МЮО5ЮБКЕМХЕ ДБХФЕМХй */
#define objcwr 1        /* НА'ekt */
#define actnwr 2        /* ДЕИуюБХЕ c НА'ektom */
#define specwr 3        /* уОЕ6-уКНБН */


/* there are also limits which cannot be exceeded due to the structure */
/* of the database. these upper limits are: */
/*      1000 non-synonymous vocabulary words */
/*       255 locations */
/*       127 objects */
/*       511 random messages */
/*       255 states of objects */


/* --- messages --- */
int  abb;                      /* бКЮЦ: МаФМН КХ ОНКМНЕ НОХуЮМХЕ */
unsigned  _rtext[ranm];        /* ЮД5ЕуЮ 5ЮГМхц уННАкЕМХИ */
#define    rtext(x) _rtext[(x)-1]
unsigned  _ltext[loct];        /* - " -  ДКХММхц НОХуЮМХИ ОНГХ6ХХ */
#define    ltext(x) _ltext[(x)-1]
unsigned  _stext[loct];        /* - " -  ЙН5НюЙХц НОХуЮМХИ ОНГХ6ХХ*/
#define    stext(x) _stext[(x)-1]
unsigned  _ctext[plcl];        /* - " -  ЙБЮКХбХЙЮ6ХИ ХЦ5НЙЮ */
#define    ctext(x) _ctext[(x)-1]
unsigned  _cval[plcl];         /* ОН5НЦХ ЙБЮКХбХЙЮ6ХХ ХЦ5НЙЮ */
#define    cval(x) _cval[(x)-1]
unsigned  _ptext[objt];        /* ХМДЕЙух уННАк. o уБНИую.НА'ektob*/
#define    ptext(x) _ptext[(x)-1]
unsigned  _pstat[sttt];        /* ХМДЕЙух уННАк. o уБНИую.НА'ektob*/
#define    pstat(x) _pstat[(x)-1]
char _abrev[loct];             /* бКЮЦХ: МЮДН уНЙ5ЮкЕММНЕ НОХуЮМХЕ*/
#define    abrev(x) _abrev[(x)-1]


/* --- travels + actions --- */
unsigned  _trvkey[loct];       /* юЮАКХ6Ю МЮ2ЮКЮ Ц5ЮбЮ ДЮММНИ ОНГ. */
#define    trvkey(x) _trvkey[(x)-1]
unsigned  _actkey[actw];       /*  -"-    -"-  5ЕЮЙ6ХХ ha ДЕИуюБХЕ */
#define    actkey(x) _actkey[(x)-1]


/* --- vocabulary --- */
int  nvoc;                     /* 2ХуКН уКНБ b уКНБЮ5Е */
int  _ktab[vocw];              /* 2ХуКЮ - ГМЮ2ЕМХй уКНБ */
#define    ktab(x) _ktab[(x)-1]
long _atab[vocw];              /* уКНБЮ - ОН 4 АаЙБх */
#define    atab(x) _atab[(x)-1]

/* --- state --- */
#define inipro  255           /* МЮ2ЮКм. уНуюНйМХЕ ДКй уНЙ5НБХк */
#define nulobj  255           /* НюуаюуюБагкХИ НА"ekt */
#define caried  1000          /* mapkep - НА'ekt МЕуаю */

int  rndini;
int  loc;                      /* юЕЙакЮй ОНГХ6Хй */
int  tally;                    /* 2ХуКН ЕкЕ МЕ НАМЮ5аФ. уНЙ5НБХк */
int  treasr;                   /* homep 1-НЦН НА'ekta-уНЙ5НБХкЮ */
int  rndini;                   /* уКа2.2ХуКН ДЮММНИ ОЮ5юХХ */
unsigned  tevent;              /* аЙЮГЮюЕКм юЮАКХ6х уКа2.уНАхюХИ */
unsigned  eevent;              /* ЙНМЕ6 юЮАКХ6х уКа2ЮИМхц уНАхюХИ */
unsigned  tiniti;              /* -"-  -"- ХМХ6ХЮКХГЮ6ХХ */
int  _place[loct];             /* ОНГХ6Хй НА7ЕЙюЮ ХКХ -[ХМДЕЙу] */
#define    place(x) _place[(x)-1]
char _fixed[fixt];             /*    ДКй бХЙуХ5НБЮММхц НА'ektob */
#define    fixed(x) _fixed[(x)-1]
char _prop[objt];              /* уБНИуюБН НА'ektЮ */
#define    prop(x) _prop[(x)-1]
