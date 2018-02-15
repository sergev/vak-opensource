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
set style line 2 linecolor rgb '#ff0000' linetype 1 linewidth 1
set style line 3 linecolor rgb '#00ff00' linetype 1 linewidth 1
set style line 4 linecolor rgb '#7f007f' linetype 1 linewidth 1
set style line 5 linecolor rgb '#007f7f' linetype 1 linewidth 1
set style line 6 linecolor rgb '#7f7f00' linetype 1 linewidth 1

set terminal png enhanced
set output 'result.png'
plot 'measure1.txt' \
    using ((-5000 - $1)/1000):((11162 - $2)/220) with lines linestyle 1 title 'line1' axes x1y2, \
 '' using ((-5000 - $3)/1000):((11162 - $4)/220) with lines linestyle 2 title 'line2' axes x1y2, \
 '' using ((-5000 - $5)/1000):((11162 - $6)/220) with lines linestyle 3 title 'line3' axes x1y2, \
 '' using ((-5000 - $7)/1000):((11162 - $8)/220) with lines linestyle 4 title 'line4' axes x1y2, \
 '' using ((-5000 - $9)/1000):((11162 - $10)/220) with lines linestyle 5 title 'line5' axes x1y2, \
 '' using ((-5000 - $11)/1000):((11162 - $12)/220) with lines linestyle 6 title 'line6' axes x1y2

# set terminal xterm
# replot
