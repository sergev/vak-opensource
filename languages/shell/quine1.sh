#!/bin/sh
z=\' a='z=\\$z a=$z$a$z\; eval echo \$a'; eval echo $a
