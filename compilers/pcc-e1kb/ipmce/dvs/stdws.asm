 MOהץלר 'CEPBיC_הBC'
***************************************************
*                                                 *
*     CEPBיCHשך נAKET נPOדEהץP היCנET‏EPA         *
*           BיPTץAלרHשX CיCTEM(הBC)               *
*               יTM י BT AH CCCP                  *
*                    1989 ח.                      *
*                                                 *
*                                                 *
*BשגOP נPOדEהץPש OנPEהEלEH COהEPציMשM ס‏ 'Bיה_PAג'*
*                                                 *
*הלס לאגOך נPOדEהץPש HEOגXOהיMO תAהAHיE Oג'סתA -  *
*TEלרHשX נAPAMETPOB: 'HOM_KAH','HOM_ץCT','יMס_Pת' *
*                                                 *
*Bיה_PAג=0 - PAתMETKA היCKA י COתהAHיE HA HEM MET-*
*            Kי TOMA.                             *
*  'דדחחNDה' TEKץ‎יE KOOPהיHATש PAתMETKי          *
*נAPAMETPש : 'PAתMEP_Pת','‏יCלO_Pת','יMס_Pת'.     *
*                                                 *
*Bיה_PAג=1 - CTAPT היCנE‏EPA BיPTץAלרHשX CיCTEM   *
*נAPAMETPש : 'הלי_תAנ','HOM_גלO','AהP_Oתץ', 'KOל_ *
*             תAנ'                                *
*                                                 *
*Bיה_PAג=2 - נEPEניCר C OנEPATיBHOך נAMסTי HA היCK*
*נAPAMETPש : 'הלי_תAנ','HOM_גלO','AהP_Oתץ', 'זלA_ *
*            ‏TE','KOל_תAנ'.                      *
*                                                 *
*Bיה_PAג=3 - היCKOBשך TECT                        *
*נAPAMETPש : תAנOלHיTר MACCיB 'O‏EPEהר',CM.TAגל.  *
*                                                 *
*Bיה_PAג=4 - נEPEPAתMETKA הOPOצKי,נPי üTOM הEלAET-*
*            Cס נOנשTKA COXPAHEHיס תAניCEך HA üTOך*
*            הOPOצKE.                             *
*נAPAMETPש : 'HOM_גלO','זלA_‏TE'                  *
*                                                 *
*Bיה_PAג=5 - COתהAHיE PAתהEלA(Pת) TOMA            *
*נAPAMETPש : 'PAתMEP_Pת','יMס_Pת'.                *
*                                                 *
*      KOT : 1 HET MECTA הלס OניCATEלס Pת         *
*            2 HET נPOCTPAHCTBA הלס  Pת           *
*            3 TAKOE יMס Pת ץצE ECTר              *
*                                                 *
*Bיה_PAג=6 - יCKלא‏EHיE ית היPEKTOPיס Pת          *
*נAPAMETPש : 'יMס_Pת'.                            *
*                                                 *
*      KOT : 1 HET TAKOחO Pת                      *
*                                                 *
*Bיה_PAג=7 - נEPEניCר C MAחHיTHOך לEHTש HA היCK.  *
*נAPAMETPש : 'HOM_זAך_Mל','HOM_KAH_Mל','HOM_ץCT_Mל*
*            'HOM_גלO','זלA_‏TE'.                 *
*                                                 *
*Bיה_PAג=8 - תAחPץתKA נPOחPAMMש ית Mה היCנAKA     *
*נAPAMETPש : 'HOM_גלO' HOMEP תOHש היCנAKA         *
*                                                 *
*                                                 *
***************************************************
 
 
KH CEKדיס 0,נ20,זיKT,CBOג
KH:גלOK;לOKAלרHשE ס‏EךKי KAHAלA
!נAPAMETPש הPAךBEPA
L_Cל:üKB 3;KOל Cלץצ CלOB
L_גלO:üKB 128+L_Cל;הליHA גלOKA
L_גגT:üKB /(9*L_גלO+8+7)//8;הליHA גלOKA גEת TEחA
L_KלA:üKB 32;KלACTEP OגMEHA
L_KנP:üKB L_KלA*3+L_KלA/L_תAנMIN+2;MAX הליHA KAH נPO
L_תAנMIN:üKB 10;MיHיMAלרHOE ‏יCלO תAנ HA הOPOצKE
L_תAנMAX:üKB 20;MAKCיMAלרHOE ‏יCלO תAנ HA הOPOצKE
!
ץCTP:נAM
KKOM:נAM
NתAנ:נAM
KתAנ:נAM
גץזK:נAM L_גלO*L_KלA+L_גלO/8+1
דדחחN:נAM L_KלA+L_KלA/L_תAנMIN+2
KAH_נPO:נAM L_KנP*2
L_KH:üKB #-KH
 KגלOK KH
 
 
CEPBיC CEKדיס 0,נ20,לKOM
היCKי גלOK BKל=KH
 נAM
BXOה ץיA CTEK(S)
 נג HA‏
: נג ;BHE_נPE
: CTOנ 3;Pצ=3 üKCTPAKOה
: CTOנ 4;Pצ=1
: CTOנ 5;Pצ=1
: CTOנ 6;Pצ=3 BHץ_נPE
 BPח 1FH;PנP
: CTOנ 7;Pצ=3 תAנP_KOM
 נג BXOה
Bיה_PAג KOHה 0
PAתMEP_Pת KOHה 808*19*10;153 520 /200/
‏יCלO_Pת KOHה 256
יMס_Pת TEKCT T''0'CיC_נAK_הץג'
 נAM 8
HOM_זAך_Mל KOHה 3;יCנOלרתץTCס נPי ‏TEHיי C Mל
HOM_KAH_Mל KOHה 3
HOM_ץCT_Mל KOHה 2
 
KOל_תAנ KOHה 100H
AהP_Oתץ KOHה 1 0000H
HOM_גלO KOHה 100H
זלA_‏TE KOHה 1
הלי_תAנ נAM;KOל תAנ C‏/תנ תA OהיH OגMEH <32
HOM_ץCT KOHה 1
HOM_KAH KOHה 0
CET_HOM KOHה 038000000 00000000H;AהPECA AגOHEHTOB
 KOHה 058000000 00000008H
 KOHה 0D8000000 00000010H
 KOHה 080000000 00000018H
O‏EPEהר KOHה 0 2 3 07 0 1 0 100 01 E00H
 KOHה 0 2 3 0F 0 1 0 2BA 0C E00H
 KOHה 0 0 0 04 0 1 1 100 01 000H
 KOHה 0 1 0 0F 1 1 1 100 01 000H
***************************************************
*     [64:61] -- HOM_KAH
*     [60:57] -- HOM_ץCT
*     [56:53] -- HOM_נAK
*     [51:45] -- הלי_תAנ
*     [44:41] -- 0-‏T,1-תנ
*     [40:37] -- זלA_‏TE
*     [36:33] -- 1-RANDOM
*     [32:21] -- דדדד
*     [20:13] -- חח
*     [12:01] -- הליHA
***************************************************
יC1 üKB 9
יC2 üKB 10
יC3 üKB 11
יC7 üKB 15
R1 üKB 1
R2 üKB 2
R3 üKB 3
R4 üKB 4
R5 üKB 5
R6 üKB 6
R7 üKB 7
R8 üKB 8
R9 üKB 9
RA üKB 10
RB üKB 11
RC üKB 12
R üKB 13
W üKB 14
S üKB 15
גץז_CTA üKB 3000H
גץזEP üKB 4000H
TAג_הEז üKB METKA_TOMA+6
MAC_‏TE üKB TAג_הEז+256;
MAC_TEC üKB MAC_‏TE+L_גלO*L_KלA
CTEK üKB MAC_TEC+2*L_גגT*L_תAנMAX
CלCל üKB CTEK+100
KנOB üKB CלCל+4
נOל_ץנP üKB KנOB+4;יC3,יC7,יP1-יP13,יP14,יP15
דחN üKB נOל_ץנP
ץCK נAM
דדחחNDה נAM 1; TEKץ‎יך דדחח נPי PAתMETKE
CCK נAM
גץC נAM 3
ACK נAM
AגגץC נAM
PAג נAM
MACKA KOHה [64:61]0CH
MAC_KAH KOHה [64:61]9H
HA‏_גלOK נAM
KOH_גלOK נAM
HOM_חOל נAM
HOM_תAנ נAM
KOל_הOP נAM
‏יC_הOP_תAM נAM
X נAM
זדדחח נAM
 KOHה [49]00 80H;HET הEזEKTOB HA הOPOצKE
ץדדחח נAM
  
! נAPAMETPש ץCTPOךCTBA
   
‏_חOל KOHה 19;‏יCלO חOלOBOK
‏_דיל KOHה 808;‏יCלO יHזOPM דיליHהPOB
‏_דילנ KOHה 814;נOלHOE ‏יCלO דיל HA נAKETE
O_הOP KOHה 13440;Oג'EM הOPOצKי B גAךTAX
!   
‏_תAנ נAM
‏_תAנO נAM;OגPATHשE BEלי‏יHש
‏_חOלO נAM
‏_דילO נAM
‏_חOלד נAM
‏_תAנד נAM
   
PAתM_KOה
 KOHה 05555 5555 5555 5555H
 KOHה 0D8AA AAAA AAAA AAAAH
 KOHה 0AAD8 5555 5555 5555H
 KOHה 05555 D8AA AAAA AAAAH
 KOHה 0AAAA AAD8 5555 5555H
 KOHה 05555 5555 D8AA AAAAH
 KOHה 0AAAA AAAA AAD8 5555H
 KOHה 05555 5555 5555 D8AAH
 KOHה 0AAAA AAAA AAAA AAD8H
X0 KOHה 8300 0000 0FFF FFFEH;הלס XAP=0
 KOHה [25]00 08 04 0000H!HET_Oûג
 KOHה [25]00 FF FF 1F90H
 KOHה [25]00 4A 04 0010H!HAהO_נOB
 KOHה [25]00 FF FF 1F90H
 KOHה [25]00 4A 04 0050H!KOPT
 KOHה [25]00 FF FF 13F0H
X1 KOHה 8400 0000 0FFF FFFEH;הלס XAP=1
 KOHה [33]000C 0000H!HET_Oûג;KB
 KOHה [33]00FF 1F90H
 KOHה [33]004E 0010H!HAהO_נOB;MKC
 KOHה [33]00FF 1F90H
 KOHה [33]0008 0000H!HET_Oûג;KK OKK=1
 KOHה [33]00FF 1F90H
K‏Tג KOHה 07 64 0000 000000H!ץדדחח;‏TEHיE C היCנAKA
 KOHה 31 54 0000 000000H!דחN
 KOHה 08 04 0000 000000H!#-1
 KOHה 06 04 0004 000000H!CלCל
 KOHה 31 54 0000 000000H!דחN+1
 KOHה 08 04 0000 000000H!#-1
 KOHה 06 80 0100 000000H
KנPת KOHה 1F 14 0000 000000H!MACKA
 KOHה 07 64 0000 000000H!ץדדחח
 KOHה 39 44 0000 000000H!דדחחNDה;נOיCK CA
 KOHה 08 04 0000 000000H!#-1
 KOHה 19 34 0001 000000H!זדדחח;CA
 KOHה 15 04 0001 000000H!דדחחNDה;R0
KנPת$ נAM 2*L_תAנMAX+1
Kנהת KOHה 1F 14 0000 000000H!MACKA;תAנ נPית הEז
 KOHה 07 64 0000 000000H!ץדדחח
 KOHה 39 44 0000 000000H!דדחחNDה;נOיCK CA
 KOHה 08 04 0000 000000H!#-1
 KOHה 19 34 0001 000000H!זדדחח;CA
 KOHה 15 00 0001 000000H!דדחחNDה;R0
KץCT
 KOHה 00 A4 04 0003 000000H!גץז_CTA;CHסTיE CTATיCTיKי
 KOHה 00 13 04 0000 000001H
KגץC KOHה 00 04 00 0003 000000H!גץC
KנKP KOHה 00 31 54 0000 000000H!גץC+1
 KOHה 00 08 04 0000 000000H!KנKP
 KOHה 00 08 00 0000 000000H
KנKPT נAM
 KOHה 00 08 04 0000 000000H!KנKPT
 KOHה 00 08 00 0000 000000H
 גAתA KH:RB
HA‏ BשT
 CTOנ
 ץיA -12(W)
 C‏
: ץי 13(W);OגHץלEHיE PEחיCTPOB
 דיKל #(W)
 ץיA METKA_TOMA(R)
: חPץל 0FFFFH
 תנH+ 1(R)
 נB ץCT(R)
 נז Bיה_PAג
 נג #+1
: ץיA (W)
 נג PAתMETKA;PAג=0
: ץיA (R9);PAג=1
 נג CTAPT_הBC
: ץיA (R9);PAג=2 נEPE Oתץ->Mה
 נג נEPE_Oתץ_Mה
: ץיA -3(R1);PAג=3
 נג TECT_Mה
: ץיA PEMOHT(RA);PAג=4
 נג ‏TEHיE_METKי
: ץיA COת_Pת(RA);PAג=5
 נג ‏TEHיE_METKי
: ץיA יCK_Pת(RA);PAג=6
 נג ‏TEHיE_METKי
: C‏ HOM_KAH;PAג=7 נEPE Mל->Mה
 נג ‏TEHיE_Mל
: C‏ HOM_גלO;PAג=8 נEPE היCנAK->Oתץ
 Cהל 2;*4
 ץי R9
 ץיA 4 0000H(R1);HA‏ AהPEC 1-CEחMEHTA
היC C‏ =MAC_‏TE
 C‏לM (R9)
 ץיA K‏Tג(R3)
 נB OגM_גלOKOM(R)
 נHP OûיגKA_OגMEHA
 נז (S);AהPEC Oתץ
 ץיA (W)
 ץיA -255(R)
: תנ (S);KOHT CץM
 C‏T+ 1(W)
 CלK (S)
 דיKל #-1(R)
 HTצ CלCל+3
 נHP KCM_HE_COBנ
 C‏ CלCל+1
 Cהנ 12
 HTצ HOM_תAנ
 י =.[19:1]
 נHP Oûג_CלCל
 ץיA MAC_‏TE(R2)
 ץיA 4 0000H(R4)
 ץיA -127(W)
ץנAK C‏ (R2)
 HTצ =.[64:17]
 נPB תAחP
 Bה
 י =.[64:33]
 C‏M 1(R2)
 Cהנ 32
 ילי (S)
 C‏M+ 2(R2)
 Cהנ 12
 ץיM R
 ץPצ 400H(R)
 תנT+ 1(R1)
 דיKל ץנAK(W)
 CליA 1(R9)
 נג היC
תAחP ץיA (W)
 CליA -4 0000H(R1)
 נזA (R1)
 חPץנ 8
 C‏T+ 1(R4)
 תנT+ 1(W)
 CTOנ
 נג 1
OגM_גלOKOM תנ HOM_תAנ
 ץMH =8000 2B1D A461 02B2H
 BהM
 ץMH ‏_חOלד
 ץי W
 Bה
 ץMH =8000 0000 0000 0005H
 Cהל 25
 ילי =.25
 תנM דחN
 Cהל 16
 יליל (W)
 Cהל 16
 תנ ץדדחח
 Cהל 16
 ילי דחN
 תנ דחN
 CלK =.25
 תנ דחN+1
 C‏ 6(R3);AהPEC Oתץ
 י =.[64:25]
 ילי -1(S)
 תנ 6(R3)
 נג תAסBKA
‏TEHיE_Mל C‏M HOM_KAH_Mל
 נB ץ0(W)
 C‏ =[49]07H
 נB תAסBKA_Mל(R);נEPEMOTKA
 נז HOM_זAך_Mל
 ץיA (R7)
: ץיA #(R);נOיCK זAךלA
 נHP OûיגKA_OגMEHA
 CליA -1(R7)
 ניPB HAûליז(R7)
 C‏ =[49]3FH;זAךל BנEPEה
תAסBKA_Mל
 C‏M HOM_ץCT_Mל
 Cהל 56
 נז HOM_KAH_Mל
 CלK HOM_KAH+1
 ץי R5
 י =.[64:57]
 ילי (S)
 תנ CלCל
 ץיA CלCל(R3)
 ץיA (R8)
 נג נץCK_OגMEHA
HAûליז C‏ =02 80 0004 000000H!MAC_‏TE
 נB תAסBKA_Mל(R);‏TEHיE METKי זAךלA
 נHP OûיגKA_OגMEHA
 C‏ MAC_‏TE+1
 תנ KOל_תAנ
 B‏Oג =1
 ץי R9
 ץי RA
 C‏ MAC_‏TE
 תנ AהP_Oתץ
 ץי R7
נEPE_MAC C‏ =02 80 0200 000000H
 יליל (R7)
 נB תAסBKA_Mל(R);‏TEHיE MACCיBA
 ץיA -511(W)
 נHP OûיגKA_OגMEHA
: תנ (S)
 C‏T+ 1(R7)
 Cהנ 32
 CלK (S)
 BהM
 Cהנ 32
 CלK (S)
 דיKל #-3(W)
 נז KOל_תAנ
 תנ MAC_‏TE-1(R9);תAניCר KCM
 דיKל נEPE_MAC(R9)
 C‏ KOל_תAנ
 Cהל 24
 ילי =02 80 0000 000000H!MAC_TEC
 נB תAסBKA_Mל(R);‏TEHיE תOHש C KCM
 נHP OûיגKA_OגMEHA
 ץיA MAC_‏TE(R7);CPABHEHיE KCM
: C‏+ 1(R7)
 HTצ MAC_TEC-1-MAC_‏TE(R7)
: נPB #+1
 CTOנ 8;KCM HE COBנAלA
 דיKל #-2(RA)
 C‏ (S)
 תנ HOM_KAH
 C‏ KOל_תAנ
 Cהל 2;*4
 ץMH ‏_תAנO
 CלK =1
 תנ KOל_תAנ
נEPE_Oתץ_Mה
 C‏ הלי_תAנ
 ץMHד (R9)
 Cהל 7
 CלK AהP_Oתץ
 C‏M הלי_תAנ
 ץMHד (R9)
 CלK HOM_גלO
 נB תAניCר(R7)
 נHP OûיגKA_OגMEHA
 CליA 1(R9)
 C‏ KOל_תAנ
 B‏ד (R9)
 נגל נEPE_Oתץ_Mה
 נג BXOה
CTAPT_הBC
 C‏ הלי_תAנ
 ץMHד (R9)
 Cהל 7
 CלK AהP_Oתץ
 C‏M הלי_תAנ
 ץMHד (R9)
 CלK HOM_גלO
 נB ‏TEHיE(R7)
 נHP OûיגKA_OגMEHA
 CליA 1(R9)
 C‏ KOל_תAנ
 B‏ד (R9)
 נגל CTAPT_הBC
 נז AהP_Oתץ
 ץיA (R8)
 C‏ KOל_תAנ
 ץMH הלי_תAנ
 Cהל 7
 נזC
 חPץנ -1
 C‏T+ 1(R8)
 תנT+ 1(R9)
 נג 1
PAתMETKA Bי W
 ץMH =L_גגT
 CלK =1D 34 0094 000000H!MAC_TEC
 תנ KנPת$(W)
 נז ‏_תAנ
 C‏ל (W)
 ץMH =L_גגT
 CלK =1E 34 0094 000000H!MAC_TEC
 CליA 1(W)
 נז ‏_תAנ
 תנ KנPת$(W)
 C‏ ‏_תAנ
 B‏ד (W)
 נגל PAתMETKA
 נז ‏_תAנ
 C‏ KנPת$(W)
 HTצ =.43
 נז ‏_תAנ
 תנ KנPת$(W)
 C‏ =1A 54 0000 000000H!גץC;C‏יTATר CA
 תנ KנPת$(W)
 C‏ ‏_דילנ
 ץMH ‏_חOל
 C‏M ‏_תAנ
 B‏ =1
 CלK PAתMEP_Pת;הEלEHיE HAדEלO
 ץMH ‏_תAנO
 תנ KOל_הOP
 B‏Oג (S);814*19=15466 KOל הOPOצEK /200/
 תנ ‏יC_הOP_תAM
 ץיA (RC);C‏ET‏יK הOPOצEK
PAת0 Bי RC;TEKץ‎Aס הOPOצKA
 ץMH ‏_חOלO;1/19
 BהM
 ץMH ‏_חOלד
 ץיM W
 Cהל 16
 יליל (W)
 תנ PAג
 Cהל 16
 תנ ץדדחח
 CהלM 8
 ילי =8000 80H;HET הEזEKTOB HA הOPOצKE
 תנM זדדחח
 Cהל 16
 תנ דדחחNDה
 ץיA (W)
 ץיA MAC_TEC(R)
PAת1 C‏ל 1(W)
 Cהל 24
 ילי =L_גלO*9;הליHA הAHHשX
 ילי דדחחNDה
 תנ+ 1(R)
 ץיA 1-/(L_גלO-L_Cל)//8(R5)
: ץיA PAתM_KOה(R6)
 חPץנ 8
 C‏+ 1(R6)
 תנ+ 1(R)
 דיKל #-2(R5)
 C‏ל -1
 תנ+ 1(R);KCM
 BPח 1DH;‏ACש
 Cהנ 20
 י =.[24:1]
 BיM RC;Bש‏יCלEHיE N תAניCי
 ץMH ‏_תAנ
 CלKל (W)
 Cהל 24
 ילי (S)
 C‏M יMס_Pת
 CגP =0303 0303 0303 0303H
 ילי (S)
 תנ CלCל+1;זOPMיPOBAHיE KP
 Cהל 32
 ‏Eה
 י =1
 Cהל 6
 C‏M CלCל+1
 Cהנ 32
 ‏Eה
 י =1
 Cהל 7
 ילי (S)
 HTצ =0D8H
 Cהל 48
 C‏M CלCל+1
 Cהנ 8
 ילי =[64:57]0D8H
 תנ+ 1(R)
 Bה
 ילי (S)
 תנ+ 1(R)
 C‏ =[48:41]0D8H
 תנ+ 1(R)
 CליA 1(W)
 C‏ ‏_תAנ
 B‏ד (W)
 נגל PAת1
PAת2 ץיA KנPת(R3);PAתMETKA הOPOצKי
 נB תAסBKA(R)
 נHP נEPEAהPECAדיס
 C‏ ‏_תAנ
 ץMH =L_גגT
 ץי R
 ץיA (W)
PAת3 C‏ MAC_TEC(W)
 נזA (R)
 HTצ MAC_TEC(W)
 נזA (R)
 תנ MAC_TEC(W);POCניCר גץזEPA
 נHP נEPEAהPECAדיס
 CליA 1(W)
 Bי R
 B‏ד (W)
 נגל PAת3
 נזA (RA)
 ניHP (RA)
 CליA 1(RC)
 C‏ KOל_הOP
 B‏ד (RC)
 נגל PAת0
 C‏ =T'היCK_BKג'
 תנ METKA_TOMA
 C‏ ‏יC_הOP_תAM
 CלK =6;הליHA ûAנKי
 ץי W;AהPEC HA‏AלA היPEKTOPיס
 C‏ ‏יCלO_Pת;נOC‏ET הליHש METKי TOMA
 CהלM 2;*4
 CלKל 127(W);הEלEHיE HAדEלO
 Cהנ 7
 תנ הלי_תAנ;הליHA METKי TOMA
 Cהל 40
 ילי PAתMEP_Pת;הליHA OCHOBHOך ‏ACTי TOMA
 ילי =2700 0000 0000 0000H
 תנ METKA_TOMA+3
 C‏ ‏יC_הOP_תAM
 Cהל 48
 C‏M KOל_הOP;HOMEP נEPBOך תAנACHOך הOP
 Cהל 24
 ילי (S)
 C‏M PAתMEP_Pת
 B‏ הלי_תAנ
 תנ KOH_גלOK;HOMEP גלOKA הץגלס METKי
 ילי (S)
 תנM METKA_TOMA+4
 Cהל 48 ;‏יCלO BT
 יליל (W)
 תנ METKA_TOMA+5
 C‏ יMס_Pת
 תנ METKA_TOMA+2
 תנ METKA_TOMA(W)
 C‏ =.25
 תנ METKA_TOMA+1(W);HOMEP HA‏AלרHOחO גלOKA
 C‏ PAתMEP_Pת
 תנ METKA_TOMA+2(W);הליHA BיPT. TOMA
תAניCר_METKי
 C‏ =METKA_TOMA
 C‏M ;OCHOBHAס METKA TOMA
 נB תAניCר(R7)
 נHP OûיגKA_OגMEHA
 C‏ =METKA_TOMA;הץגלר METKי TOMA
 C‏M KOH_גלOK
 נB תAניCר(R7)
 נHP OûיגKA_OגMEHA
 נג BXOה
נEPEAהPECAדיס
 C‏ ‏יC_הOP_תAM
 B‏Oג
 ץי W
: ניPB HET_PEת_הOP(W);נOיCK CBOג הOPOצKי
 CליA 1(W)
 נז ‏יC_הOP_תAM
 C‏ TAג_הEז-1(W)
 נHP #-2
 נז ‏יC_הOP_תAM
 C‏ל -1(W)
 CלK KOל_הOP
 ץMH ‏_חOלO;1/19
 BהM
 ץMH ‏_חOלד
 ץיM R
 Cהל 16
 יליל (R)
 CהלM 32
 תנ דדחחNDה
 ילי PAג
 נז ‏יC_הOP_תAM
 תנ TAג_הEז-1(W)
 C‏ זדדחח
 ילי =[57]02H
 תנ זדדחח
 ץיA Kנהת(3);הEזEKTAדיס הOPOצKי
 נB תAסBKA(R)
 נHP HE_ניûETCס_CA
 C‏ =.57
 תנM זדדחח
 Cהל 16
 תנ ץדדחח;AהPEC תAנACHOך הOPOצKי
 נג PAת2
‏TEHיE
 C‏M =00 06 84 0083 000000H
 נB זOP_KנPO(R)
 נB נץCK_OגMEHA(R)
 נHP (R7)
 C‏ =גץזK
 ץי R4
 C‏ KתAנ
 ץיM W
 ץי R1;גAתA MACCיBA OגMEHA
‏TE0 ץיA -127(R2)
 C‏
: תנ (S)
 C‏T+ 1(R4)
 תנT+ 1(R1)
 CלK (S)
 דיKל #-2(R2)
 HTצ+ 1(R4)
 נHP KCM_HE_COBנ
 C‏ NתAנ
 HTצ+ 2(R4);HOM TOMA; HOM גלOKA
 BהM
 CלK =.25
 תנM NתAנ
 Cהנ 24
 נHP Oûג_CלCל
 דיKל ‏TE0(W)
 נג (R7)
תAניCר
 C‏M =05 84 0083 000000H
 נB זOP_KנPO(R)
 C‏ KתAנ
 ץיM W;KOל תAניCEך
 ץי R1;גAתA MACCיBA OגMEHA
 C‏ =גץזK
 ץי R4;גAתA גץזEPA KAHAלA
 C‏ זלA_‏TE
 נPB תAנ0
 C‏ =08 04 0000 000000H
 יליל L_KנP(R3)
 תנ (R2)
 C‏ KKOM
 תנ -1(R2)
תAנ0 ץיA -127(R2)
 C‏
: תנ (S)
 C‏T+ 1(R1)
 תנT+ 1(R4)
 CלK (S)
 דיKל #-2(R2)
 תנ+ 1(R4);KCM
 C‏ NתAנ
 תנ+ 2(R4);HOM TOMA; HOM גלOKA
 CלK =.25
 תנ NתAנ
 דיKל תAנ0(W)
 נB נץCK_OגMEHA(R)
 C‏ זלA_‏TE
 נPB (R7)
 C‏ =גץזK
 ץי R4
 C‏ KתAנ
 ץי W
KCM C‏
 ץיA -127(R3)
: תנ (S)
 C‏T+ 1(R4)
 CלK (S)
 דיKל #-1(R3)
 HTצ+ 3(R4)
 נHP OûיגKA_OגMEHA
 דיKל KCM(W)
 נג (R7)
זOP_KנPO
 C‏M HOM_KAH;זOPMיPOBAHיE גAתש KAHAלA
 ץMH =L_KH
 נזC
 ץיA גץזEP(RB)
 נזA (RB)
 ץיA דדחחN-KH(R1)
 נזA (RB)
 ץיA KAH_נPO-KH(R2)
 C‏ל -1
 תנ+ 1(R1)
 C‏ HOM_ץCT
 ץי R6
 Cהל 56
 נז HOM_KAH
 CלK HOM_KAH+1
 ץי R5;AהPEC MנK
 י =.[64:57]
 תנ ץCTP
 ץיי R3(R2)
 ילי (S)
 CלK =גץזK-L_גלO
 תנ KKOM
 C‏ =1
 B‏ הלי_תAנ
 ץי W
 תנM KתAנ
 ץי R4;HA‏AלרHשך גלOK
 Cהל 24
 C‏M יMס_Pת
 CגP =0303 0303 0303 0303H
 ילי (S)
 תנ NתAנ
זOP0 C‏ל (R4)
 CליA 1(R4)
 ץMH ‏_דילO;1/190
 BהM ;דיליHה
 ץMH ‏_חOלד;*19
 תנ HOM_חOל
 Bה
 ץMH ‏_תAנד;*10
 Cהל 8
 CלK =100H
 תנM HOM_תAנ
 B‏ ‏_דיל;MAX דיליHהP
 נגP HEBEP_היAנ
 Cל ‏_דיל
 Cהל 16;דיליHהP
 ילי HOM_חOל
 Cהל 16
 ילי HOM_תAנ
 תנ (R1)
 Cהל 16
 תנ (S)
 HTצ -1(R1)
 Cהנ 32
 נPB HET_ץCT
 C‏ =07 64 0000 000000H
 ילי ץCTP
 יליל (R1)
 תנ+ 1(R2)
 תנ L_KנP-1(R2);KK ‏TE
 CליA 1(R1)
HET_ץCT C‏ =31 54 0000 000000H
 ילי ץCTP
 יליל (R1)
 תנ+ 1(R2)
 תנM L_KנP-1(R2);KK ‏TE
 תנ+ 1(R1)
 C‏ =08 04 0000 000000H
 יליל -1(R2)
 תנ+ 1(R2)
 CלK =L_KנP
 תנ L_KנP-1(R2);KK ‏TE
 C‏ KKOM;KOM=תנ/C‏
 CלK =L_גלO
 תנ KKOM
 תנ+ 1(R2)
 CלK =.49
 תנ L_KנP-1(R2);KK ‏TE
 דיKל זOP0(W)
 HTצ =.43
 תנ L_KנP-1(R2);KK ‏TE
 C‏ KKOM
 HTצ =.43
 תנ -1(R2)
 ץיA -7(R8)
 נג (R)
תAסBKA
 ץיA -7(R8);זOPMיPץET HץC
 C‏ HOM_ץCT
 ץי R6
 Cהל 56
 נז HOM_KAH
 CלK HOM_KAH+1
 ץי R5
 י =.[64:57]
 תנ ץCTP
 ץיי W(R3)
: C‏ (W)
 י =.[56:1]
 ילי ץCTP
 תנ+ 1(W)
 י =.43
 נHP #-2
נץCK_OגMEHA
 Bי R3
 נB צהץ_נPE(W)
 BPח 280H(R5)
 תנ ץCK
 BPח 284H(R5)
 י =.[19:1]
 תנ ACK
 BPח 286H(R5)
 תנ CCK
 Cהנ 1
 י =7
 BהM
 נז (S)
 נPB נEPEK_XAP
 BPח 686H(R5);ECTר TPג-A
 Cהנ 24
 י =0FFH
 נז HOM_KAH
 C‏M HOM_KAH+1
 Cהנ 56
 B‏Oג (S);CM=HOM_ץCT
 נג נץCK_OגMEHA
נEPEK_XAP ץיA X0(W);XAP=0
 נג AHAלית_CCK
 ץיA X1(W);XAP=1
 נג AHAלית_CCK
: נג Oûג_OגM;XAP=2 OTKAת KAHAלA
: נג Oûג_OגM;XAP=3 Oû AהPECA Bץ
: נג Oûג_OגM;XAP=4 Bץ HE HAךהEHO
: BPח 686H(R5);XAP=5 Bץ תAH=50 00 0A
 נג צץצץ
 CTOנ 6;XAP=6 Oûיג ‏TEHיס Oתץ
 נג BXOה
 CTOנ 7;XAP=7 נEPEXOה B KAHAלE
 נג BXOה
AHAלית_CCK נז (W)
 ץיA (R4);הליHA
 C‏ CCK
 Cהלנ+ 1(W)
 HTצ (R3)
 י =.[64:57]
 נHP Oû_HOM_ץCT
: Bה
 HTצ+ 1(W)
 י+ 1(W)
 נז -2(W)
 נPB
 דיKל #-2(R4)
Oûג_OגM BPח 680H(R5)
 C‏ ACK
 ילי =08 04 0000 000000H
 תנ KנKP+2
 C‏ KגץC;ץTO‏HEHיE COCTOסHיס
 י =.[56:1]
 ילי ץCTP
 תנ KגץC
 C‏ =KגץC
 נB צהץ_נPE(W)
 BPח 686H(R5)
 C‏ גץC
 HTצ =53H
 י =0FFH
 נPB KOPPEKדיס
 C‏ KנOB(R6)
 CלK =.17
 תנ KנOB(R6)
 דיKל נץCK_OגMEHA(R8)
 CלK =.33
 תנ KנOB(R6)
 ץיA ץCK(W)
 נז AגגץC
 ץיA (R4)
 חPץנ 5
 C‏+ 1(W)
 תנ+ 1(4)
 Bי R4
 תנ AגגץC
 HTצ =גץז_CTA+300
 נPB MHOחO_OûיגOK
 C‏ גץC
 י =.[64:61]
 נHP TPEגץETCס_BMEû
 תנ ץCTP
OKOT C‏ =1
 נג (R)
KOPPEKדיס
 C‏ ץCK
 י =.48
 נHP KOPT+1;ECTר TEח !!
 C‏ KנOB(R6);C‏ET‏יK KOPPEKדיך
 CלK =.49
 תנ KנOB(R6)
 C‏ ץCK
 י =.[19:1];AהPEC גץזEPA
 Cהל 3
 C‏M גץC+2
 Cהנ 48
 BהM
 Cהנ 48
 B‏Oג (S)
 CלK (S)
 ץיA -2(R8)
ד C‏לM 2(R8)
 Cהנ 3
 C‏ גץC+2
 Cהלה -24
 י =0FFH
 C‏לM 2(R8)
 CלK -2(S)
 Cהנ 3
 ץיM W
 Cהנה -56
 HTצ (W)
 תנM (W)
 דיKל ד(R8)
 C‏ ץCK
 י =.43
 נPB (R);KOMAHהA נOCלEהHסס B דEנO‏KE
 C‏ KנKP
 י =.[56:1]
 ילי ץCTP
 תנ KנKP
 C‏ =KנKP
 ץיA נץCK_OגMEHA+1(W)
 נג צהץ_נPE
KOPT
 BPח 680H(R5)
: C‏ ץCK
 C‏M ACK;זOP נPOחPAMMש ‏TEHיס גEת TEחOB
 ץי W;AהPEC KOMAHהש
 ילי =08 04 0000 000000H;BOתBPAT B דEנO‏Kץ
 BיM R3;AהPEC דEנO‏Kי
 BיM R;AהPEC BOתBPATA
 C‏M -1(W)
 י =0FF FF 0B 0000 FFFFFFH;גEת TEחOB י דEנO‏Kי
 ילי =00 00 30 0094 000000H;הליHA גEת TEחOB
 תנ KנKPT+2
 י =.[19:1];גAתA גץזEPA
 C‏M -3(W);נOיCK נO יהEHTיזיKATOPץ
 תנ KנKPT
 ץיA KנKPT(R3)
 נB נץCK_OגMEHA(R)
 ץיM R4;KOT
 ץי W;גAתA גץזEPA
 ץיי R3(W)
 ץיA -16(R8)
: ץיA -64(R)
PACנ C‏+ 1(W);PACנAKOBKA גץזEPA C TEחAMי
 Cהל 64(R)
 C‏M (W)
 Cהל (R)
 ילי (S)
 C‏M (W)
 Cהל 12(R)
 י =03F0H
 נזC
 ץPT
 C‏ (S)
 תנT+ 1(R3)
 CליA 8(R)
 ניHP PACנ(R)
 CליA 1(W)
 דיKל PACנ-1(R8)
 C‏ (S);BOCCTAHOBלEHיE
 ץיM R
 ץיM R3
 תנM KנKPT;KOMAHהA BOתBPATA B נPEPBAHHOE MECTO
 ניHP OKOT(R4);HE נPO‏לי, A XOTEלי
 י =.43
 נPB (R);נOCלEהHסס KOMAHהA
 C‏ =KנKPT;BOתBPAT B דEנO‏Kץ
 ץיA נץCK_OגMEHA+1(W)
 נג צהץ_נPE
MHOחO_OûיגOK C‏ =34H
 נג BXOה
TPEגץETCס_BMEû C‏ =30H
 נג BXOה
HE_ניûETCס_CA C‏ =9H
 נג BXOה
HET_PEת_הOP C‏ =28H
 נג BXOה
Oû_HOM_ץCT C‏ =27H
 נג BXOה
Oûג_CלCל C‏ =31H
 נג BXOה
KCM_HE_COBנ C‏ =32H
 נג BXOה
OûיגKA_OגMEHA C‏ =33H
 נג BXOה
TOM_HE_MOך C‏ =35H
 נג BXOה
HEBEP_היAנ C‏ =39H
 נג BXOה
HAהO_נOB BPח 681H(R5);HOלר KAHAלA
 C‏ KנOB(R6)
 CלK =1
 תנ KנOB(R6)
 C‏ ACK
 B‏ =1
 ץיA נץCK_OגMEHA+1(W)
 נג צהץ_נPE
HET_Oûג BPח 686H(R5)
 C‏
 תנ ץCTP
 נג (R)
TECT_Mה
 C‏ ‏_תAנ
 ץMH ‏_חOל
 C‏M O‏EPEהר+3(R1)
 Cהנ 20
 י =.[16:1]
 ץMH (S);10*19= 190
 C‏M O‏EPEהר+3(R1)
 Cהנ 12
 י =0FFH
 ץMH ‏_תAנ
 CלK (S)
 Cהל 12
 C‏M O‏EPEהר+3(R1)
 י =0FFFF FFFF 0000 0FFFH
 ילי (S)
 תנ CלCל+3(R1)
 דיKל TECT_Mה(R1)
 BPח 1DH;‏ACש
 תנ X
HA‏_נPOCM תנH (S)
 ץיA -3(R9)
CלEה_תAסB C‏ =0EB6D B6DB EB6D B6DBH
 ץיA MAC_‏TE(W)
 חPץל L_גלO*L_KלA-1
 תנ+ 1(W)
 C‏ CלCל+3(R9);PACנAKOBKA תAסBKי
 י =0FFFH
 נPB Kד
 ילי (S)
 C‏M CלCל+3(R9)
 Cהל 4
 BהM
 תנM HOM_KAH
 Cהל 4
 BהM
 תנM HOM_ץCT
 Cהל 4
 BהM
 נזC
 תנM יMס_Pת
 Cהל 8
 BהM
 תנ הלי_תAנ
 B‏ =1
 Cהל 12
 ילי =0FFFH
 CלK CלCל+3(R9)
 תנM CלCל+3(R9)
 Cהל 4
 BהM
 ץיA תAניCר(W)
 ץיM R
: ניHP #+1(R)
 ץיA ‏TEHיE(W)
 Cהל 4
 BהM
 תנM זלA_‏TE
 Cהל 4
 BהM
 ץיM R
 Cהל 20
 C‏ =MAC_‏TE
 BהM
: ניPB #+1(R)
 נB RAN(R);HOMEP גלOKA OT RAN
 נזA (W)
 נB (R7)
Kד דיKל CלEה_תAסB(R9)
 C‏ (S)
 נHP HA‏_נPOCM
 ץיA -3(W)
: Cהל 16
 C‏M KנOB+3(W)
 Cהנ 32
 י =.[16:1]
 ילי (S)
 דיKל #-2(W)
 נג BXOה
‏TEHיE_METKי
 C‏ =1
 תנ הלי_תAנ
 C‏ =METKA_TOMA
 C‏M ;‏TEHיE 0-גלOKA
 נB ‏TEHיE(R7)
 נHP OûיגKA_OגMEHA
 C‏ METKA_TOMA
 HTצ =T'היCK_BKג'
 נHP TOM_HE_MOך
 C‏ METKA_TOMA+3
 Cהנ 40
 י =0FFH
 תנ הלי_תAנ;PAתMEP METKי TOMA
 תנ HA‏_גלOK
 C‏ =METKA_TOMA
 C‏M ;‏TEHיE METKי TOMA(הץגלר=257A7H)
 נB ‏TEHיE(R7)
 נHP OûיגKA_OגMEHA
 C‏ METKA_TOMA+4
 י =.[19:1]
 תנ KOH_גלOK;KOניס METKי
 C‏ METKA_TOMA+5
 ץי W
 CליA METKA_TOMA(W);גAתA היPEKTOPיס
 Cהנ 48
 B‏Oג =1
 ץי R;MAX ‏יCלO BT B היPEKTOPיי
 ץיי R1(W)
 CליA 4(W)
 ץיA (R3)
CKAH CליA 4(R1);נOהC‏ET הליHש היPEKTOPיס
 C‏ (R1)
 נPB (RA)
 HTצ יMס_Pת
 C‏M 1(R1)
 HTצ יMס_Pת+1
 Cהנ 32;24!!
 ילי (S)
: נHP #+1
 ץיי R3(R1);AהPEC COBנABûEחO יMEHי BT
 דיKל CKAH(R)
 נג (RA)
PEMOHT
 C‏ HOM_גלO
 ץMH ‏_תAנO;1/10
 ץי RC;HOMEP הOPOצKי
 ץMH ‏_תAנ;HA‏ HOMEP גלOKA HA הOPOצKE
 תנ HOM_גלO
 Bי RC
 ץMH ‏_חOלO;1/19
 BהM
 ץMH ‏_חOלד
 ץיM R
 Cהל 16
 יליל (R)
 C‏M METKA_TOMA+4;B MAח דדחח
 CהנM 24
 י =.[19:1]
 תנM KOל_הOP;N נEPBOך תAנACHOך הOPOצKי
 Cהנ 48
 תנ ‏יC_הOP_תAM
 B‏Oג =1
 ץי W
 ץיA TAג_הEז(R)
נOיCK_הOP C‏+ 1(R)
 HTצ -1(S);דדחח
 Cהל 32
: נHP #+1
 תנ -1(R)
 דיKל נOיCK_הOP(W)
 ץיA (R9)
 C‏ =1
 תנM הלי_תAנ
ץנP_הOP C‏ל (R9)
 Cהל 7
 CלK =MAC_‏TE
 C‏לM (R9)
 CלK HOM_גלO
 נB ‏TEHיE(R7)
 תנ CTEK+10(R9)
 CליA 1(R9)
 C‏ ‏_תAנ
 B‏ד (R9)
 נגל ץנP_הOP
 ץיA (R9)
 נB PAת0(RA)
BOC_הOP C‏ CTEK+10(R9)
 נHP KOH_BOC
 C‏ל (R9)
 Cהל 7
 CלK =MAC_‏TE
 C‏לM (R9)
 CלK HOM_גלO
 נB תAניCר(R7)
 נHP OûיגKA_OגMEHA
KOH_BOC CליA 1(R9)
 C‏ ‏_תAנ
 B‏ד (R9)
 נגל BOC_הOP
 נג תAניCר_METKי
COת_Pת
 C‏ =3;KOT=3
 ניHP BXOה(R3)
נOיCK_זPAח C‏ 1(W)
 י =.[19:1]
 נPB B_KOHEד
 תנ PAג
 C‏ HA‏_גלOK
 CלK PAתMEP_Pת
 B‏Oג PAג
 נגP ECTר_זPAח
 C‏ 2(W)
 י =.[19:1]
 CלK PAג
 תנ HA‏_גלOK
 CליA 4(W)
 דיKל נOיCK_זPAח(R)
KOT1 C‏ =1;HET MECTA הלס OניCATEלס זAךלA
 נג BXOה
ECTר_זPAח CליA -1(R1);נOCלEהHיך OניCATEלר
 ץיי R(W)
 B‏י R(R1);הליHA נEPEניCי
: C‏+ -1(R1)
 תנ 5(R1)
 דיKל #-1(R)
B_KOHEד C‏ יMס_Pת
 תנ (W)
 C‏ יMס_Pת+1
 י =.[64:33]
 ילי =.25;KOניס=1
 ילי HA‏_גלOK
 תנ 1(W)
 C‏ PAתMEP_Pת
 תנ 2(W)
 C‏ HA‏_גלOK
 CלK PAתMEP_Pת
 B‏ KOH_גלOK
 נMH תAניCר_METKי
 C‏ =2;HET נPOCTPAHCTBA הלס BT
 נג BXOה
יCK_Pת
 ניPB KOT1(R3);יMס HE HAךהEHO
 CליA -5(R1)
 B‏יOג R3(R1)
 ניגל #+3(R1)
: C‏ 4(R3)
 תנ+ 1(R3)
 דיKל #-1(R1)
: C‏
 חPץל 3;POCניCר XBOCTA
 תנ+ 1(R3)
 נג תAניCר_METKי
ץCT ץPצ;Bש‏יCלEHיE KOל תAניCEך HA הOPOצKE
 C‏ =59+11+7+49+L_גלO*9+7;נOלHשך Oג'EM תAניCי
 KOP 434H
 C‏M O_הOP
 B‏ =92+13+7+49+11+7;הליHA HA
 KOP 434H
 הEל (S)
 דEלז 52
 תנ ‏_תAנ
 תנ הלי_תAנ
 HTצ =.64
 תנ ‏_תAנד
 Bה
 נB נPEOג(W)
 תנ ‏_תAנO
 C‏ ‏_חOל
 HTצ =.64
 תנ ‏_חOלד
 Bה
 נB נPEOג(W)
 תנ ‏_חOלO
 C‏ ‏_חOל;‏יCלO תAניCEך HA דיליHהPE
 ץMH ‏_תAנ
 נB נPEOג(W)
 תנ ‏_דילO
 C‏ =.[40:33]
 ץיA -31(W)
: ץPח 40H+31(W);ץCTAHOBKA נPיניCKי
 CלK =0000 0100 0000 0100H
 דיKל #-1(W)
 ץPח 22H;ץCTAHOBKA PMC
 C‏ =.[64:33]
 ץPח 29H;ץCTAHOBKA PCנO
 C‏ =גץז_CTA
 תנ AגגץC
 C‏ =1200H;גOנ י TT
 ץP יC1
 C‏ =[48:33]0C002H
 ץPח 26H;ץCTAHOBKA MACKי
 C‏ HOM_KAH
 ץMH =L_KH
 נזC
 ץיA גץזEP(RB)
 C‏ HOM_KAH
ץ0 C‏M =.44;OגHץלEHיE KOHTP
 נז (S)
 Cהנ
 ץPח 24H
 C‏
 ץPח 24H
 ץיA 32(R5)
CHס_יM
 C‏ל =08 40 0000 000000H;נEPEXOה B KAHAלE
 CליA -8(R5)
 ץPח 484H(R5)
 ץיA -99(W)
: ץי ;תAהEPצKA
 דיKל #(W)
 C‏
 ץיA -7(W)
: נזA (R5);POCניCר MנK
 ץPח 80H+7(W)
 דיKל #-1(W)
 BPח 686H(R5);OגHץלEHיE KAHAלA
 ניHP CHס_יM(R5)
 ץיA KץCT(R3)
 נג תAסBKA
RAN C‏ ‏_דיל
 ץMH ‏_חOל
 ץMH ‏_תAנ
 HTצ =.64
 BהM
 נB נPEOג(W)
 C‏M X
 Cהנ 4
 HTצ X
 CהלM 11
 HTצ (S)
 י =.[52:1]
 תנ X
 ץMH (S);1/808*19*10= 153 520 /200/
 Bה
 ץMH (S);808*19*10= 153 520 /200/
 נג (R)
נPEOג ץPצ 20H
 KOP 434H
 C‏M =‏'1.0'
 הEל -1(S)
 י =.[52:1]
 תנM PAג
 CלK =.[64:54]
 C‏M PAג
 Cהננ (S)
 CלK =8000 0000 0000 0001H
 ץPצ 230H
 נג (W)
צהץ_נPE BשT
 ץPח 484H(R5);נץCK KAHAלA
צץצץ BPח 24H
 C‏M =[36]11H
 נז HOM_KAH
 Cהנ
 י (S)
 נPB צץצץ
 י =.36
 נPB (W)
: CTOנ 0777H;
 נג #
 HליCT ץCל,ECלי
 ECלי 1=0
צץצץ C‏ =.19;300 MיליCEK.
 ץPח 1EH;TAךMEP
 ץנP 4;PAתP BHE_נPE
 C‏
: CלK =1;צהץ‎יך TECT
 נג #
BHE_נPE C‏ =1200H;גOנ,TT
 ץP יC1
 BPח 27H;חPBנ
 CגP =[48:33]0C002H
 HEה
 נPB צץצץ
 נזC
 נג #
: נג KOEC
: CTOנ 771H;יCEC
 נג BXOה
: CTOנ 0FFFH;TAךMEP
 נג צץצץ
KOEC
 BPח 24H
 Cהל 24
 י MAC_KAH
 נPB Oû_HOM_ץCT
 HEה
 B‏ =1
 תנ HOM_KAH
 נזC
 נז HOM_KAH+1
 ץיA (R5);AהPEC MנK
 ץMH =L_KH
 נזC
 ץיA גץזEP(RB)
 C‏ =1200H;גOנ י TT
 ץP יC1
 נג (W)
üKCTPAKOה
 תנ PAג;üTO CM
 Bי S
 תנ נOל_ץנP+16
 Bי W
 תנ נOל_ץנP+15
 ץיA נOל_ץנP(S)
 ץיA -12(W)
 ץPצ 230H
 BP יC3
 BPM יC7
: BיM 13(W)
 דיKל #(W)
 ץיA -6(W)
 C‏M נOל_ץנP+16;CTEK נOלרתOBATEלס
 ץי S
 C‏ PAג;üTO CM
: תנM AהP_Oתץ+6(W);נEPEניCר נAPAMETPOB BשתOBA
 דיKל #(W)
 ץיA CTEK(S)
 BP יC2;יCנ AהPEC üKC
 נזC
 נג #+1
: נג üKץ
: ץיA תAניCר(W);יCנ=1
 נג üK0
: ץיA ‏TEHיE(W);יCנ=2
üK0 C‏ AהP_Oתץ
 C‏M HOM_גלO
 ץיA BOת_üKC(R7)
 נג (W)
üKץ נB ץCT(R)
BOת_üKC תנ PAג;KOה OTBETA
 C‏ נOל_ץנP;יC3 - KOPPEKדיס AהPECA BOתBPATA
 ץP יC3
 C‏ נOל_ץנP+1;יC7
 ץP יC7
 ץיA 13(W)
 ץיA נOל_ץנP+15(S)
 C‏ (S)
: ץיM (W)
 CליA -1(W)
 ניHP #-1(W)
 נז נOל_ץנP+16;יP15
 ץיA (S)
 נז נOל_ץנP+15;יP14
 ץיA (W)
 ץPצ 30H
 C‏ PAג;KOה OTBETA
 BשX
 BCE
 ליTEP
METKA_TOMA
 KגלOK היCKי
 זיHיû
