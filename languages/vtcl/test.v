#!./vtcl

# Basic color: gray on blue
set vcolor 0x17

# The last line
set y1 [expr $vlines - 1]
set x1 [expr $vcolumns - 2]

# The center line
set y2 [expr $vlines / 2]

# The center column
set x2 [expr $vcolumns / 2]

set f1 [format "%c" 0x90]
set f2 [format "%c" 0x91]
set f3 [format "%c" 0x92]

proc handlekey k {
	global y1
	set pos [vmove]
	vputs "Pressed key $k" $y1 0 -color 0x1c
	eval vmove $pos
	vdisplay
}

vkey -call handlekey [vkey -code "f1"]
vkey -call handlekey [vkey -code "f2"]
vkey -call handlekey [vkey -code "f3"]
vkey -call handlekey [vkey -code "f4"]
vkey -call handlekey [vkey -code "f5"]
vkey -call handlekey [vkey -code "f6"]
vkey -call handlekey [vkey -code "f7"]
vkey -call handlekey [vkey -code "f8"]
vkey -call handlekey [vkey -code "f9"]
vkey -call handlekey [vkey -code "f0"]

vdisplay -cursor 0

vclear -fill $f3 -color 0x13
vputs "Press any key..." $y1 $x1 -color 0x1f -right; vdisplay; vkey

vclear 2 4 21 72 -fill $f2 -color 0x13
vputs "Press any key..." $y1 $x1 -color 0x1f -right; vdisplay; vkey

vclear 4 8 17 64 -fill $f1 -color 0x13
vputs "Press any key..." $y1 $x1 -color 0x1f -right; vdisplay; vkey

vputs "Hello, World" [expr $y2 - 2] $x2 -color 0x7f -right
vputs "Hello, World" $y2            $x2 -color 0x7f -center
vputs "Hello, World" [expr $y2 + 2] $x2 -color 0x7f

vput -hline 13 [expr $y2 - 1] [expr $x2 - 11]
vput -special "vert" [expr $y2 - 2] [expr $x2 + 1]
vput -special "lrc"  [expr $y2 - 1] [expr $x2 + 1]

vput -hline 13 [expr $y2 + 1] [expr $x2 - 1]
vput -special "vert" [expr $y2 + 2] [expr $x2 - 1]
vput -special "ulc"  [expr $y2 + 1] [expr $x2 - 1]

vput -vline 15 5 9
vput -vline 13 6 11
vput -vline 11 7 13
vput -vline 15 5 70
vput -vline 13 6 68
vput -vline 11 7 66
vput -frame 9 30 8 25
vputs "Press any key..." $y1 $x1 -color 0x1f -right; vdisplay; vkey

set b [vbox -get 9 30 8 25]
vput -box $b -color 0x6f
vputs "Press any key..." $y1 $x1 -color 0x1f -right; vdisplay; vkey

vclear 8 25 9 30 -fill $f1 -color 0x13
set b1 [vbox -get 9 30 4 17]
vput -box $b 4 17 -color 0x4f
vputs "Press any key..." $y1 $x1 -color 0x1f -right; vdisplay; vkey

vput -box $b1; vbox -delete $b1
set b1 [vbox -get 9 30 12 33]
vput -box $b 12 33 -color 0x2f
vputs "Press any key..." $y1 $x1 -color 0x1f -right; vdisplay; vkey

vput -box $b1; vbox -delete $b1
vput -box $b
vputs "Press any key..." $y1 $x1 -color 0x1f -right; vdisplay; vkey

vbox -delete $b
vmove [expr $vlines - 1] 0
vdisplay
