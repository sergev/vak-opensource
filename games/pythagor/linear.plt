set size square
set nokey

set terminal png #monochrome
set output "pythagor-linear.png"
set size 0.9,1.2

#set terminal aqua

plot 'pyth.txt' using 2:3 with points pointtype 3 ps 0.1
reset
