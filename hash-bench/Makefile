CC		= gcc -Wall -g
CFLAGS		= -O
OBJS		= hash-bench.o GeneralHashFunctions.o
#INPUT		= usdict
INPUT		= symbols

all:		hash-bench

hash-bench:	$(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) -o $@

bench:		hash-bench
		@wc -l $(INPUT)
		for i in rs js pjw elf bkdr sdbm djb dek ap; do \
			echo -n $$i " "; \
			hash-bench --$$i $(INPUT) | sort +1 | uniq -d -f1 | wc -l; \
		done | tee result

clean:
		rm -f *.o *~ hash-bench

rs:		hash-bench
		hash-bench --$@ $(INPUT) | sort +1 | uniq -d -f1

js:		hash-bench
		hash-bench --$@ $(INPUT) | sort +1 | uniq -d -f1

pjw:		hash-bench
		hash-bench --$@ $(INPUT) | sort +1 | uniq -d -f1

elf:		hash-bench
		hash-bench --$@ $(INPUT) | sort +1 | uniq -d -f1

bkdr:		hash-bench
		hash-bench --$@ $(INPUT) | sort +1 | uniq -d -f1

sdbm:		hash-bench
		hash-bench --$@ $(INPUT) | sort +1 | uniq -d -f1

djb:		hash-bench
		hash-bench --$@ $(INPUT) | sort +1 | uniq -d -f1

dek:		hash-bench
		hash-bench --$@ $(INPUT) | sort +1 | uniq -d -f1

ap:		hash-bench
		hash-bench --$@ $(INPUT) | sort +1 | uniq -d -f1
