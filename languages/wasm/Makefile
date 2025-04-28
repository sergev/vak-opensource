all:    echo.wasm

clean:
	rm -f *.wasm

%.wasm: %.ts
	asc $< --debug --use abort= -O -o $@

run:    echo.wasm
	wasmtime $< foobar йцукенг 12 3 456
