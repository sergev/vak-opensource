all:    main

main:   main.gy
	goyacc -o main.go $<
	go build main.go

clean:
	rm -f main.go y.output main
