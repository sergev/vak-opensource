/*d* === chnloc ===        07.01.85   version    9 */
/* изменение позиции ha позицию #newloc */

#include "../common/adv_common"

int drkfla;

chnloc(newloc) {
    static int drkold;
    drkold=dark();
    loc=newloc;
    if( dark() && drkfla && drkold && pct(30) ) {
	rspeak(23);                   /* свалился b колодец b temhote */
	score();
	exit(0);
    }
    drkfla=1;
    descr();
}
