set title 'N-JFET 2N5458'
set xlabel 'Gate-Source Voltage, V'
set y2label 'Drain Current, mA'

set nokey
set xtics out nomirror
set noytics
set y2tics out nomirror
set border 9 front linetype black linewidth 1.0 dashtype solid
set grid
set y2range [0:4]

set style line 1 linecolor rgb '#0060ad' linetype 1 linewidth 1
set style line 2 linecolor rgb '#7f0000' linetype 1 linewidth 1
set style line 3 linecolor rgb '#007f00' linetype 1 linewidth 1

set terminal png enhanced
set output 'result.png'
plot '2n5458.txt' \
    using 1:2 with lines linestyle 1 title 'line1' axes x1y2, \
 '' using 3:4 with lines linestyle 2 title 'line2' axes x1y2, \
 '' using 5:6 with lines linestyle 3 title 'line3' axes x1y2

# set terminal xterm
# replot
