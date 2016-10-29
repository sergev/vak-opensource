#
# Win32 Environments
#
vpath %.c ..
CC = i586-mingw32msvc-gcc -std=c99 -g
CFLAGS = -U__STRICT_ANSI__ -D_WIN32 -DCYR_CTLN_CTLO
LDFLAGS = -lm -lwsock32 -lwinmm
EXE = .exe

# Add network support
#CFLAGS += -DUSE_NETWORK
#LDFLAGS += -lwpcap -lpacket

#
# Common Libraries
#
BIN = ../../demos-dvk/
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
CFLAGS += -O -DVM_PDP11 -I${PDP11D}

#
# Build everything
#
all : ${BIN}pdp11${EXE}

clean :
	${RM} *.o

#
# Individual builds
#
${BIN}pdp11${EXE} : ${PDP11} ${SIM}
	${CC} ${PDP11} ${SIM} ${PDP11_OPT} -o $@ ${LDFLAGS}

###
