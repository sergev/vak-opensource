Example of using `nex', a Lex tool for Go language.

Here we create a scanner, which counts lines, words and characters in the input file,
much like `wc' utility.

Sources of NEX: https://github.com/blynn/nex


How to build and run
~~~~~~~~~~~~~~~~~~~~

    $ go get github.com/blynn/nex
    $ $HOME/go/bin/nex wc.nex
    $ go build wc.nn.go
    $ ./wc.nn < /etc/fstab
    Lines: 12
    Words: 88
    Chars: 675
