#!/bin/sh

chmod -x game.log

# Remove \r
sed -i~ '
s/
' game.log

# Remove extra newlines
sed -i~ '
/^>/{N;N;s/\n\n/\
/;}
' game.log