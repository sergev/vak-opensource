/*d* === treat events ===       19.01.85   version   10 */

#include "..\common\advexter.h"
#include <stdio.h>

extern int actfla;
extern FILE *F1;

events() {
    int actres;
    actfla=1;
    actres=act(tevent,0);
    actfla=0;
}


ini() {
    int actres, tim[2];
    unsigned rand();

    loadcm();

    if( (F1=fopen("frozen.adv","rb")) != NULL ) {
        loadfr();

    } else {
        time( tim );  srand( tim[1] );
        loc=1;
        rndini=rand();
        actfla=1;
        actres=act(tiniti,0);
        actfla=0;
    }
    descr();

}
