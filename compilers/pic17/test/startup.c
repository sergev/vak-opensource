#include "pic17c43.h"

#define HZ      10      /* Timer ticks per second. */
#define BAUD    115200  /* Baud rate for RS-232 port. */

void init ();
void main ();
void putchar ();
void print ();

void _startup () @ 0
{
	awake ();
	init ();
	intr_enable ();
	main ();
	print ("\r\n*** Done.\r\n");
	while (! TRMT)
		awake ();
	asm ("halt");
}

void init ()
{
	BSR = 0;

	/* Intialize the serial port. */
	TXSTA = 0;
	SPBRG = /*((MHZ / BAUD) + 32) / 64 - 1*/ 0;
	SPEN = 1;
	TXEN = 1;
}

void print (char far *s)
{
	char c;

	for (;;) {
		c = *s++;
		if (! c)
			return;
		putchar (c);
	}
}

void putchar (char c)
{
again:
	awake ();
	if (! TRMT)
		goto again;
	TXREG = c;
}

void printd (int d)
{
#if 0
	if (d > 99) putchar ((char) ((d / 100) % 10) + '0');
	if (d > 9)  putchar ((char) ((d / 10) % 10) + '0');
	putchar ((char) (d % 10) + '0');
#else
	if (d == 0) putchar ('0');
	else if (d == 1) putchar ('1');
	else if (d == 2) putchar ('2');
	else if (d == 3) putchar ('3');
	else if (d == 4) putchar ('4');
	else if (d == 5) putchar ('5');
	else if (d == 6) putchar ('6');
	else if (d == 7) putchar ('7');
	else if (d == 8) putchar ('8');
	else if (d == 9) putchar ('9');
	else if (d == 10) print ("10");
	else if (d == 11) print ("11");
	else if (d == 12) print ("12");
	else if (d == 13) print ("13");
	else if (d == 14) print ("14");
	else if (d == 15) print ("15");
	else if (d == 16) print ("16");
	else if (d == 17) print ("17");
	else if (d == 18) print ("18");
	else if (d == 19) print ("19");
	else if (d == 20) print ("20");
	else if (d == 21) print ("21");
	else if (d == 22) print ("22");
	else if (d == 23) print ("23");
	else if (d == 24) print ("24");
	else if (d == 25) print ("25");
	else if (d == 26) print ("26");
	else if (d == 27) print ("27");
	else if (d == 28) print ("28");
	else if (d == 29) print ("29");
	else if (d == 30) print ("30");
	else if (d == 31) print ("31");
	else if (d == 32) print ("32");
	else if (d == 33) print ("33");
	else if (d == 34) print ("34");
	else if (d == 35) print ("35");
	else if (d == 36) print ("36");
	else if (d == 37) print ("37");
	else if (d == 38) print ("38");
	else if (d == 39) print ("39");
	else if (d == 40) print ("40");
	else if (d == 41) print ("41");
	else if (d == 42) print ("42");
	else if (d == 43) print ("43");
	else if (d == 44) print ("44");
	else if (d == 45) print ("45");
	else if (d == 46) print ("46");
	else if (d == 47) print ("47");
	else if (d == 48) print ("48");
	else if (d == 49) print ("49");
	else if (d == 50) print ("50");
	else if (d == 51) print ("51");
	else if (d == 52) print ("52");
	else if (d == 53) print ("53");
	else if (d == 54) print ("54");
	else if (d == 55) print ("55");
	else if (d == 56) print ("56");
	else if (d == 57) print ("57");
	else if (d == 58) print ("58");
	else if (d == 59) print ("59");
	else if (d == 60) print ("60");
	else if (d == 61) print ("61");
	else if (d == 62) print ("62");
	else if (d == 63) print ("63");
	else if (d == 64) print ("64");
	else if (d == 65) print ("65");
	else if (d == 66) print ("66");
	else if (d == 67) print ("67");
	else if (d == 68) print ("68");
	else if (d == 69) print ("69");
	else if (d == 70) print ("70");
	else if (d == 71) print ("71");
	else if (d == 72) print ("72");
	else if (d == 73) print ("73");
	else if (d == 74) print ("74");
	else if (d == 75) print ("75");
	else if (d == 76) print ("76");
	else if (d == 77) print ("77");
	else if (d == 78) print ("78");
	else if (d == 79) print ("79");
	else if (d == 80) print ("80");
	else if (d == 81) print ("81");
	else if (d == 82) print ("82");
	else if (d == 83) print ("83");
	else if (d == 84) print ("84");
	else if (d == 85) print ("85");
	else if (d == 86) print ("86");
	else if (d == 87) print ("87");
	else if (d == 88) print ("88");
	else if (d == 89) print ("89");
	else if (d == 90) print ("90");
	else if (d == 91) print ("91");
	else if (d == 92) print ("92");
	else if (d == 93) print ("93");
	else if (d == 94) print ("94");
	else if (d == 95) print ("95");
	else if (d == 96) print ("96");
	else if (d == 97) print ("97");
	else if (d == 98) print ("98");
	else if (d == 99) print ("99");
	else if (d == 100) print ("100");
	else if (d == 101) print ("101");
	else if (d == 102) print ("102");
	else if (d == 103) print ("103");
	else if (d == 104) print ("104");
	else if (d == 105) print ("105");
	else if (d == 106) print ("106");
	else if (d == 107) print ("107");
	else if (d == 108) print ("108");
	else if (d == 109) print ("109");
	else if (d == 110) print ("110");
	else if (d == 111) print ("111");
	else if (d == 112) print ("112");
	else if (d == 113) print ("113");
	else if (d == 114) print ("114");
	else if (d == 115) print ("115");
	else if (d == 116) print ("116");
	else if (d == 117) print ("117");
	else if (d == 118) print ("118");
	else if (d == 119) print ("119");
	else if (d == 120) print ("120");
	else if (d == 121) print ("121");
	else if (d == 122) print ("122");
	else if (d == 123) print ("123");
	else if (d == 124) print ("124");
	else if (d == 125) print ("125");
	else if (d == 126) print ("126");
	else if (d == 127) print ("127");
	else if (d == 128) print ("128");
	else if (d == 129) print ("129");
	else if (d == 130) print ("130");
	else if (d == 131) print ("131");
	else if (d == 132) print ("132");
	else if (d == 133) print ("133");
	else if (d == 134) print ("134");
	else if (d == 135) print ("135");
	else if (d == 136) print ("136");
	else if (d == 137) print ("137");
	else if (d == 138) print ("138");
	else if (d == 139) print ("139");
	else if (d == 140) print ("140");
	else if (d == 141) print ("141");
	else if (d == 142) print ("142");
	else if (d == 143) print ("143");
	else if (d == 144) print ("144");
	else if (d == 145) print ("145");
	else if (d == 146) print ("146");
	else if (d == 147) print ("147");
	else if (d == 148) print ("148");
	else if (d == 149) print ("149");
	else if (d == 150) print ("150");
	else if (d == 151) print ("151");
	else if (d == 152) print ("152");
	else if (d == 153) print ("153");
	else if (d == 154) print ("154");
	else if (d == 155) print ("155");
	else if (d == 156) print ("156");
	else if (d == 157) print ("157");
	else if (d == 158) print ("158");
	else if (d == 159) print ("159");
	else if (d == 160) print ("160");
	else if (d == 161) print ("161");
	else if (d == 162) print ("162");
	else if (d == 163) print ("163");
	else if (d == 164) print ("164");
	else if (d == 165) print ("165");
	else if (d == 166) print ("166");
	else if (d == 167) print ("167");
	else if (d == 168) print ("168");
	else if (d == 169) print ("169");
	else if (d == 170) print ("170");
	else if (d == 171) print ("171");
	else if (d == 172) print ("172");
	else if (d == 173) print ("173");
	else if (d == 174) print ("174");
	else if (d == 175) print ("175");
	else if (d == 176) print ("176");
	else if (d == 177) print ("177");
	else if (d == 178) print ("178");
	else if (d == 179) print ("179");
	else if (d == 180) print ("180");
	else if (d == 181) print ("181");
	else if (d == 182) print ("182");
	else if (d == 183) print ("183");
	else if (d == 184) print ("184");
	else if (d == 185) print ("185");
	else if (d == 186) print ("186");
	else if (d == 187) print ("187");
	else if (d == 188) print ("188");
	else if (d == 189) print ("189");
	else if (d == 190) print ("190");
	else if (d == 191) print ("191");
	else if (d == 192) print ("192");
	else if (d == 193) print ("193");
	else if (d == 194) print ("194");
	else if (d == 195) print ("195");
	else if (d == 196) print ("196");
	else if (d == 197) print ("197");
	else if (d == 198) print ("198");
	else if (d == 199) print ("199");
	else if (d == 200) print ("200");
	else if (d == 201) print ("201");
	else if (d == 202) print ("202");
	else if (d == 203) print ("203");
	else if (d == 204) print ("204");
	else if (d == 205) print ("205");
	else if (d == 206) print ("206");
	else if (d == 207) print ("207");
	else if (d == 208) print ("208");
	else if (d == 209) print ("209");
	else if (d == 210) print ("210");
	else if (d == 211) print ("211");
	else if (d == 212) print ("212");
	else if (d == 213) print ("213");
	else if (d == 214) print ("214");
	else if (d == 215) print ("215");
	else if (d == 216) print ("216");
	else if (d == 217) print ("217");
	else if (d == 218) print ("218");
	else if (d == 219) print ("219");
	else if (d == 220) print ("220");
	else if (d == 221) print ("221");
	else if (d == 222) print ("222");
	else if (d == 223) print ("223");
	else if (d == 224) print ("224");
	else if (d == 225) print ("225");
	else if (d == 226) print ("226");
	else if (d == 227) print ("227");
	else if (d == 228) print ("228");
	else if (d == 229) print ("229");
	else if (d == 230) print ("230");
	else if (d == 231) print ("231");
	else if (d == 232) print ("232");
	else if (d == 233) print ("233");
	else if (d == 234) print ("234");
	else if (d == 235) print ("235");
	else if (d == 236) print ("236");
	else if (d == 237) print ("237");
	else if (d == 238) print ("238");
	else if (d == 239) print ("239");
	else if (d == 240) print ("240");
	else if (d == 241) print ("241");
	else if (d == 242) print ("242");
	else if (d == 243) print ("243");
	else if (d == 244) print ("244");
	else if (d == 245) print ("245");
	else if (d == 246) print ("246");
	else if (d == 247) print ("247");
	else if (d == 248) print ("248");
	else if (d == 249) print ("249");
	else if (d == 250) print ("250");
	else if (d == 251) print ("251");
	else if (d == 252) print ("252");
	else if (d == 253) print ("253");
	else if (d == 254) print ("254");
	else if (d == 255) print ("255");
#endif
}

void prints (short v)
{
	if (v > 9999) putchar ((char) (v / 10000) + '0');
	if (v > 999)  putchar ((char) ((v / 1000) % 10) + '0');
	if (v > 99)   putchar ((char) ((v / 100) % 10) + '0');
	if (v > 9)    putchar ((char) ((v / 10) % 10) + '0');
	putchar ((char) (v % 10) + '0');
}

void printl (long v)
{
	if (v > 999999999) putchar ((char) (v / 1000000000) + '0');
	if (v > 99999999)  putchar ((char) ((v / 100000000) % 10) + '0');
	if (v > 9999999)   putchar ((char) ((v / 10000000) % 10) + '0');
	if (v > 999999)    putchar ((char) ((v / 1000000) % 10) + '0');
	if (v > 99999)     putchar ((char) ((v / 100000) % 10) + '0');
	if (v > 9999)      putchar ((char) ((v / 10000) % 10) + '0');
	if (v > 999)       putchar ((char) ((v / 1000) % 10) + '0');
	if (v > 99)        putchar ((char) ((v / 100) % 10) + '0');
	if (v > 9)         putchar ((char) ((v / 10) % 10) + '0');
	putchar ((char) (v % 10) + '0');
}
