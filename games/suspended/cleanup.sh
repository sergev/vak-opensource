#!/bin/sh

sed -i~ '
s/\[[0-9]*d//g
s/\[[0-9]*M//g
s/\[[0-9]*G.//g
s/\[[AC]//g
s/\[0;7m[A-Za-z0-9 ]*//g
s/\[0m//g
s///g
s/\[[0-9]*;[0-9]*H//g
s/\[?[0-9]*l//g
s/
/g
' game.log