ALL		= libuni.a unicoder
SRCS		= 8859-2.txt 8859-3.txt 8859-4.txt 8859-5.txt\
		  8859-6.txt 8859-7.txt 8859-8.txt 8859-9.txt\
		  8859-14.txt 8859-15.txt\
		  cp037.txt cp10000.txt cp10006.txt cp10007.txt\
		  cp10029.txt cp10079.txt cp10081.txt cp1026.txt\
		  cp1250.txt cp1251.txt cp1252.txt cp1253.txt\
		  cp1254.txt cp1255.txt cp1256.txt cp1257.txt\
		  cp1258.txt cp437.txt cp500.txt cp737.txt\
		  cp775.txt cp850.txt cp852.txt cp855.txt\
		  cp857.txt cp860.txt cp861.txt cp862.txt\
		  cp863.txt cp864.txt cp865.txt cp866.txt\
		  cp869.txt cp874.txt cp875.txt koi8-r.txt\
		  koi8-u.txt mac-arabic.txt mac-centeuro.txt\
		  mac-croatian.txt mac-cyrillic.txt mac-farsi.txt\
		  mac-greek.txt mac-iceland.txt mac-roman.txt\
		  mac-turkish.txt mac-ukraine.txt nextstep.txt\
		  koi7.txt
OBJS		= $(patsubst %.txt,%.o,$(SRCS)) Unicoder.o
CFLAGS		= -O -Wall -I.

all:		$(ALL)

libuni.a:	$(patsubst %.txt,%.h,$(SRCS)) $(OBJS)
		ar r libuni.a $(OBJS)
		ranlib libuni.a

unicoder:	unimain.o libuni.a
		$(CXX) $(LDFLAGS) unimain.o -L. -luni -o unicoder

.SUFFIXES:	.txt

.cpp.o:
		$(CXX) $(CFLAGS) -c $<

.txt.o:
		python mkcode.py $* < $< > $*.cpp
		$(CXX) $(CFLAGS) -c $*.cpp

.txt.h:
		python mkheader.py $* > $*.h

table.h:	mktableh.py
		python mktableh.py $(SRCS) > $*.h

table.cpp:	mktable.py
		python mktable.py $(SRCS) > $*.cpp

clean:
		rm -f *.o *.a *~ unicoder

cleanall:	clean
		rm -f table.h table.cpp 8859-1.cpp $(patsubst %.txt,%.h,$(SRCS)) $(patsubst %.txt,%.cpp,$(SRCS))

Unicoder.o:	Unicoder.h table.h table.cpp
unimain.o:	Unicoder.h
