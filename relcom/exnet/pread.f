From vak!vak%vak.home.kiae.su@kiae.UUCP Thu Oct  1 13:13:52 1992
Received: from kiae.UUCP by rdrel.relcom.msk.su with UUCP id AA00308
  (5.65c8/IDA-1.4.4 for vak@rd.relcom.msk.su); Mon, 5 Oct 1992 12:51:44 GMT
Received: by relcom.kiae.su; Mon,  5 Oct 92 10:46:29 +0300
Received:  by vak.home.kiae.su (UUPC/@ v4.06 from Ache, 04Mar92);
           Sun, 04 Oct 1992 23:04:52 MSD
Resent-To: vak@rdrel.relcom.msk.su
Resent-Message-Id: <AAE1qpga47@vak.home.kiae.su>
Resent-Organization: home
Resent-From: vak@vak.home.kiae.su (Serge V.Vakulenko)
Resent-Date: Sun,  4 Oct 92 23:04:46 +0400 (MSD)
X-Mailer: BML [MS/DOS Beauty Mail v.1.27]
Received:  by vak.home.kiae.su (UUPC/@ v4.06 from Ache, 04Mar92);
           Sun, 04 Oct 1992 23:01:35 MSD
Received: by relcom.kiae.su; Fri,  2 Oct 92 10:23:32 +0300
Received:  by igp.msk.su (UUPC/@ Release 3.00/fossil/28Oct91);
           Thu, 01 Oct 1992 10:13:55 MSD
To: vak@vak.home.kiae.su
Message-Id: <AAmKgogaj6@igp.msk.su>
Organization: IGP&Co
From: igph@igp.msk.su (Pasynkov I.G.)
Date: Thu,  1 Oct 92 10:13:52 +0300
Subject: text of arm_exnet_post_proccecor
X-Mailer: BML [MS/DOS Beauty Mail v.1.30]
Status: RO

program pread
C      A.D
       parameter (NNC=14)
C             ,txt='.txt')
       character*80 line,l,fdata*24,lines(10)
       character*560 lcat
       character nlot*14,nbrok*4,time*17
       character suf*1,dir*1
       character regp*2,regs*2
C               ,lreg*1,lregs*1,lregp*1
       integer inc(1:NNC)
C
C
      do 11 n=1,NNC
        inc(n)=0
   11 continue
      regp='-b'
      regs='+b'
    1 read(*,'(A)',END=999) line
    2 continue
c      if(line(1:11).EQ.'Конец файла') goto 999
      if(line(1:6).EQ.'> Get ') goto 600
      if(line(1:6).EQ.'> get ') goto 600
      if(line(1:9).EQ.'Почтовый ') goto 900
      if(line(1:9).EQ.'> CREATE ') goto 300
      if(line(1:10).EQ.'Архив лота') goto 700
      if(line(1:10).EQ.'> destroy ') goto 800
C=====================
      id=13
      ib=31
      dir='L'
      NC=1
      if(line(1:10).EQ.'> LONGINDE') goto 100
      if(line(1:10).EQ.'> longinde') goto 100
      if(line(1:10).EQ.'> LONGinde') goto 100
      id=16
      ib=34
C      dir='N'
C      NC=3
      if(line(1:10).EQ.'> LONGNEWI') goto 100
      if(line(1:10).EQ.'> longnewi') goto 100
      if(line(1:10).EQ.'> LONGnewi') goto 100
C================================
      id=0
      ib=9
      dir='I'
      NC=2
      if(line(1:7).EQ.'> INDEX') goto 200
      if(line(1:7).EQ.'> index') goto 200
C---------------------------
      if(line(1:8).EQ.'> STATUS') goto 2001
      if(line(1:8).EQ.'> status') goto 2001
      id=12
      ib=30
C      dir='J'
C      NC=4
      if(line(1:10).EQ.'> NEWINDEX') goto 200
      if(line(1:10).EQ.'> newindex') goto 200
C----------------------      NC=6
      if(line(1:10).EQ.'> NEWSTATU') goto 2000
      if(line(1:10).EQ.'> newstatu') goto 2000
c        write(*,'(1X,A)') line
      goto 1
C========LONGINDEX/LONGNEWINDEX=============
  100 continue
C======  признак 'Кто:'=========
      ik=0
      if(line(ib:ib+1).EQ.'..') then
       suf='S'
       NCD=10
       if(inc(NC+NCD).EQ.0) then
         OPEN(UNIT=NC+NCD,STATUS='NEW',FILE=dir//suf//'.txt')
         inc(NC+NCD)=1
       endif
      else
       NCD=0
       if(inc(NC+NCD).EQ.0) then
         OPEN(UNIT=NC+NCD,STATUS='NEW',FILE=dir//'.txt')
         inc(NC+NCD)=1
       endif

       OPEN(UNIT=NC+NCD,STATUS='NEW',FILE=dir//'.txt')
      endif
C  NC= 01 - L   NC+NCD= 11 - LS
C  NC= 03 - N   NC+NCD= 13 - NS
  101 read(*,'(A)',END=999) line
      if(line(1:6).EQ.'Конец ') goto 109
      if(line(1:2).EQ.'> ') goto 109
C =====  '+b' ======
 1010 nlot=line(1:14)
      ip=index(nlot,regp)
      is=index(nlot,regs)
      if((ip.EQ.0).AND.(is.EQ.0)) then
      if(line(1:2).EQ.'> ') goto 109
 1011 read(*,'(A)',END=999) line
        if(line(1:1).EQ.':') goto 1011
        goto 1010
      endif
      if(is.GT.0) then
       itt=index(line(is+2:),'.')+is
       nbrok=line(is+2:itt)
        lcat(41:41)='+'
C--для продажи - цена максимальная с 28 позиции:ism+28
       ism=0
      else
       itt=index(line(ip+2:),'.')+ip
       nbrok=line(ip+2:itt)
        lcat(41:41)='-'
C--для покупки - цена максимальная с 42 позиции:ism+28
        ism=14
      endif
C---kb+b207.1 24.10.91/10.00.00 [a-z]* ------------
      lcat(59:73)=' '//nlot
C----  MM.DD/HH-MM.DD/HH -----------------
      read(*,'(A)',END=999) line
      time=line(34:38)//line(42:44)//' '//line(53:57)//line(61:63)
      lcat(42:58)=time
C---описание лота, пропускаем шапку ----------
      read(*,'(A)',END=999) line
C--- номер конторы и суть лота ---------------
      read(*,'(A)',END=999) line
        lcat(1:40)=line(10:49)
        lcat(39:40)=fdata(9:10)
C        lcat(74:80)='N='//line(3:7)
        lcat(74:80)=nlot(1:2)//lcat(41:41)//line(4:7)
        ibl=81
      il=1
 1090 read(*,'(A)',END=999) lines(il)
C      write(*,'(A)') lines(il)
      if(lines(il)(1:6).EQ.'Конец ') goto 109
      if(lines(il)(1:2).EQ.'> ') goto 109
C--заголовок главной строки (если он есть)-----------
      if(lines(il)(1:9).EQ.':  кол-во') goto 1090
C------Пустая строка-------------
      if(lines(il)(1:16).EQ.':               ') goto 1090
C--Pазделитель лотов----
      if(lines(il)(1:4).EQ.': __') goto 1091
C========= Кто:......
      if(lines(il)(1:4).EQ.': **')  goto 1093
       il=il+1
       goto 1090
C==Конец ..........
  109 continue
C      CLOSE(NC+NCD)
      if(line(1:2).EQ.'> ') goto 2
      goto 1
C=====Mail Line=======
 1091 ibl=81
 1092 lcat(ibl:ibl+79)=lines(il-1)
      ibl=ibl+80
      if(il.EQ.2) goto 119
      do 1100 ii=1,il-2
       lcat(ibl:ibl+79)=lines(ii)
       ibl=ibl+80
 1100 continue
      goto 119
C======Строка сделки ==
 1093 ibl=81
      read(*,'(A)',END=999) line
      lcat(ibl:ibl+79)=line
      ibl=ibl+80
C==: _______________________
      read(*,'(A)',END=999) line
      goto 1092
C=== Запись стороки ========
  119 write(NC+NCD,'(A)') lcat(1:ibl-1)
      goto 101
C=====STATUS/NEWSTATUS======
 2000 id=id+1
 2001 ib=ib+1
      dir='C'
C      suf='C'
      NC=5
      NCD=0
      if(inc(NC+NCD).EQ.0) then
         OPEN(UNIT=NC+NCD,STATUS='NEW',FILE=dir//'.txt')
         inc(NC+NCD)=1
      endif
      goto 2003
C========== INDEX/NEWINDEX=======
  200 NDC=0
 2002 if(line(ib:ib+1).EQ.'..') then
       suf='S'
       NCD=10
       if(inc(NC+NCD).EQ.0) then
         OPEN(UNIT=NC+NCD,STATUS='NEW',FILE=dir//suf//'.txt')
         inc(NC+NCD)=1
       endif
      else
       NCD=0
       if(inc(NC+NCD).EQ.0) then
         OPEN(UNIT=NC+NCD,STATUS='NEW',FILE=dir//'.txt')
         inc(NC+NCD)=1
       endif
      endif
C  NC=2 -  I     NC+NCD=12 - IS
C  NC=4 -  J     NC+NCD=14 - JS
C  NC=5 -  IC    STATUS
C  NC=6 -  JC    NEWSTATUS
 2003 read(*,'(A)',END=999) line
C      write(*,'(1X,A)') line
C =====  '+b' ======
  201 if(line(1:2).EQ.'> ') goto 209
      if(line(1:6).EQ.'Конец ') goto 209
      ip=index(line(1:14),regp)
      is=index(line(1:14),regs)
      if((ip.EQ.0).AND.(is.EQ.0)) then
      if(line(1:2).EQ.'> ') goto 209
        read(*,'(A)',END=999) line
        goto 201
      endif
      if(is.GT.0) then
       itt=index(line(is+2:),'.')+is
        nbrok=line(is+2:itt)
        lcat(41:41)='+'
      else
       itt=index(line(ip+2:),'.')+ip
       nbrok=line(ip+2:itt)
        lcat(41:41)='-'
      endif
      nlot=line(1:14)
      ia=index(line(15:80),' ')
      do 202 i=ia+15,ia+25
       if(line(i:i).NE.' ') goto 203
  202 continue
  203 lcat(1:40)=line(i:i+39)
      lcat(39:40)=fdata(9:10)
      read(*,'(A)',END=999) line
      time=line(34:38)//line(42:44)//' '//line(53:57)//line(61:63)
      lcat(42:58)=time
      lcat(59:73)=' '//nlot
       read(*,'(A)',END=999) line
       lcat(74:79)='  '//nbrok
       iend=79
      if(NC.GT.4) then
         if (line(1:2).NE.': ') goto 208
       lcat(74:75)='  '
         if(line(3:6).EQ.'Нет ') then
           read(*,'(A)',END=999) line
           goto 208
         endif
        lcat(74:75)=' +'
  207  ie=index(line(3:),' ')
       lcat(iend+1:iend+ie+1)=line(2:ie+1)
       iend=iend+ie
       read(*,'(A)',END=999) line
       if (line(1:2).EQ.': ') goto 207
      endif
  208  write(NC+NCD,'(A)') lcat(1:iend)
C       write(*,'(1X,A)') lcat(1:iend)
      goto 201
  209 continue
C      CLOSE(NC+NCD)
      if(line(1:2).EQ.'> ') goto 2
       goto 1
C=========CREATE=================
 300  NCR=8
      do 301 i=80,3,-1
        if(line(i:i).NE.' ') goto 302
  301 continue
  302 j=index(line(10:),' ')
      j=j+10+35
      lcat(1:41)=line(j:i)
      read(*,'(A)') l
      if(l(1:11).EQ.'Создан лот ') then
          nlot=l(12:25)
         lcat(59:73)=' '//nlot
        if(inc(NCR).EQ.0) then
           NL=0
           OPEN(UNIT=NCR,STATUS='NEW',FILE='create.txt')
           inc(NCR)=1
        endif
        NL=NL+1
        read(*,'(A)') l
        time=l(23:27)//l(31:33)//l(43:48)//l(52:54)
        lcat(42:58)=time
      write(NCR,'(A)') lcat(1:73)
      endif
      goto 1
C======> Get iba-b27.200
 600  read(*,'(A)',END=999) line
       write(*,'(1X,A)') line
      goto 1
C======Архив лота ibf-b103, модем ....=================
C NAR=9
 700  i=index(line(12:),',')
      nlot=line(12:i+10)
      ip=index(nlot,regp)
      is=index(nlot,regs)
      if(ip.GT.0) then
        itt=index(nlot(ip+2:),'.')+ip
        nbrok=nlot(ip+2:itt)
        lcat(41:41)='-'
      endif
      if(is.GT.0) then
       itt=index(nlot(is+2:),'.')+is
        nbrok=nlot(is+2:itt)
        lcat(41:41)='+'
      endif
        NAR=9
        if(inc(NAR).EQ.0) then
           OPEN(UNIT=NAR,STATUS='NEW',FILE='R.txt')
           inc(NAR)=1
        endif
      lcat(1:40)=line(i+13:i+52)
      lcat(39:40)=fdata(9:10)
      read(*,'(A)') line
      time=line(23:27)//line(31:33)//line(43:48)//line(52:54)
      lcat(42:58)=time
      lcat(59:73)=' '//nlot
C---Пустая---
      read(*,'(A)') line
C-- Подписчики:
      read(*,'(A)') line
C---Список--
      NL=0
      iend=81
  701 read(*,'(A)') line
      if(line(5:10).EQ.'      ') goto 702
      ie=index(line(5:),' ')
      lcat(iend:iend+ie)=line(5:ie+5)
      iend=iend+ie+1
      NL=NL+1
      goto 701
C--индикатор подписки---
  702 lcat(74:80)='   '//nbrok
C      if(NL.GT.0) lcat(74:80)=' Есть  '
      if(NL.GT.0) lcat(76:76)='+'
      write(NAR,'(A)') lcat(1:iend)
C==Сообщения:  =====
      read(*,'(A)') line
C==1) ====
  704 read(*,'(A)') line
      if(line(2:2).NE.')') goto 1
      lcat(74:79)='  '//line(1:4)
C---From: exn@exnet1.msk.su--
       read(*,'(A)') line
C-- Date: Mon NOv 18
       read(*,'(A)') line
       time=line(15:26)//'/ '//line(33:34)
       lcat(42:58)=time
       lcat(59:73)=' '
       read(*,'(A)') line
       read(*,'(A)') line
       lcat(74:80)=line(7:13)
       iend=81
       ie=65
       lcat(iend:iend+ie)=line(15:16+ie)
       iend=iend+ie
       write(NAR,'(A)') lcat(1:iend)
       read(*,'(A)') line
       goto 704
C=========destroy=====
C   NDES=10
 800  read(*,'(A)') line
       if(line(1:13).NE.'Уничтожен лот') goto 1
       nlot=line(15:28)
       ip=index(line(15:28),regp)
       is=index(line(15:28),regs)
       if((ip.EQ.0).AND.(is.EQ.0)) then
       goto 1
      endif
        NDES=10
        if(inc(NDES).EQ.0) then
           OPEN(UNIT=NDES,STATUS='NEW',FILE='U.txt')
           inc(NDES)=1
        endif
      if(is.GT.0) then
        lcat(34:34)='+'
      else
        lcat(34:34)='-'
      endif
      lcat(1:33)=line(1:33)
      lcat(35:58)=fdata
      lcat(59:73)=line(14:28)
      write(NDES,'(A)') lcat(1:73)
      goto 1
C====Почтовый Сервер=======
  900  read(*,'(A)') line
       read(*,'(A)') line
       fdata=line(1:24)
       goto 1
C======Конец файла==================
  999 stop
      end

