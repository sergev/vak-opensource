#!/bin/sh
b=\' c=\\ a='echo b=$c$b c=$c$c a=$b$a$b; echo $a'
echo b=$c$b c=$c$c a=$b$a$b; echo $a
