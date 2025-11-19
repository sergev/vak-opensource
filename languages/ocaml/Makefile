OC      = ocamlopt
OFLAGS  = -O2

all:    echo unify unify2

clean:
	rm -f echo *.o *.cmi *.cmo *.cmx

echo:   echo.ml
	$(OC) $(OFLAGS) -o $@ $<

unify:  unify.ml
	$(OC) $(OFLAGS) -o $@ $<

unify2: unify2.ml
	$(OC) $(OFLAGS) -o $@ $<
