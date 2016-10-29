LIBS    = -ldl -lpthread

all:    tst

tst:    main.o
	$(CC) $(LDFLAGS) $< -o $@ $(LIBS)

clean:
	rm -f tst *.log *.raw *.o
