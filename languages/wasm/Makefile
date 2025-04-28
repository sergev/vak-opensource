all:    echo.wasm

clean:
	rm -f *.wasm

%.wasm: %.ts
	asc $< --use abort= -O3 -o $@

run:    echo.wasm
	wasmtime $< foobar йцукенг 12 3 456
