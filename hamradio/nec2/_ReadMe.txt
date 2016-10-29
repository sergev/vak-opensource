The Nec2dXS*.exe programs are available as two different
packages.

1) The Nec2dXS_VM.zip package using the MinGW port

This package contains nec2dXS executables capable of using
virtual Memory when on-board RAM is not enough to run the
number of segments as indicated by the executable name. On
most systems all exe's 500 to 11k will run. However when
there is 64Mb or less available it is possible the 11k or
even the 8k executable wil not run due to unsufficient 
memory to start the program.

the _VM package was compiled using the MinGW port for the 
G77, version 3.2 compiler. The automatic virtual memory use
is realized by using standard windows dynamic loadable
libraries (DLL's) thus resulting in somewhat less perfor-
mance and smaller executable size. (appr. 333 Kb) 

2) The Nec2dXS_FB.zip package using the DJGPP port

Note: latest observations (feb-2006) show that due to the
explicit DOS nature of the resulting executables, the DJGPP
version does not always run on latest Windows-XP systems.

This package contains nec2dXS executables optimized for 
SPEED, especially for lower segment counts. These exe's
however are not (directly) capable of running Virtual memory
when physical (on board) RAM is insufficient. In this case,
the message 'Load error, no DPMI memory' is displayed.
The speed increase is especially appreciated when running
the 4nec2 optimizer.

The _FB package was compiled using the DJGPP port for the 
G77, version 3.2 compiler. The speed increase is realized
by using special internal DJGPP static libraries, resulting
in a larger executable file size. (appr 418 Kb)

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Next I want to thank Raymond Anderson for supplying me with
initial hints and tips concerning the Gnu77 fortran compiler.

The original nec-2 sources as created by G. Burke and A. 
Poggio of were modified so that the SomNec ground calcula-
tions are integrated in the nec-2 engine. 
This means that no som2d.nec file is generated any more and 
the required data is stored internally.

Because of the performance of most modern computers, the
SomNec ground calculations do not take much time any more
(typically less than 1 sec). So there is no special need
for re-using the pre-generated SomNec data.

Furthermore it is now possible to recalculate the SomNec data
for each step in the Frequency-loop/sweep. This was not
possible with earlier versions of the nec2d(x) engine.

Note however that when requesting a sweep for a large number
of steps, each step requires about 1 sec of additional ground
calculation. When using for instance a sweep from 3 to 30 Mhz
with 0.1 Mhz increment this will require 27*10*1 = 270 seconds
(4.5 minutes) of additional calculation time.

It is still possible to use the 'negative conductivity' trick.
If specified the SomNec calculation is only done once, prior
to running the frequency-loop. This will decrease the precision
back to the original nec2d precision, but will speed-up the
calculation time.

For further changes on nec2dXS see the _Builds.txt file.

For further compilation hints see the _Compile.txt file. 

Arie.		  