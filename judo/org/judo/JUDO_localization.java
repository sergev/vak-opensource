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
 * defines all the strings that are necessary for localizations
 */
public class JUDO_localization {

  public JUDO_localization() {
    try {
      Class lz = Class.forName("org.judo.JUDO_localization");
    }
    catch (ClassNotFoundException cnfe) {
      System.out.println("Caught ClassNotFoundException in JUDO_localization constructor: " + cnfe.toString());
      cnfe.printStackTrace();
    }
  }

  public String IDE_DEFAULT_PROG_NAME;
  public String IDE_SAVE_PROG;
  public String IDE_ERR_INIT_JAVA;

  public String IDE_FILE;
  public String IDE_FILE_NEW;
  public String IDE_FILE_SAVE;
  public String IDE_FILE_SAVE_NEW_NAME;
  public String IDE_FILE_SAVE_FLOPPY;
  public String IDE_FILE_SAVE_FLOPPY_NEW_NAME;
  public String IDE_FILE_OPEN;
  public String IDE_FILE_OPEN_FLOPPY;
  public String IDE_FILE_DELETE;
  public String IDE_FILE_DELETE_FLOPPY;
  public String IDE_FILE_EXIT;

  public String IDE_EDIT;
  public String IDE_EDIT_COPY;
  public String IDE_EDIT_CUT;
  public String IDE_EDIT_PASTE;
  public String IDE_EDIT_UNDO;
  public String IDE_EDIT_GOTO_LINE;

  public String IDE_PROG;
  public String IDE_PROG_PROP;

  public String IDE_RUN;
  public String IDE_RUN_RUN;
  public String IDE_RUN_COMPILE;

  public String IDE_HELP;
  public String IDE_HELP_JUDO_HELP;
  public String IDE_HELP_TUTORIALS;
  public String IDE_HELP_FUNC_REF;
  public String IDE_HELP_COLOR_REF;
  public String IDE_HELP_ABOUT_JUDO;
  public String IDE_HELP_JUDO_WEBSITE;

  public String IDE_BUTTON_RUN;

  public String IDE_COMPILE_ERR;
  public String IDE_404_ERR;
  public String IDE_READ_FILE_ERR;
  public String IDE_CREATE_FILE_ERR;
  public String IDE_EXIT_JUDO_NO_SAVE_MSG;
  public String IDE_EXIT_NO_SAVE_TIT;
  public String IDE_SAVE_NEW_NAME_TIT;
  public String IDE_SAVE_TIT;
  public String IDE_PROG_EXISTS_MSG;
  public String IDE_PROG_EXISTS_TIT;
  public String IDE_NO_FLOPPY_MSG;
  public String IDE_FLOPPY_ERR_TIT;
  public String IDE_FLOPPY_WRITE_PROT_MSG;
  public String IDE_NO_PROG_DIR_MSG;
  public String IDE_ERR_SAVING_TIT;
  public String IDE_PROG_DIR_NOT_WRIT;
  public String IDE_OPEN_NO_SAVE_MSG;
  public String IDE_OPEN_NO_SAVE_TIT;
  public String IDE_OPEN_NO_FLOPPY_MSG;
  public String IDE_OPEN_ERR_TIT;
  public String IDE_NO_PROG_MSG;
  public String IDE_NO_PROG_TIT;
  public String IDE_OPEN_TIT;
  public String IDE_OPEN_PROG_MSG;
  public String IDE_OPEN_PROG_ERR_TIT;
  public String IDE_NO_PROG_DEL_MSG;
  public String IDE_NO_PROG_DEL_TIT;
  public String IDE_DEL_NO_FLOPPY_MSG;
  public String IDE_DEL_ERR_TIT;
  public String IDE_DEL_TIT;
  public String IDE_NEW_WITHOUT_SAVE_MSG;
  public String IDE_NEW_WITHOUT_SAVE_TIT;
  public String IDE_GOTO_LINE_NUM_MSG;
  public String IDE_GOTO_LINE_NUM_TIT;
  public String IDE_LINE_NUM_GREATER_MSG;
  public String IDE_INV_LINE_NUM_TIT;
  public String IDE_LINE_COUNT_MSG;
  public String IDE_NO_LINE_NUM_MSG;
  public String IDE_LINE_NUM_INV_MSG;

  public String IDE_VAR_VAL_DEC_ASS_TIT;
  public String IDE_JUDO_ABOUT_MSG;

  public String CER_STRING_MISSING_QUOTE;
  public String CER_EMPTY_CHAR;
  public String CER_UNCLOSED_CHAR;
  public String CER_INVALID_STATEMENT;
  public String CER_CURLY_BRACE_NO_MATCH;
  public String CER_NO_VOID_MAIN;
  public String CER_ERR_ON_LINE;
  public String CER_COMP_SUCCESS;
  public String CER_UNDEF_VAR;
  public String CER_UNDEF_FUNC;
  public String CER_UNDEF_SYMBOL;

  public String APP_PROG_COMPLETE;

  public String DEL_PROG_TO_DELETE;
  public String DEL_OK;
  public String DEL_CANCEL;
  public String DEL_CONFIRM_TIT;
  public String DEL_CONFIRM_MSG;

  public String OPN_PROG_TO_OPEN;
  public String OPN_OK;
  public String OPN_CANCEL;

  public String PRP_CHOOSE_PROG_PROP;
  public String PRP_GRAPHICS;
  public String PRP_TEXT;
  public String PRP_TEXT_AND_GRAPHICS;
  public String PRP_WIN_STYLE;
  public String PRP_OK;
  public String PRP_CANCEL;

  public String SAV_ENTER_PROG_NAME;
  public String SAV_SAVE;
  public String SAV_CANCEL;
  public String SAV_CHOOSE_DIFF_NAME_MSG;
  public String SAV_CHOOSE_DIFF_NAME_TIT;
  public String SAV_TYPE_PROG_NAME_MSG;
  public String SAV_TYPE_PROG_NAME_TIT;
  public String SAV_ONLY_NUM_LET_SPACE_MSG;
  public String SAV_INV_PROG_NAME;
  public String SAV_NO_SAV_PROG_WITH_NUM_MSG;


  public String IDE_DELETE_PROG_MSG;
  public String IDE_DELETE_PROG_ERR_TIT;
  public String IDE_OPEN_SAMPLE_CONFIRM_MSG;
  public String IDE_OPEN_SAMPLE_CONFIRM_TIT;
  public String APP_EXCEPTION_OCCURED;
  public String IDE_EDIT_REDO;
  public String IDE_EDIT_SELECT_ALL;
  public String IDE_FILE_OPEN_SAMPLE;
}