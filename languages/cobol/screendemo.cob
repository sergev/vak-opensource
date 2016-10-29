       >>SOURCE FREE
*> This program is one example of the use of the Screen Section.
*> tectonics: cobc -x screendemo.cbl
identification division.
program-id. screendemo.
data division.
working-storage section.
78  black                       value 0.
78  blue                        value 1.
78  green                       value 2.
78  cyan                        value 3.
78  red                         value 4.
78  magenta                     value 5.
78  yellow                      value 6.
78  white                       value 7.

01  test-record.
    05 last-name pic x(10).
    05 first-name pic x(10).
    05 soc-sec-no pic x(9). 
    05 comment pic x(25).

01  a-message pic x(35) value spaces.
01  field-1-color pic 9.

01 hek pic x value spaces.

screen section.
01 blank-screen.
   05 filler line 1 blank screen background-color white.

01 entry-screen.
    05 blank screen background-color white.
    05 filler line 1 column 5
        value "Enter Last Name".
    05 screen-last-name pic x(10) using last-name
        line 2 column 5
        foreground-color field-1-color.

    05 filler line 3 column 5
        value "Enter First Name".
    05 screen-first-name pic x(10) using first-name
        line 4 column 5
        foreground-color blue
        HIGHLIGHT.

    05 filler line 5 column 5
        value "Enter Social Sec".
    05 screen-soc-sec-no pic x(9) using soc-sec-no
        line 6 column 5
        foreground-color red
        REVERSE-VIDEO SECURE.

    05 filler line 7 column 5
        value "Enter Comment".
    05 screen-comment pic x(25)  using comment
        line 8 column 5
        foreground-color yellow
        HIGHLIGHT.

    05 screen-message pic x(80) from a-message
        line 10 column 5
        foreground-color white.

01 exit-screen.
   05 filler line 1 blank screen background-color yellow.
   05 filler line 2 column 5 value "You entered:".
   05 filler line 4 column 5 value "  Last name:".
   05 exit-last-name pic x(10) from last-name
       line 4 column 18.
   05 exit-first-name pic x(10) from first-name
       line 5 column 18.
   05 exit-soc pic x(9) from soc-sec-no
       line 6 column 18.
   05 exit-comment pic x(25) from comment
       line 7 column 18.
   05 filler line 24 column 1 value "hit enter".
   05 exit-hek pic x using hek line 24 column 40.
	   
procedure division.
    display blank-screen.
    move magenta to field-1-color.

screen-loop.
    display entry-screen.
    accept entry-screen.
    if last-name(1:1) not equal "q"
        move green  to field-1-color
        move "Last name must begin with q to exit" to a-message
        display entry-screen
        go to screen-loop 
    end-if.

    display exit-screen.
    accept exit-screen.

    stop run.
