\ Draw recursive squares.

95 constant p
100 p - constant q
variable xA variable xB variable xC variable xD
variable yA variable yB variable yC variable yD

: squares
	0 do
		xA @ yA @
		xD @ yD @
		xC @ yC @
		xB @ yB @
		xA @ yA @
		7 grline grline grline grline drop drop drop
		xA @ p * xB @ q * + 100 /
		yA @ p * yB @ q * + 100 /
		xB @ p * xC @ q * + 100 /
		yB @ p * yC @ q * + 100 /
		xC @ p * xD @ q * + 100 /
		yC @ p * yD @ q * + 100 /
		xD @ p * xA @ q * + 100 /
		yD @ p * yA @ q * + 100 /
		yD ! xD ! yC ! xC ! yB ! xB ! yA ! xA !
	loop ;

grinit drop				( maxx maxy
0 yA ! 0 yB ! dup yC ! dup yD ! drop	( maxx
0 xA ! dup xB ! dup xC ! 0 xD ! drop
50 squares
forget p
\ halt
