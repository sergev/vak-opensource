#!/bin/sh

chmod -x game.log

# Remove \r
sed -i~ '
s///g
' game.log

# Remove extra newlines
sed -i~ '
/^>/{N;N;s/\n\n/\
/;}
/Warning:/d
' game.log
