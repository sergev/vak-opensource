#!/bin/sh

# Replace \r -> \n
sed -i~ '
s//\
/g
' game.log

# Delete updates of the banner line
sed -i~ '
/H/d
/M/d
s/\[[0-9]*d/\
/g
s/\[B//g
s/\[0;10m//g
' game.log
