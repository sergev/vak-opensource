#!/bin/sh

chmod -x game.log

# Remove \r
sed -i~ '
s/
' game.log

sed -i~ '
' game.log