#!/bin/sh

chmod -x game.log

# Remove \r
sed -i~ '
s///g
' game.log

sed -i~ '
' game.log
