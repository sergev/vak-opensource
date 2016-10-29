#
# CC Command
#
CC = gcc -std=c99 -g -Wall
CFLAGS = -U__STRICT_ANSI__ -DCYR_CTLN_CTLO -D_GNU_SOURCE
LDFLAGS = -lm

# On system debug, print all memory writes and i/o accesses.
CFLAGS += -DSYS_DEBUG

# Unix Environments
ifneq (,$(findstring solaris,$(OSTYPE)))
    LDFLAGS += -lsocket -lnsl -lrt -lpthread
else
ifeq (,$(findstring darwin,$(OSTYPE)))
    LDFLAGS += -lrt
endif
endif

# Enable network
#NETWORK_OPT = -DUSE_NETWORK -isystem /usr/local/include /usr/local/lib/libpcap.a

#
# Common Libraries
#
BIN = ../demos-dvk/
SIM = scp.o sim_console.o sim_fio.o sim_timer.o sim_sock.o \
	sim_tmxr.o sim_ether.o sim_tape.o

#
# Emulator source files and compile time options
#
PDP11D = .
PDP11 = ${PDP11D}/pdp11_fp.o ${PDP11D}/pdp11_cpu.o ${PDP11D}/pdp11_dz.o \
	${PDP11D}/pdp11_cis.o ${PDP11D}/pdp11_lp.o ${PDP11D}/pdp11_rk.o \
	${PDP11D}/pdp11_rl.o ${PDP11D}/pdp11_rp.o ${PDP11D}/pdp11_rx.o \
	${PDP11D}/pdp11_stddev.o ${PDP11D}/pdp11_sys.o ${PDP11D}/pdp11_tc.o \
	${PDP11D}/pdp11_tm.o ${PDP11D}/pdp11_ts.o ${PDP11D}/pdp11_io.o \
	${PDP11D}/pdp11_rq.o ${PDP11D}/pdp11_tq.o ${PDP11D}/pdp11_pclk.o \
	${PDP11D}/pdp11_ry.o ${PDP11D}/pdp11_pt.o ${PDP11D}/pdp11_hk.o \
	${PDP11D}/pdp11_xq.o ${PDP11D}/pdp11_xu.o ${PDP11D}/pdp11_vh.o \
	${PDP11D}/pdp11_rh.o ${PDP11D}/pdp11_tu.o ${PDP11D}/pdp11_cpumod.o \
	${PDP11D}/pdp11_cr.o ${PDP11D}/pdp11_rf.o ${PDP11D}/pdp11_dl.o \
	${PDP11D}/pdp11_ta.o ${PDP11D}/pdp11_rc.o ${PDP11D}/pdp11_kg.o \
	${PDP11D}/pdp11_ke.o ${PDP11D}/pdp11_dc.o ${PDP11D}/pdp11_io_lib.o \
	${PDP11D}/pdp11_kmd.o ${PDP11D}/pdp11_kgd.o
CFLAGS += -O -DVM_PDP11 -I${PDP11D} ${NETWORK_OPT}

#
# Build everything
#
all : ${BIN}pdp11${EXE}

clean :
ifeq ($(WIN32),)
	${RM} ${BIN}pdp11${EXE} *.o *~ ../demos-dvk/*~
else
	if exist BIN\*.exe del /q BIN\*.exe
endif

#
# Individual builds
#
${BIN}pdp11${EXE} : ${PDP11} ${SIM}
	${CC} ${PDP11} ${SIM} ${PDP11_OPT} -o $@ ${LDFLAGS}

###
pdp11_cis.o: pdp11_cis.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_cpu.o: pdp11_cpu.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h pdp11_cpumod.h
pdp11_cpumod.o: pdp11_cpumod.c pdp11_defs.h sim_defs.h scp.h \
  sim_console.h sim_timer.h sim_fio.h pdp11_io_lib.h pdp11_cpumod.h
pdp11_cr.o: pdp11_cr.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h pdp11_cr_dat.h
pdp11_dc.o: pdp11_dc.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h sim_sock.h sim_tmxr.h
pdp11_dl.o: pdp11_dl.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h sim_sock.h sim_tmxr.h
pdp11_dz.o: pdp11_dz.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h sim_sock.h sim_tmxr.h
pdp11_fp.o: pdp11_fp.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_hk.o: pdp11_hk.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_io.o: pdp11_io.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_io_lib.o: pdp11_io_lib.c pdp11_defs.h sim_defs.h scp.h \
  sim_console.h sim_timer.h sim_fio.h pdp11_io_lib.h sim_sock.h \
  sim_tmxr.h
pdp11_ke.o: pdp11_ke.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_kg.o: pdp11_kg.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_kmd.o: pdp11_kmd.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_kgd.o: pdp11_kmd.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_lp.o: pdp11_lp.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_pclk.o: pdp11_pclk.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_pt.o: pdp11_pt.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_rc.o: pdp11_rc.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_rf.o: pdp11_rf.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_rh.o: pdp11_rh.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_rk.o: pdp11_rk.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_rl.o: pdp11_rl.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_rp.o: pdp11_rp.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_rq.o: pdp11_rq.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h pdp11_uqssp.h pdp11_mscp.h
pdp11_rx.o: pdp11_rx.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_ry.o: pdp11_ry.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_stddev.o: pdp11_stddev.c pdp11_defs.h sim_defs.h scp.h \
  sim_console.h sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_sys.o: pdp11_sys.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_ta.o: pdp11_ta.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h sim_tape.h
pdp11_tc.o: pdp11_tc.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h
pdp11_tm.o: pdp11_tm.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h sim_tape.h
pdp11_tq.o: pdp11_tq.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h pdp11_uqssp.h pdp11_mscp.h \
  sim_tape.h
pdp11_ts.o: pdp11_ts.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h sim_tape.h
pdp11_tu.o: pdp11_tu.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h sim_tape.h
pdp11_vh.o: pdp11_vh.c pdp11_defs.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h pdp11_io_lib.h sim_sock.h sim_tmxr.h
pdp11_xq.o: pdp11_xq.c pdp11_xq.h pdp11_defs.h sim_defs.h scp.h \
  sim_console.h sim_timer.h sim_fio.h pdp11_io_lib.h sim_ether.h \
  pdp11_xq_bootrom.h
pdp11_xu.o: pdp11_xu.c pdp11_xu.h pdp11_defs.h sim_defs.h scp.h \
  sim_console.h sim_timer.h sim_fio.h pdp11_io_lib.h sim_ether.h
scp.o: scp.c sim_defs.h scp.h sim_console.h sim_timer.h sim_fio.h \
  sim_rev.h
sim_console.o: sim_console.c sim_defs.h scp.h sim_console.h sim_timer.h \
  sim_fio.h sim_sock.h sim_tmxr.h
sim_ether.o: sim_ether.c sim_ether.h sim_defs.h scp.h sim_console.h \
  sim_timer.h sim_fio.h sim_sock.h
sim_fio.o: sim_fio.c sim_defs.h scp.h sim_console.h sim_timer.h sim_fio.h
sim_sock.o: sim_sock.c sim_defs.h scp.h sim_console.h sim_timer.h \
  sim_fio.h sim_sock.h
sim_tape.o: sim_tape.c sim_defs.h scp.h sim_console.h sim_timer.h \
  sim_fio.h sim_tape.h
sim_timer.o: sim_timer.c sim_defs.h scp.h sim_console.h sim_timer.h \
  sim_fio.h
sim_tmxr.o: sim_tmxr.c sim_defs.h scp.h sim_console.h sim_timer.h \
  sim_fio.h sim_sock.h sim_tmxr.h
txt2cbn.o: txt2cbn.c pdp11_cr_dat.h
