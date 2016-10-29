# Written By:	Robert Alan Byer / byer@mail.ourservers.net
# Modified By:	Mark Pizzolato / mark@infocomm.com
#		Norman Lastovica / norman.lastovica@oracle.com
#		Oleg Safiullin / form@pdp-11.nsk.ru
#
# This build script will accept the following build options.
#
#            ALL             Just Build "Everything".
#            PDP11           Just Build The DEC PDP-11.
#            CLEAN           Will Clean Files Back To Base Kit.
#
# To build with debugging enabled (which will also enable traceback 
# information) use..
#
#        MMK/MACRO=(DEBUG=1)
#
# This will produce an executable named {Simulator}-{I64|VAX|AXP}-DBG.EXE
#

# Let's See If We Are Going To Build With DEBUG Enabled.  Always compile
# /DEBUG so that the traceback and debug information is always available
# in the object files.

CC_DEBUG = /DEBUG

.IFDEF DEBUG
LINK_DEBUG = /DEBUG/TRACEBACK
CC_OPTIMIZE = /NOOPTIMIZE

.IFDEF MMSALPHA
ALPHA_OR_IA64 = 1
CC_FLAGS = /PREF=ALL
ARCH = AXP-DBG
CC_DEFS = "_LARGEFILE"
.ENDIF

.IFDEF MMSIA64
ALPHA_OR_IA64 = 1
CC_FLAGS = /PREF=ALL
ARCH = I64-DBG
CC_DEFS = "_LARGEFILE"
.ENDIF

.IFDEF MMSVAX
ALPHA_OR_IA64 = 0
CC_FLAGS = $(CC_FLAGS)
ARCH = VAX-DBG
CC_DEFS = "__VAX"
.ENDIF

.ELSE
LINK_DEBUG = /NODEBUG/NOTRACEBACK

.IFDEF MMSALPHA
ALPHA_OR_IA64 = 1
CC_OPTIMIZE = /OPT=(LEV=5)/ARCH=HOST
CC_FLAGS = /PREF=ALL
ARCH = AXP
CC_DEFS = "_LARGEFILE"
LINK_SECTION_BINDING = /SECTION_BINDING
.ENDIF

.IFDEF MMSIA64
ALPHA_OR_IA64 = 1
CC_OPTIMIZE = /OPT=(LEV=5)
CC_FLAGS = /PREF=ALL
ARCH = I64
CC_DEFS = "_LARGEFILE"
.ENDIF

.IFDEF MMSVAX
ALPHA_OR_IA64 = 0
CC_OPTIMIZE = /OPTIMIZE
CC_FLAGS = $(CC_FLAGS)
ARCH = VAX
CC_DEFS = "__VAX"
.ENDIF

.ENDIF

# Define Our Compiler Flags & Define The Compile Command
OUR_CC_FLAGS = $(CC_FLAGS)$(CC_DEBUG)$(CC_OPTIMIZE) \
	/NEST=PRIMARY/NAME=(AS_IS,SHORT)
CC = CC/DECC$(OUR_CC_FLAGS)

# Define The BIN Directory Where The Executables Will Go.
# Define Our Library Directory.
# Define The platform specific Build Directory Where The Objects Will Go.
#
BIN_DIR = SYS$DISK:[.BIN]
LIB_DIR = SYS$DISK:[.LIB]
BLD_DIR = SYS$DISK:[.LIB.BLD-$(ARCH)]

# Check To Make Sure We Have SYS$DISK:[.BIN] & SYS$DISK:[.LIB] Directory.
#
.FIRST
  @ IF (F$SEARCH("SYS$DISK:[]BIN.DIR").EQS."") THEN CREATE/DIRECTORY $(BIN_DIR)
  @ IF (F$SEARCH("SYS$DISK:[]LIB.DIR").EQS."") THEN CREATE/DIRECTORY $(LIB_DIR)
  @ IF (F$SEARCH("SYS$DISK:[.LIB]BLD-$(ARCH).DIR").EQS."") THEN CREATE/DIRECTORY $(BLD_DIR)
  @ IF (F$SEARCH("$(BLD_DIR)*.*").NES."") THEN DELETE/NOLOG/NOCONFIRM $(BLD_DIR)*.*;*
  @ IF "".NES."''CC'" THEN DELETE/SYMBOL/GLOBAL CC

# Core SIMH File Definitions.
#
SIMH_DIR = SYS$DISK:[]
SIMH_LIB = $(LIB_DIR)SIMH-$(ARCH).OLB
SIMH_SOURCE = $(SIMH_DIR)SIM_CONSOLE.C,$(SIMH_DIR)SIM_SOCK.C,\
              $(SIMH_DIR)SIM_TMXR.C,$(SIMH_DIR)SIM_ETHER.C,\
              $(SIMH_DIR)SIM_TAPE.C,$(SIMH_DIR)SIM_FIO.C,\
              $(SIMH_DIR)SIM_TIMER.C

# VMS PCAP File Definitions.
#
PCAP_DIR = SYS$DISK:[.PCAP-VMS.PCAP-VCI]
PCAP_LIB = $(LIB_DIR)PCAP-$(ARCH).OLB
PCAP_SOURCE = \
	$(PCAP_DIR)PCAPVCI.C,$(PCAP_DIR)VCMUTIL.C,\
	$(PCAP_DIR)BPF_DUMP.C,$(PCAP_DIR)BPF_FILTER.C,\
	$(PCAP_DIR)BPF_IMAGE.C,$(PCAP_DIR)ETHERENT.C,\
	$(PCAP_DIR)FAD-GIFC.C,$(PCAP_DIR)GENCODE.C,\
	$(PCAP_DIR)GRAMMAR.C,$(PCAP_DIR)INET.C,\
	$(PCAP_DIR)NAMETOADDR.C,$(PCAP_DIR)OPTIMIZE.C,\
	$(PCAP_DIR)PCAP.C,$(PCAP_DIR)SAVEFILE.C,\
	$(PCAP_DIR)SCANNER.C,$(PCAP_DIR)SNPRINTF.C,\
	$(PCAP_DIR)PCAP-VMS.C
PCAP_VCMDIR = SYS$DISK:[.PCAP-VMS.PCAPVCM]
PCAP_VCM_SOURCES = $(PCAP_VCMDIR)PCAPVCM.C,$(PCAP_VCMDIR)PCAPVCM_INIT.MAR,\
		   $(PCAP_VCMDIR)VCI_JACKET.MAR,$(PCAP_VCMDIR)VCMUTIL.C
PCAP_VCI = SYS$COMMON:[SYS$LDR]PCAPVCM.EXE

# PCAP is not available on OpenVMS VAX or IA64 right now
#
#.IFDEF MMSALPHA
#PCAP_EXECLET = $(PCAP_VCI)
#PCAP_INC = ,$(PCAP_DIR)
#PCAP_LIBD = $(PCAP_LIB)
#PCAP_LIBR = ,$(PCAP_LIB)/LIB/SYSEXE
#PCAP_DEFS = ,"USE_NETWORK=1"
#PCAP_SIMH_INC = /INCL=($(PCAP_DIR))
#.ENDIF

#
# Digital Equipment PDP-11 Simulator Definitions.
#
PDP11_DIR = SYS$DISK:[]
PDP11_LIB = $(LIB_DIR)PDP11-$(ARCH).OLB
PDP11_SOURCE = $(PDP11_DIR)PDP11_FP.C, $(PDP11_DIR)PDP11_CPU.C, \
	$(PDP11_DIR)PDP11_DZ.C, $(PDP11_DIR)PDP11_CIS.C, \
	$(PDP11_DIR)PDP11_LP.C, $(PDP11_DIR)PDP11_RK.C, \
	$(PDP11_DIR)PDP11_RL.C, $(PDP11_DIR)PDP11_RP.C, \
	$(PDP11_DIR)PDP11_RX.C, $(PDP11_DIR)PDP11_STDDEV.C, \
	$(PDP11_DIR)PDP11_SYS.C, $(PDP11_DIR)PDP11_TC.C, \
	$(PDP11_DIR)PDP11_TM.C, $(PDP11_DIR)PDP11_TS.C, \
	$(PDP11_DIR)PDP11_IO.C, $(PDP11_DIR)PDP11_RQ.C, \
	$(PDP11_DIR)PDP11_TQ.C, $(PDP11_DIR)PDP11_PCLK.C, \
	$(PDP11_DIR)PDP11_RY.C, $(PDP11_DIR)PDP11_PT.C, \
	$(PDP11_DIR)PDP11_HK.C, $(PDP11_DIR)PDP11_XQ.C, \
	$(PDP11_DIR)PDP11_XU.C, $(PDP11_DIR)PDP11_VH.C, \
	$(PDP11_DIR)PDP11_RH.C, $(PDP11_DIR)PDP11_TU.C, \
	$(PDP11_DIR)PDP11_CPUMOD.C, $(PDP11_DIR)PDP11_CR.C, \
	$(PDP11_DIR)PDP11_RF.C, $(PDP11_DIR)PDP11_DL.C, \
	$(PDP11_DIR)PDP11_TA.C, $(PDP11_DIR)PDP11_RC.C, \
	$(PDP11_DIR)PDP11_KG.C, $(PDP11_DIR)PDP11_KE.C, \
	$(PDP11_DIR)PDP11_DC.C, $(PDP11_DIR)PDP11_IO_LIB.C, \
	$(PDP11_DIR)PDP11_KMD.C, $(PDP11_DIR)PDP11_KGD.C
PDP11_OPTIONS = /INCL=($(SIMH_DIR),$(PDP11_DIR)$(PCAP_INC))\
		/DEF=($(CC_DEFS),"VM_PDP11=1","CYR_CTLN_CTLO=1")

ALL :	PDP11

CLEAN : 
	$!
	$! Clean out all targets and building Remnants
	$!
	$ IF (F$SEARCH("$(BIN_DIR)*.EXE;*").NES."") THEN -
	     DELETE/NOLOG/NOCONFIRM $(BIN_DIR)*.EXE;*
	$ IF (F$SEARCH("$(LIB_DIR)*.OLB;*").NES."") THEN -
	     DELETE/NOLOG/NOCONFIRM $(LIB_DIR)*.OLB;*
	$ IF (F$SEARCH("SYS$DISK:[...]*.OBJ;*").NES."") THEN -
	     DELETE/NOLOG/NOCONFIRM SYS$DISK:[...]*.OBJ;*
	$ IF (F$SEARCH("SYS$DISK:[...]*.LIS;*").NES."") THEN -
	     DELETE/NOLOG/NOCONFIRM SYS$DISK:[...]*.LIS;*
	$ IF (F$SEARCH("SYS$DISK:[...]*.MAP;*").NES."") THEN -
	     DELETE/NOLOG/NOCONFIRM SYS$DISK:[...]*.MAP;*

#
# Build The Libraries.
#
$(SIMH_LIB) : $(SIMH_SOURCE)
                $!
		$! Building The $(SIMH_LIB) Library.
                $!
                $ $(CC)/DEF=($(CC_DEFS)$(PCAP_DEFS))$(PCAP_SIMH_INC) -
    			/OBJ=$(BLD_DIR) $(MMS$CHANGED_LIST)
                $ IF (F$SEARCH("$(MMS$TARGET)").EQS."") THEN -
                        LIBRARY/CREATE $(MMS$TARGET)
                $ LIBRARY/REPLACE $(MMS$TARGET) $(BLD_DIR)*.OBJ
                $ DELETE/NOLOG/NOCONFIRM $(BLD_DIR)*.OBJ;*

$(PDP11_LIB) : $(PDP11_SOURCE)
                $!
		$! Building The $(PDP11_LIB) Library.
                $!
                $(CC)$(PDP11_OPTIONS) -
    			/OBJ=$(BLD_DIR) $(MMS$CHANGED_LIST)
                $ IF (F$SEARCH("$(MMS$TARGET)").EQS."") THEN -
                        LIBRARY/CREATE $(MMS$TARGET)
                $ LIBRARY/REPLACE $(MMS$TARGET) $(BLD_DIR)*.OBJ
                $ DELETE/NOLOG/NOCONFIRM $(BLD_DIR)*.OBJ;*

$(PCAP_LIB) : $(PCAP_SOURCE)
                $!
		$! Building The $(PCAP_LIB) Library.
                $!
		$ SET DEFAULT $(PCAP_DIR)
                $ @VMS_PCAP $(DEBUG)
		$ SET DEFAULT [--]
                $ IF (F$SEARCH("$(PCAP_LIB)").NES."") THEN -
                        DELETE $(PCAP_LIB);
                $ COPY $(PCAP_DIR)PCAP.OLB $(PCAP_LIB)
                $ DELETE/NOLOG/NOCONFIRM $(PCAP_DIR)*.OBJ;*,$(PCAP_DIR)*.OLB;*
			     
PDP11 : $(SIMH_LIB) $(PCAP_LIBD) $(PDP11_LIB) $(PCAP_EXECLET)
        $!
        $! Building The $(BIN_DIR)PDP11-$(ARCH).EXE Simulator.
        $!
        $ $(CC)$(PDP11_OPTIONS)/OBJ=$(BLD_DIR) SCP.C
        $ LINK $(LINK_DEBUG)/EXE=$(BIN_DIR)PDP11-$(ARCH).EXE -
               $(BLD_DIR)SCP.OBJ,$(PDP11_LIB)/LIBRARY,$(SIMH_LIB)/LIBRARY$(PCAP_LIBR)
        $ DELETE/NOLOG/NOCONFIRM $(BLD_DIR)*.OBJ;*

#
# PCAP VCI Components
#
$(PCAP_VCI) : $(PCAP_VCMDIR)PCAPVCM.EXE
              $!
              $! Installing the PCAP VCI Execlet in SYS$LOADABLE_IMAGES
              $!
              $ COPY $(PCAP_VCMDIR)PCAPVCM.EXE SYS$COMMON:[SYS$LDR]PCAPVCM.EXE 

$(PCAP_VCMDIR)PCAPVCM.EXE : $(PCAP_VCM_SOURCES) 
                            $!
                            $! Building The PCAP VCI Execlet
                            $!
                            $ @SYS$DISK:[.PCAP-VMS.PCAPVCM]BUILD_PCAPVCM
                            $ DELETE/NOLOG/NOCONFIRM $(PCAP_VCMDIR)*.OBJ;*,$(PCAP_VCMDIR)*.MAP;*
