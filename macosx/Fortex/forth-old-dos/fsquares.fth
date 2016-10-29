\ Draw recursive squares.

0.95 fconstant p
1.0 p f- fconstant q
variable xA variable xB variable xC variable xD
variable yA variable yB variable yC variable yD

: squares
	0 do
		xA @ f->i yA @ f->i
		xD @ f->i yD @ f->i
		xC @ f->i yC @ f->i
		xB @ f->i yB @ f->i
		xA @ f->i yA @ f->i
		7 grline grline grline grline drop drop drop	
		xA @ p f* xB @ q f* f+
		yA @ p f* yB @ q f* f+
		xB @ p f* xC @ q f* f+
		yB @ p f* yC @ q f* f+
		xC @ p f* xD @ q f* f+
		yC @ p f* yD @ q f* f+
		xD @ p f* xA @ q f* f+
		yD @ p f* yA @ q f* f+
		yD ! xD ! yC ! xC ! yB ! xB ! yA ! xA !
	loop ;

grinit drop i->f swap i->f		( maxy maxx
0. xA ! dup xB ! dup xC ! 0. xD ! drop	( maxy
0. yA ! 0. yB ! dup yC ! dup yD ! drop
50 squares
getch drop grend forget p
halt
