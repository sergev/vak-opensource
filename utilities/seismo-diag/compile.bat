set QTDIR=C:\Qt\2009.05\qt
set PATH=C:\Qt\2009.05\qt\bin
set PATH=%PATH%;C:\Qt\2009.05\bin;C:\Qt\2009.05\mingw\bin
set PATH=%PATH%;%SystemRoot%\System32
set QMAKESPEC=win32-g++

qmake

mingw32-make -f Makefile
