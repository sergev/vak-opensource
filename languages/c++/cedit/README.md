Example of plain text editor with the C/C++ ncurses library.

Usage:
    cedit [file]

Keys:

 * Printable character or space - insert into text
 * Cursor left, right, up down - move cursor
 * Enter - split the line
 * Delete - erase character under cursor
 * Backspace - erase previous character
 * Tab - insert spaces up to n*4 column
 * Escape - ignore
 * F1 - enter Command mode
 * F2 - save file

Keys in Command mode:

 * Printable character or space - insert into command line
 * Cursor left, right, up down - move cursor
 * Enter - execute command
 * Delete - erase character under cursor in command line
 * Backspace - erase previous character in command line
 * Escape - clear command line
 * Tab, F1, F2 - ignore
