#!./vtcl

# Basic color: gray on blue
set vcolor 0x70
set fillchar [format "%c" 0x90]
vclear -fill $fillchar
vclear [expr $vlines - 1] 0 -color 0x37

vkey -call quit [vkey -code "^C"]
vkey -call quit [vkey -code "f10"]

# Create the menu.
set Menu [vmenu -create]
vmenu -palette $Menu 0x1b 0x1f 0x17 0x70 0x71

# Create submenus.
set fileMenu [vmenu -add $Menu " ~File "]
set optMenu  [vmenu -add $Menu " ~Options "]
set helpMenu [vmenu -add $Menu " ~Help "]

proc refresh item {
	vdisplay -redraw
	vdisplay
}

proc hello item {
	global vlines vcolumns
	vmessage " Hello " "Hello, World!" "{ Ok } { No } { Are you sure? }" 0x2f 0x1f
}

proc lreply item {
	global vlines vcolumns
	vmessage " Hello " "Hello, World!" "{ Happy to see you! } { It seems to be not very attractive... } { Are you really sure? }" 0x2f 0x1f
}

proc getstring item {
	global vlines vcolumns
	set res [vinput " String " "Enter some string:" 0x2f 0x1f -string 40 "abracadabra"]
	vmessage " Result " "Got: `$res'" "Ok" 0x2f 0x1f
}

proc checknumber num {
	if [string match {[0-9][0-9]*} $num] {
		return 1
	}
	vdisplay -beep
	vmessage " Error " "Invalid number: `$num'" "Again" 0x4f 0x1f
	return 0
}

proc getnumber item {
	global vlines vcolumns
	set res [vinput " String " "Enter some number:" 0x2f 0x1f -string 40 "12345" -verify checknumber]
	vmessage " Result " "Got: `$res'" "Ok" 0x2f 0x1f
}

proc help item {
	set flash [vmessage " Help " "No help available" -flash 0x4f 0x7f]
	vdisplay -delay 400
	vmessage -delete $flash
}

proc quit item {
	global vlines
	vmove [expr $vlines - 1] 0
	vclear - -color 7
	vdisplay
	exit 0
}

# Make up submenus.
vsubmenu -add $fileMenu "~Refresh" refresh
vsubmenu -add $fileMenu "~Quit" quit

vsubmenu -add $optMenu "~Hello" hello
vsubmenu -add $optMenu "~Long Reply Message" lreply
vsubmenu -add $optMenu "~Get String" getstring
vsubmenu -add $optMenu "~Get Number" getnumber

vsubmenu -add $helpMenu "~Help" help

while 1 {
	vmenu -display $Menu 0
	vmenu -run $Menu 0
}
vdisplay
