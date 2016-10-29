set title "Code size of rot13 hash function, GCC"
set nokey #bottom right

set terminal png #monochrome
set output "codesize.png"

set ylabel "Commands in loop"
set xlabel "Total commands"

#eff(x) = 100 * exp(x * 0.2302585092994045684)

#set label "Dipole"	 at 2+eff(-0.10),	1540
#set label "Delta"	 at 2+eff( 0.76),	1372
#set label "Quad"	 at 2+eff( 1.09),	1469
#set label "Loop"	 at 2+eff( 1.38),	1640
#set label "Mag.loop 2"	 at -15+eff(-0.87),	225
#set label "Mag.loop 1.5" at 2+eff(-0.96),	87
#set label "Mag.loop 1.2" at 2+eff(-1.33),	48
#set label "Mag.loop 1"	 at 2+eff(-1.9),	30

#set label "Tor 2-12"	 at 2+eff(-0.53),	273	/1.03
#set label "Tor 2-16"	 at 2+eff(-0.49),	291	*1.03
#set label "Tor 1.5-12"	 at -15+eff(-1.03),	143	/1.03
#set label "Tor 1.5-16"	 at -15+eff(-0.95),	154     /1.03
#set label "Tor 1.5-32"	 at -15+eff(-0.91),	167     *1.03
#set label "Tor 1.2-16"	 at -15+eff(-1.37),	96	/1.03
#set label "Tor 1.2-32"	 at -15+eff(-1.39),	104	*1.03
#set label "Tor 1-12"	 at 2+eff(-1.88),	59
#set label "Tor 1-16"	 at -13+eff(-1.77),	64	/1.03
#set label "Tor 1-32"	 at -13+eff(-1.76),	70	*1.03
#set label "Tor 1-64"	 at 2+eff(-2.46),	91

# PNG point types:
# 1 - diamond
# 2 - plus
# 3 - box
# 4 - X
# 5 - triangle
# 6 - star
#plot [0:26] [0:12] 'data.txt' using 2:3 with points pointtype 1 ps 2

set style data boxes
plot 'data.txt' using 2:3 with boxes
reset
