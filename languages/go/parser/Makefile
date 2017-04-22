all:    main

main:   main.y
	goyacc -o main.go $<
	go build main.go

clean:
	rm -f main.go y.output main
