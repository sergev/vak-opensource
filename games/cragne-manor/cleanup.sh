#!/bin/sh

chmod -x game.log

# Remove \r
sed -i~ '
s/
' game.log

sed 's/[[:blank:]]*$//' < game.log > walkthrough.txt