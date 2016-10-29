/***************************************************************************
 * JUDO - A Java IDE for Children and Beginning Programmers
 * Copyright (C) 2001  Thomas J. Dunn
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ***************************************************************************/

package org.judo;

/**
 * English localization
 */
public class JUDO_en extends JUDO_localization {
  public JUDO_en() {
    IDE_DEFAULT_PROG_NAME = "Untitled";
    IDE_SAVE_PROG = "Save Program";
    IDE_ERR_INIT_JAVA = "Error Initializing Java, please read README.txt that came with JUDO";

    IDE_FILE = "File";
    IDE_FILE_NEW = "New";
    IDE_FILE_SAVE = "Save";
    IDE_FILE_SAVE_NEW_NAME = "Save with New Name...";
    IDE_FILE_SAVE_FLOPPY = "Save to Floppy";
    IDE_FILE_SAVE_FLOPPY_NEW_NAME = "Save to Floppy with New Name...";
    IDE_FILE_OPEN = "Open Program...";
    IDE_FILE_OPEN_FLOPPY = "Open Program From Floppy...";
    IDE_FILE_DELETE = "Delete Program...";
    IDE_FILE_DELETE_FLOPPY = "Delete Program From Floppy...";
    IDE_FILE_EXIT = "Exit";

    IDE_EDIT = "Edit";
    IDE_EDIT_COPY = "Copy";
    IDE_EDIT_CUT = "Cut";
    IDE_EDIT_PASTE = "Paste";
    IDE_EDIT_UNDO = "Undo";
    IDE_EDIT_GOTO_LINE = "Goto Line...";

    IDE_PROG = "Program";
    IDE_PROG_PROP = "Program Properties...";

    IDE_RUN = "Run";
    IDE_RUN_RUN = "Run";
    IDE_RUN_COMPILE = "Compile";

    IDE_HELP = "Help";
    IDE_HELP_JUDO_HELP = "JUDO Help";
    IDE_HELP_TUTORIALS = "Tutorials";
    IDE_HELP_FUNC_REF = "JUDO Function Reference";
    IDE_HELP_COLOR_REF = "Color Values Reference";
    IDE_HELP_ABOUT_JUDO = "About JUDO";
    IDE_HELP_JUDO_WEBSITE = "JUDO's Website";

    IDE_BUTTON_RUN = "Run";

    IDE_COMPILE_ERR = "Internal Compilation Error (javac interrupted)";
    IDE_404_ERR = "Error: File %s was not found.";
    IDE_READ_FILE_ERR = "Error: Reading file %s.";
    IDE_CREATE_FILE_ERR = "Error: Cannot create file %s.";
    IDE_EXIT_JUDO_NO_SAVE_MSG = "Do you want to exit JUDO without Saving this Program?";
    IDE_EXIT_NO_SAVE_TIT = "Exit without saving?";
    IDE_SAVE_NEW_NAME_TIT = "Save with New Name";
    IDE_SAVE_TIT = "Save";
    IDE_PROG_EXISTS_MSG = "Program \"%s\" already exists.  Replace it?";
    IDE_PROG_EXISTS_TIT = "Program already exists";
    IDE_NO_FLOPPY_MSG = "There does not seem to be a Floppy disk in\nthe Floppy drive.  If there is, try ejecting\nit, putting it back in, and saving again.";
    IDE_FLOPPY_ERR_TIT = "Error saving to Floppy Disk";
    IDE_FLOPPY_WRITE_PROT_MSG = "Check that the Floppy Disk is not write protected.\nLook at the back of the disk.  In the top-left corner,\nthere is a little moveable tab.  Push it in the down\nposition if it is not there already and try to save again.";
    IDE_NO_PROG_DIR_MSG = "Program Directory did not exist and its re-creation failed.";
    IDE_ERR_SAVING_TIT = "Error saving";
    IDE_PROG_DIR_NOT_WRIT = "Error saving to Program Directory.  It is not writeable.";
    IDE_OPEN_NO_SAVE_MSG = "Do you want to Open a Program without\nSaving your changes to this one?";
    IDE_OPEN_NO_SAVE_TIT = "Open Without Saving?";
    IDE_OPEN_NO_FLOPPY_MSG = "There does not seem to be a Floppy disk in\nthe Floppy drive.  If there is, try ejecting\nit, putting it back in, and opening again.";
    IDE_OPEN_ERR_TIT = "Error opening from Floppy Disk";
    IDE_NO_PROG_MSG = "There are currently no Programs to Open";
    IDE_NO_PROG_TIT = "No Programs to Open";
    IDE_OPEN_TIT = "Open Program";
    IDE_OPEN_PROG_MSG = "There was an error opening the Program \"%s\"";
    IDE_OPEN_PROG_ERR_TIT = "Error opening Program";
    IDE_NO_PROG_DEL_MSG = "There are currently no Programs to Delete";
    IDE_NO_PROG_DEL_TIT = "No Programs to Delete";
    IDE_DEL_NO_FLOPPY_MSG = "There does not seem to be a Floppy disk in\nthe Floppy drive.  If there is, try ejecting\nit, putting it back in, and deleting again.";
    IDE_DEL_ERR_TIT = "Error deleting from Floppy Disk";
    IDE_DEL_TIT = "Delete Program";
    IDE_NEW_WITHOUT_SAVE_MSG = "Do you want to start a New Program without\nSaving your changes to this one?";
    IDE_NEW_WITHOUT_SAVE_TIT = "New Program Without Saving?";
    IDE_GOTO_LINE_NUM_MSG = "Goto Line Number";
    IDE_GOTO_LINE_NUM_TIT = "Goto Line Number";
    IDE_LINE_NUM_GREATER_MSG = "Line number must be greater than or equal to 1";
    IDE_INV_LINE_NUM_TIT = "Invalid Line Number";
    IDE_LINE_COUNT_MSG = "There are only %s lines in the Program";
    IDE_NO_LINE_NUM_MSG = "You did not enter a line number";
    IDE_LINE_NUM_INV_MSG = "Line number %s is not a valid line number.";

    IDE_VAR_VAL_DEC_ASS_TIT = "Variables, Values, Declarations, and Assignments";
    IDE_JUDO_ABOUT_MSG = "JUDO - A Java IDE for Beginning Programmers\nhttp://judo.sourceforge.net\nVersion: %s\n\nJUDO, Copyright (C) 2001-2003 Thomas J. Dunn";

    CER_STRING_MISSING_QUOTE = "String missing a \"";
    CER_EMPTY_CHAR = "Empty character%sDid you mean to use a double-quote character and not two single-quote characters?";
    CER_UNCLOSED_CHAR = "Character Error%sDid you mean to use a double-quote character and not two single-quote characters?";
    CER_INVALID_STATEMENT = "Not a valid Java statement";
    CER_CURLY_BRACE_NO_MATCH = "Curly braces { } not matching?";
    CER_NO_VOID_MAIN = "Error: missing 'void main()'";
    CER_ERR_ON_LINE = "Error on line ";
    CER_COMP_SUCCESS = "Compilation successful.";
    CER_UNDEF_VAR = "Undefined variable ";
    CER_UNDEF_FUNC = "Undefined function ";
    CER_UNDEF_SYMBOL = "Undefined symbol ";

    APP_PROG_COMPLETE = "Program Complete";

    DEL_PROG_TO_DELETE = "Choose a Program to Delete";
    DEL_OK = "Delete";
    DEL_CANCEL = "Cancel";
    DEL_CONFIRM_TIT = "Delete Program?";
    DEL_CONFIRM_MSG = "Are you sure you want to delete program '%s'?";

    OPN_PROG_TO_OPEN = "Choose a Program to Open";
    OPN_OK = "OK";
    OPN_CANCEL = "Cancel";

    PRP_CHOOSE_PROG_PROP = "Choose the Properties for this Program";
    PRP_GRAPHICS = "Graphics";
    PRP_TEXT = "Text";
    PRP_TEXT_AND_GRAPHICS = "Text & Graphics";
    PRP_WIN_STYLE = "Window Style";
    PRP_OK = "OK";
    PRP_CANCEL = "Cancel";

    SAV_ENTER_PROG_NAME = "Enter a Name For Your Program";
    SAV_SAVE = "Save";
    SAV_CANCEL = "Cancel";
    SAV_CHOOSE_DIFF_NAME_MSG = "Please choose a different Program Name";
    SAV_CHOOSE_DIFF_NAME_TIT = "Choose a different Program Name";
    SAV_TYPE_PROG_NAME_MSG = "Type a Program Name that has letters and/or numbers in it";
    SAV_TYPE_PROG_NAME_TIT = "No Program Name entered";
    SAV_ONLY_NUM_LET_SPACE_MSG = "Only use letters, numbers, and spaces in a Program Name";
    SAV_INV_PROG_NAME = "Not a valid Program Name";
    SAV_NO_SAV_PROG_WITH_NUM_MSG = "Do not start the Program Name with a number";


    IDE_DELETE_PROG_MSG = "There was an error deleting the Program \"%s\"";
    IDE_DELETE_PROG_ERR_TIT = "Error deleting Program";
    IDE_OPEN_SAMPLE_CONFIRM_MSG = "Do you want to open a Sample Program without\nSaving your changes to this one?";
    IDE_OPEN_SAMPLE_CONFIRM_TIT = "Open Sample Program without Saving?";
    APP_EXCEPTION_OCCURED = "Runtime Error Occurred";
    IDE_EDIT_REDO = "Redo";
    IDE_EDIT_SELECT_ALL = "Select All";
    IDE_FILE_OPEN_SAMPLE = "Open Sample Program...";
  }
}