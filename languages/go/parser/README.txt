Example of yacc-based parser in Go.

Downloaded from: http://stackoverflow.com/questions/8422146/go-how-to-create-a-parser

How to build goyacc tool:

    git clone https://github.com/golang/tools.git
    cd tools/cmd/goyacc
    go build yacc.go
    mv yacc /usr/local/bin/goyacc
