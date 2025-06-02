OC      = ocamlfind ocamlopt
OFLAGS  = -package base -linkpkg -O2

all:    echo

clean:
	rm -f echo *.o *.cmi *.cmo *.cmx

echo:   echo.ml
	$(OC) $(OFLAGS) -o $@ $<
