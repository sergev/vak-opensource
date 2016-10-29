SHELL           = /bin/sh
DEBUGFLAGS      = -O
CFLAGS          = $(DEBUGFLAGS)
CC              = cc -Wall
LIB             = libbtree.a

OBJS            = close.o conv.o debug.o delete.o get.o open.o\
		  overflow.o page.o put.o search.o seq.o split.o\
		  stack.o utils.o mpool.o ndbm.o memmove.o

all:            $(LIB)

$(LIB):         $(OBJS)
		if [ "$?" != "" ]; then ar cr $(LIB) $?; ranlib $(LIB); fi

clean:;         rm -f *.[oba] *~ .,* core a.out

close.o: db.h btree.h mpool.h
conv.o: db.h btree.h mpool.h
debug.o: db.h btree.h mpool.h
delete.o: db.h btree.h mpool.h
get.o: db.h btree.h mpool.h
mpool.o: db.h mpool.h
ndbm.o: db.h ndbm.h btree.h mpool.h
open.o: db.h btree.h mpool.h
overflow.o: db.h btree.h mpool.h
page.o: db.h btree.h mpool.h
put.o: db.h btree.h mpool.h
search.o: db.h btree.h mpool.h
seq.o: db.h btree.h mpool.h
split.o: db.h btree.h mpool.h
stack.o: db.h btree.h mpool.h
utils.o: db.h btree.h mpool.h
