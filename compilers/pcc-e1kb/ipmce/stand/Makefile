SHELL   = /bin/sh
INSTALL = /usr/SVSB
LIB     = $(INSTALL)/lib
INCPATH = -I.
COPTS   = ${INCPATH} -DSTANDALONE -L
LDFLAGS = -s
RELOC   = 0x30001
BASADDR = 1
CC      = vcc
LD      = vld
AS      = vas
AR      = var
RANLIB  = vranlib
TMAKER  = vtestmaker
TDIS    = vtestdis
DIS     = vdis

LIBSA   = libsa.a

BOOTSRCS = boot.c conf.c bootxx.c confxx.c prf.c sys.c srt0.c\
	machdep.c dos.c hd.c hp.c mt.c scio.c bcio.c mem.c

SASRCS  = hdformat.c hpformat.c hello.c fdisk.c copy.c\
	bcxtest.c bctest.c bcrtest.c bcstest.c uctest.c

DRIVERS = dos.o hd.o hp.o mt.o scio.o bcio.o mem.o

UTILS   = hdformat.sa hello.sa fdisk.sa copy.sa compare.sa #hpformat.sa

TESTS   = uctest.sa bctest.sa bcrtest.sa bcstest.sa bcxtest.sa

ALL     = boot.tst elftest.tst elfutest.tst elfbtest.tst $(UTILS) $(TESTS)

all:    ${ALL}

floppy: all
	doscp boot.tst *.sa a:/tester

$(UTILS) $(TESTS): $(LIBSA) srt0.o

boot.tst: boot
	vtestmaker -a1 boot -o boot.tst

elftest.tst: elftest
	vtestmaker -a1 elftest -o elftest.tst

elfutest.tst: elfutest
	vtestmaker -a1 elfutest -o elfutest.tst

elfbtest.tst: elfbtest
	vtestmaker -a1 elfbtest -o elfbtest.tst

elftest: elftest.o relsrt0.o ${LIBSA}
	$(LD) -C -T${RELOC} -o elftest relsrt0.o elftest.o ${LIBSA} -ltst

elfutest: elfutest.o relsrt0.o ${LIBSA}
	$(LD) -C -T${RELOC} -o elfutest relsrt0.o elfutest.o ${LIBSA} -ltst

elfbtest: elfbtest.o relsrt0.o ${LIBSA}
	$(LD) -C -T${RELOC} -o elfbtest relsrt0.o elfbtest.o ${LIBSA} -ltst

boot.dis: boot.tst
	vtestdis boot.tst > boot.dis

${LIBSA}: sys.o conf.o ${DRIVERS} prf.o machdep.o
	$(AR) cr ${LIBSA} $?
	$(RANLIB) ${LIBSA}

srt0.o: srt0.c
	$(CC) -A -c -DRELOC=${RELOC} ${COPTS} srt0.c

relsrt0.o: srt0.c
	cp srt0.c relsrt0.c
	$(CC) -A -c -DRELOC=${RELOC} -DREL ${COPTS} relsrt0.c
	rm -f relsrt0.c

boot:	boot.o relsrt0.o bootconf.o ${LIBSA}
	$(LD) -C -T${RELOC} -o boot relsrt0.o boot.o bootconf.o ${LIBSA} -ltst

z.tst:  z
	vtestmaker -a1 z -o z.tst

z:      z.o relsrt0.o ${LIBSA}
	$(LD) -C -T${RELOC} -o z relsrt0.o z.o ${LIBSA} -ltst

bootconf.o: conf.o
	cp conf.c bootconf.c
	$(CC) -c ${COPTS} -DBOOT bootconf.c
	rm -f bootconf.c

clean:
	rm -f *.o *.i *.b *.a *.out *.tst *.dis *.sa boot core bootconf.c relsrt0.c

lint:
	lint ${COPTS} -hxbn boot.c ${BOOTSRCS} |\
	    grep -v 'possible pointer alignment' |\
	    grep -v 'struct/union .* never defined'

install: ${INSTALL}/lib/srt0.o ${INSTALL}/lib/$(LIBSA)\
	${INSTALL}/pult/boot.tst ${INSTALL}/pult/elftest.tst

${INSTALL}/lib/srt0.o: srt0.o
	cp srt0.o ${INSTALL}/lib

${INSTALL}/lib/$(LIBSA): $(LIBSA)
	cp $(LIBSA) ${INSTALL}/lib
	$(RANLIB) ${INSTALL}/lib/$(LIBSA)

${INSTALL}/pult/boot.tst: boot.tst
	cp boot.tst ${INSTALL}/pult

${INSTALL}/pult/elftest.tst: elftest.tst
	cp elftest.tst ${INSTALL}/pult

depend:
	$(CC) -M ${COPTS} ${BOOTSRCS} ${SASRCS} | awk ' { if ($$1 != prev)\
		{ print rec; rec = $$0; prev = $$1; }\
		else { if (length(rec $$2) > 78) { print rec; rec = $$0; }\
		else rec = rec " " $$2 } }\
		END { print rec } ' > makedep
	echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp Makefile Makefile.bak
	ed - Makefile < eddep
	rm eddep makedep
	echo >> Makefile
	echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile
	echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile
	echo '# see make depend above' >> Makefile

.c.o:
	$(CC) $(COPTS) -c $<

.c.s:
	$(CC) $(COPTS) -S $<

.c.i:
	$(CC) $(COPTS) -P $<

.o.tst:
	$(LD) $(LDFLAGS) -C -T1 srt0.o $*.o -ltst -o $*.out
	$(TMAKER) -a1 $*.out -o $*.tst
	-rm -f $*.o $*.out

#.c.tst:
#        $(CC) -c $(COPTS) $<
#        $(LD) $(LDFLAGS) -C -T1 srt0.o $*.o -ltst -o $*.out
#        $(TMAKER) -a1 $*.out -o $*.tst
#        -rm -f $*.o $*.out

.c.out:
	$(CC) -c $(COPTS) $<
	$(LD) $(LDFLAGS) -C -T$(RELOC) srt0.o $*.o -ltst -o $*.out
	-rm -f $*.o

.o.sa:
	$(LD) $(LDFLAGS) -C -T$(BASADDR) srt0.o $< $(LIBSA) -ltst -o $@

.tst.dis:
	$(TDIS) $< > $@

.sa.dis:
	$(DIS) $< > $@

.SUFFIXES: .c .o .s .i .tst .out .dis .sa

# DO NOT DELETE THIS LINE
# make depend uses it

boot.o: boot.c ./param.h ./machparam.h ./types.h ./dir.h ./a.out.h ./svsb.h
conf.o: conf.c ./param.h ./machparam.h ./types.h ./inode.h ./fs.h ./saio.h
conf.o: ./svsb.h
bootxx.o: bootxx.c ./param.h ./machparam.h ./types.h ./dir.h ./a.out.h
bootxx.o: ./svsb.h
confxx.o: confxx.c ./param.h ./machparam.h ./types.h ./inode.h ./fs.h ./saio.h
confxx.o: ./svsb.h
prf.o: prf.c ./param.h ./machparam.h ./types.h ./svsb.h
sys.o: sys.c ./param.h ./machparam.h ./types.h ./inode.h ./fs.h ./dir.h
sys.o: ./saio.h ./svsb.h
srt0.o: srt0.c
machdep.o: machdep.c
dos.o: dos.c ./dosio.h ./svsb.h
hd.o: hd.c ./param.h ./machparam.h ./types.h ./inode.h ./fs.h ./saio.h ./scio.h
hd.o: ./svsb.h
hp.o: hp.c ./param.h ./machparam.h ./types.h ./inode.h ./fs.h ./saio.h ./scio.h
hp.o: ./svsb.h
mt.o: mt.c ./param.h ./machparam.h ./types.h ./inode.h ./fs.h ./saio.h ./scio.h
mt.o: ./svsb.h ./mtreg.h
scio.o: scio.c ./svsb.h ./scio.h
bcio.o: bcio.c ./svsb.h ./bcio.h
mem.o: mem.c ./param.h ./machparam.h ./types.h ./inode.h ./fs.h ./saio.h
mem.o: ./svsb.h
hdformat.o: hdformat.c ./vollab.h ./svsb.h ./scio.h
hpformat.o: hpformat.c ./svsb.h ./scio.h
hello.o: hello.c
fdisk.o: fdisk.c ./vollab.h ./svsb.h
copy.o: copy.c ./svsb.h
bcxtest.o: bcxtest.c ./svsb.h ./bcio.h
bctest.o: bctest.c ./svsb.h ./bcio.h
bcrtest.o: bcrtest.c ./svsb.h ./bcio.h
bcstest.o: bcstest.c ./svsb.h ./bcio.h
uctest.o: uctest.c ./svsb.h ./ucio.h

# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
###
bcio.o: svsb.h bcio.h
bcrtest.o: svsb.h bcio.h
bcstest.o: svsb.h bcio.h
bctest.o: svsb.h bcio.h
bcxtest.o: svsb.h bcio.h
bcztest.o: svsb.h bcio.h
boot.o: param.h machparam.h types.h dir.h svsb.h
bootxx.o: param.h machparam.h types.h dir.h svsb.h
compare.o: svsb.h
conf.o: param.h machparam.h types.h inode.h fs.h saio.h svsb.h
confxx.o: param.h machparam.h types.h inode.h fs.h saio.h svsb.h
copy.o: svsb.h
dos.o: dosio.h svsb.h
elftest.o: svsb.h ucio.h bcio.h
fdisk.o: vollab.h svsb.h
hd.o: param.h machparam.h types.h inode.h fs.h saio.h scio.h svsb.h
hdformat.o: vollab.h svsb.h scio.h
hp.o: param.h machparam.h types.h inode.h fs.h saio.h scio.h svsb.h
hpformat.o: svsb.h scio.h
ls.o: saio.h
mem.o: param.h machparam.h types.h inode.h fs.h saio.h svsb.h
mt.o: param.h machparam.h types.h inode.h fs.h saio.h scio.h svsb.h mtreg.h
prf.o: param.h machparam.h types.h svsb.h
scio.o: svsb.h scio.h
sys.o: param.h machparam.h types.h inode.h fs.h dir.h saio.h svsb.h
uc0test.o: svsb.h ucio.h
ucio.o: svsb.h ucio.h
uctest.o: svsb.h ucio.h
