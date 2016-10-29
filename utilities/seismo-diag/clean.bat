if exist moc_*.cpp del moc_*.cpp
if exist *.o del *.o
if exist *~ del *~
if exist Makefile.Debug del Makefile.Debug
if exist Makefile.Release del Makefile.Release
if exist release\moc_*.cpp del release\moc_*.cpp
if exist release\*.o del release\*.o
if exist debug del /s /q debug
if exist debug rmdir debug
