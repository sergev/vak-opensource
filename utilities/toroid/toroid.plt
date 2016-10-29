set title "Antenna gain vs bandwidth, band 21 MHz, free space"
set nokey #bottom right

set terminal png monochrome
set output "toroid-eff-bw.png"

#set terminal postscript eps enhanced monochrome "Times-Roman" 24
#set output "toroid-eff-bw.eps"
#set size 2,2

set xlabel "Gain relative to ideal dipole, %"
set ylabel "Bandwidth, kHz"
set logscale y

eff(x) = 100 * exp(x * 0.2302585092994045684)

set label "Dipole"	 at 2+eff(-0.10),	1540
set label "Delta"	 at 2+eff( 0.76),	1372
set label "Quad"	 at 2+eff( 1.09),	1469
#set label "Loop"	 at 2+eff( 1.38),	1640
#set label "Mag.loop 2"	 at -15+eff(-0.87),	225
set label "Mag.loop 1.5" at 2+eff(-0.96),	87
set label "Mag.loop 1.2" at 2+eff(-1.33),	48
set label "Mag.loop 1"	 at 2+eff(-1.9),	30

#set label "Tor 2-12"	 at 2+eff(-0.53),	273	/1.03
#set label "Tor 2-16"	 at 2+eff(-0.49),	291	*1.03
#set label "Tor 1.5-12"	 at -15+eff(-1.03),	143	/1.03
set label "Tor 1.5-16"	 at -15+eff(-0.95),	154     /1.03
set label "Tor 1.5-32"	 at -15+eff(-0.91),	167     *1.03
set label "Tor 1.2-16"	 at -15+eff(-1.37),	96	/1.03
set label "Tor 1.2-32"	 at -15+eff(-1.39),	104	*1.03
#set label "Tor 1-12"	 at 2+eff(-1.88),	59
set label "Tor 1-16"	 at -13+eff(-1.77),	64	/1.03
set label "Tor 1-32"	 at -13+eff(-1.76),	70	*1.03
#set label "Tor 1-64"	 at 2+eff(-2.46),	91

# PNG point types:
# 1 - diamond
# 2 - plus
# 3 - box
# 4 - X
# 5 - triangle
# 6 - star
plot [50:140] [20:2000] '-' using (eff($1)):2 with points pointtype 1 ps 2,\
	'-' using (eff($1)):2 with points pointtype 6 ps 1.5
	-0.10	1540	73	0.9681	# Диполь
	0.76	1372	119	1.0707	# Дельта
	1.09	1469	127	1.058	# Квадрат
#	1.38	1640	138	1.04	# Кольцо
#	-0.87	225	7.4	0	# Магн.рамка 2 м
	-0.96	87	2.4	0	# Магн.рамка 1.5 м
	-1.33	48	1.1	0	# Магн.рамка 1.2 м
	-1.9	30	0.6	0	# Магн.рамка 1 м
end
#	-0.53	273	26.7	1.134	# Тороид 2 м, 12 витков
#	-0.49	291	28.9	1.306	# Тороид 2 м, 16 витков
#	-1.03	143	14.3	1.287	# Тороид 1.5 м, 12 витков
	-0.95	154	15.9	1.375	# Тороид 1.5 м, 16 витков
	-0.91	167	18.9	1.624	# Тороид 1.5 м, 32 витка
	-1.37	96	10.1	1.382	# Тороид 1.2 м, 16 витков
	-1.39	104	12.5	1.665	# Тороид 1.2 м, 32 витка
#	-1.88	59	6.1	1.248	# Тороид 1 м, 12 витков
	-1.77	64	7.1	1.361	# Тороид 1 м, 16 витков
	-1.76	70	9.1	1.661	# Тороид 1 м, 32 витка
#	-2.46	91	12.4	1.992	# Тороид 1 м, 64 витка
end
reset
