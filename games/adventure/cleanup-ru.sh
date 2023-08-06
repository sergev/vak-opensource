#!/bin/sh

chmod -x game-ru.log

# Remove \r
sed -i~ '
s///g
' game-ru.log

# Remove extra newlines
sed -i~ '
/^>/{N;N;s/\n\n/\
/;}
' game-ru.log
