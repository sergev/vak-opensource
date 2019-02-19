#!/usr/bin/env tclsh
package require Tk

foreach {p} {7 2 10 11 5 4 9 12 13 8 3 14 15 1 6} {i} {0 1 2 3 4 5 6 7 8 9 10 11 12 13 14} {
    button .$p -relief solid -bd 1 -width 8 -height 4 -text $p -command "move $p"
    grid   .$p -row [expr {$i/4}] -column [expr {$i%4}] -padx 1 -pady 1}

proc move {b} {
    lassign [list [dict get [grid info .$b] -column] [dict get [grid info .$b] -row]] x y
    foreach {ix iy} {0 -1 0 1 -1 0 1 0} {
        lassign [list [expr {$x+$ix}] [expr {$y+$iy}]] i j
        if {$i>=0 && $i<=3 && $j>=0 && $j<=3 && ![llength [grid slaves . -col $i -row $j]]} {
            grid configure .$b -column $i -row $j}}}
