OBJS		= JUDOIDE.class
JC		= javac
#JC		= /System/Library/Frameworks/JavaVM.framework/Versions/1.4.2/Home/bin/javac
JFLAGS		= -classpath ../.. -encoding UTF-8

all:		$(OBJS)

run:
		cd ../..; java -DLANGUAGE=en_US.UTF-8 org.judo.JUDOIDE

clean:
		rm -f *~ ../../programs/JUDOApp.*

cleanall:	clean
		rm -f *.class

.SUFFIXES:	.java .class

.java.class:
		$(JC) $(JFLAGS) $<
