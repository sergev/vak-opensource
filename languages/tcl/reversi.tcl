#!/usr/bin/env tclsh
package require Tk 8.5

for {set i 0} {$i<64} {incr i} {lappend icells [expr {$i/8}] [expr {$i%8}]}

array set vs [list 1 2 2 1 cl,1 black cl,2 white pn,1 черные pn,2 белые]

ttk::button .b1 -text "Новая игра" -command {newgame 1 2}
ttk::button .b2 -text "Выход" -command {exit}
ttk::label  .l1 -text "Добро пожаловать в игру Реверси"
canvas .cv -width 479 -height 479
grid rowconfigure . 1 -weight 1
grid columnconfigure . 2 -weight 1
grid .b1 .b2 .l1 -padx 4 -pady 4 -sticky e
grid .cv -padx 4 -pady 4 -columnspan 3

foreach {x y} $icells {
  set cr1 [list [expr {$x*60+2}] [expr {$y*60+2}] [expr {$x*60+60}] [expr {$y*60+60}]]
  set cr2 [list [expr {$x*60+4}] [expr {$y*60+4}] [expr {$x*60+58}] [expr {$y*60+58}]]
  .cv create rectangle $cr1 -fill gray -tag "cell,$x,$y"
  .cv create oval $cr2 -state hidden -tag "piece $x,$y"
  .cv bind cell,$x,$y <1> [list evuser $x $y]}

proc pieceset {x y p}  {
  .cv itemconfigure $x,$y -state normal -fill $::vs(cl,$p)
  incr ::score($p)        [expr {+($::board($x,$y) != $p)}]
  incr ::score($::vs($p)) [expr {-($::board($x,$y) == $::vs($p))}]
  set  ::board($x,$y)     [list $p]}

proc newgame {p1 p2} {
  .cv itemconfigure piece -state hidden
  array set ::score  [list 0 0 1 0 2 0]
  array set ::player [list 1 $p1 2 $p2]
  foreach {x y} $::icells {set ::board($x,$y) 0}
  foreach {x y s} {3 3 2 4 4 2 3 4 1 4 3 1} {pieceset $x $y $s}
  set ::cur 1; waitturn}

proc getflips {x y p} {
  if {$::board($x,$y) != 0} return;
  set result {}
  foreach {ix iy} {0 -1 0 1 -1 0 1 0 -1 -1 1 1 1 -1 -1 1} {
    set temp {}
    for {set i [expr {$x+$ix}]; set j [expr {$y+$iy}]} \
        {[info exists ::board($i,$j)]} {incr i $ix; incr j $iy} {
        switch -- $::board($i,$j) \
          $::vs($p) {lappend temp $i $j} \
          $p        {foreach {m n} $temp {lappend result $m $n}; break} \
          0         {break}
  }}
  return $result}

proc waitturn {} {
  .l1 configure -text "Ходят $::vs(pn,$::cur) ($::score(1):$::score(2))"
  array set v [list $::cur {} $::vs($::cur) {}]
  foreach {x y} $::icells {
    set l [getflips $x $y $::cur]; if {[llength $l]} {lappend v($::cur) [list $x $y]}
    set l [getflips $x $y $::vs($::cur)]; if {[llength $l]} {lappend v($::vs($::cur)) [list $x $y]}}
  if {[llength $v($::cur)] == 0 && [llength $v($::vs($::cur))] == 0} {
    tk_messageBox -title "Reversi" -message "Игра окончена"; return}
  if {$::player($::cur) == 1 && [llength $v($::cur)]} {
    set ::waituser 1; return}
  if {$::player($::cur) == 2 && [llength $v($::cur)]} {
    set ::waituser 0
    set ::flip [lindex $v($::cur) [expr {int([llength $v($::cur)]*rand())}]]
    turn [lindex $::flip 0] [lindex $::flip 1] $::cur}
  set ::cur $::vs($::cur); after idle waitturn}

proc evuser {x y} {
  if {[info exists ::waituser] && $::waituser && [turn $x $y $::cur]} {
    set ::cur $::vs($::cur); after idle waitturn}}

proc turn {x y p} {
  set flips [getflips $x $y $p]
  foreach {i j} $flips  {pieceset $i $j $p}
  if {[llength $flips]} {pieceset $x $y $p; return 1} else {return 0}}
