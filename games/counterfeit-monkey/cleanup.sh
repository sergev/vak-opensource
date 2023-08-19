#!/bin/sh

chmod -x game.log

# Remove \r
sed -i~ '
s///g
' game.log

sed -i~ '
/Glk library error/d
/Run-time problem P17/d
' game.log
