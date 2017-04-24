all:    main

main:   main.gy lex.go
	goyacc -o main.go $<
	go build main.go lex.go

clean:
	rm -f main.go y.output main
