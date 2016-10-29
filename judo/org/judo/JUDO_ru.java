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
 * Russian localization
 */
public class JUDO_ru extends JUDO_localization {
  public JUDO_ru() {
    IDE_DEFAULT_PROG_NAME = "Без имени";
    IDE_SAVE_PROG = "Сохранить программу";
    IDE_ERR_INIT_JAVA = "Error Initializing Java, please read README.txt that came with JUDO";

    IDE_FILE = " Файл ";
    IDE_FILE_NEW = "Новый";
    IDE_FILE_SAVE = "Сохранить";
    IDE_FILE_SAVE_NEW_NAME = "Сохранить как...";
    IDE_FILE_SAVE_FLOPPY = "Сохранить на флопике";
    IDE_FILE_SAVE_FLOPPY_NEW_NAME = "Сохранить на флопике как...";
    IDE_FILE_OPEN = "Открыть программу...";
    IDE_FILE_OPEN_SAMPLE = "Открыть пример...";
    IDE_FILE_OPEN_FLOPPY = "Открыть программу на флопике...";
    IDE_FILE_DELETE = "Удалить программу...";
    IDE_FILE_DELETE_FLOPPY = "Удалить программу на флопике...";
    IDE_FILE_EXIT = "Выход";

    IDE_EDIT = " Правка ";
    IDE_EDIT_COPY = "Копировать";
    IDE_EDIT_CUT = "Вырезать";
    IDE_EDIT_PASTE = "Вставить";
    IDE_EDIT_SELECT_ALL = "Выделить всё";
    IDE_EDIT_UNDO = "Отменить";
    IDE_EDIT_REDO = "Повторить";
    IDE_EDIT_GOTO_LINE = "На строку...";

    IDE_PROG = "Программа";
    IDE_PROG_PROP = "Свойства программы...";

    IDE_RUN = "Запуск";
    IDE_RUN_RUN = "Выполнить";
    IDE_RUN_COMPILE = "Компилировать";

    IDE_HELP = " Помощь ";
    IDE_HELP_JUDO_HELP = "Справка JUDO";
    IDE_HELP_TUTORIALS = "Учебник JUDO";
    IDE_HELP_FUNC_REF = "Справка по методам";
    IDE_HELP_COLOR_REF = "Справка по цветам";
    IDE_HELP_ABOUT_JUDO = "О JUDO";
    IDE_HELP_JUDO_WEBSITE = "Веб-сайт JUDO";

    IDE_BUTTON_RUN = "Пуск";

    IDE_COMPILE_ERR = "Internal Compilation Error (javac interrupted)";
    IDE_404_ERR = "Error: File %s was not found.";
    IDE_READ_FILE_ERR = "Error: Reading file %s.";
    IDE_CREATE_FILE_ERR = "Error: Cannot create file %s.";
    IDE_EXIT_JUDO_NO_SAVE_MSG = "Вы действительно хотите выйти из JUDO, не сохранив текст программы?";
    IDE_EXIT_NO_SAVE_TIT = "Выйти без сохранения?";
    IDE_SAVE_NEW_NAME_TIT = "Сохранить с новым именем";
    IDE_SAVE_TIT = "Сохранить";
    IDE_PROG_EXISTS_MSG = "Program \"%s\" already exists.  Replace it?";
    IDE_PROG_EXISTS_TIT = "Такая программа уже существует";
    IDE_NO_FLOPPY_MSG = "There does not seem to be a Floppy disk in\nthe Floppy drive.  If there is, try ejecting\nit, putting it back in, and saving again.";
    IDE_FLOPPY_ERR_TIT = "Error saving to Floppy Disk";
    IDE_FLOPPY_WRITE_PROT_MSG = "Check that the Floppy Disk is not write protected.\nLook at the back of the disk.  In the top-left corner,\nthere is a little moveable tab.  Push it in the down\nposition if it is not there already and try to save again.";
    IDE_NO_PROG_DIR_MSG = "Program Directory did not exist and its re-creation failed.";
    IDE_ERR_SAVING_TIT = "Error saving";
    IDE_PROG_DIR_NOT_WRIT = "Error saving to Program Directory.  It is not writeable.";
    IDE_OPEN_NO_SAVE_MSG = "Вы действительно хотите открыть программу,\nне сохранив изменения в тексте?";
    IDE_OPEN_NO_SAVE_TIT = "Открыть без сохранения?";
    IDE_OPEN_NO_FLOPPY_MSG = "There does not seem to be a Floppy disk in\nthe Floppy drive.  If there is, try ejecting\nit, putting it back in, and opening again.";
    IDE_OPEN_ERR_TIT = "Error opening from Floppy Disk";
    IDE_NO_PROG_MSG = "There are currently no Programs to Open";
    IDE_NO_PROG_TIT = "No Programs to Open";
    IDE_OPEN_TIT = "Открыть программу";
    IDE_OPEN_PROG_MSG = "There was an error opening the Program \"%s\"";
    IDE_OPEN_PROG_ERR_TIT = "Error opening Program";
    IDE_NO_PROG_DEL_MSG = "There are currently no Programs to Delete";
    IDE_NO_PROG_DEL_TIT = "No Programs to Delete";
    IDE_DEL_NO_FLOPPY_MSG = "There does not seem to be a Floppy disk in\nthe Floppy drive.  If there is, try ejecting\nit, putting it back in, and deleting again.";
    IDE_DEL_ERR_TIT = "Error deleting from Floppy Disk";
    IDE_DEL_TIT = "Удалить программу";
    IDE_NEW_WITHOUT_SAVE_MSG = "Вы действительно хотите начать новую программу,\nне сохранив изменения в тексте?";
    IDE_NEW_WITHOUT_SAVE_TIT = "Начать программу без сохранения?";
    IDE_GOTO_LINE_NUM_MSG = "На строку с номером";
    IDE_GOTO_LINE_NUM_TIT = "На строку с номером";
    IDE_LINE_NUM_GREATER_MSG = "Line number must be greater than or equal to 1";
    IDE_INV_LINE_NUM_TIT = "Invalid Line Number";
    IDE_LINE_COUNT_MSG = "There are only %s lines in the Program";
    IDE_NO_LINE_NUM_MSG = "You did not enter a line number";
    IDE_LINE_NUM_INV_MSG = "Line number %s is not a valid line number.";

    IDE_VAR_VAL_DEC_ASS_TIT = "Variables, Values, Declarations, and Assignments";
    IDE_JUDO_ABOUT_MSG = "JUDO - среда программирования на языке Java для начинающих\nhttp://judo.sourceforge.net\nVersion: %s\n\nJUDO, Copyright (C) 2001-2003 Thomas J. Dunn";

    CER_STRING_MISSING_QUOTE = "String missing a \"";
    CER_EMPTY_CHAR = "Empty character%sDid you mean to use a double-quote character and not two single-quote characters?";
    CER_UNCLOSED_CHAR = "Character Error%sDid you mean to use a double-quote character and not two single-quote characters?";
    CER_INVALID_STATEMENT = "Not a valid Java statement";
    CER_CURLY_BRACE_NO_MATCH = "Curly braces { } not matching?";
    CER_NO_VOID_MAIN = "Error: missing 'void main()'";
    CER_ERR_ON_LINE = "Ошибка в строке ";
    CER_COMP_SUCCESS = "Компиляция выполнена.";
    CER_UNDEF_VAR = "Необъявленная переменная ";
    CER_UNDEF_FUNC = "Неизвестный метод ";
    CER_UNDEF_SYMBOL = "Неизвестный символ ";

    APP_PROG_COMPLETE = "Программа завершена";

    DEL_PROG_TO_DELETE = "Choose a Program to Delete";
    DEL_OK = "Удалить";
    DEL_CANCEL = "Отменить";
    DEL_CONFIRM_TIT = "Удалить программу?";
    DEL_CONFIRM_MSG = "Вы уверены, что хотите удалить программу '%s'?";

    OPN_PROG_TO_OPEN = "Выберите программу";
    OPN_OK = "Готово";
    OPN_CANCEL = "Отменить";

    PRP_CHOOSE_PROG_PROP = "Выберите свойства для этой программы";
    PRP_GRAPHICS = "Графика";
    PRP_TEXT = "Текст";
    PRP_TEXT_AND_GRAPHICS = "Текст & графика";
    PRP_WIN_STYLE = "Оконный стиль";
    PRP_OK = "Готово";
    PRP_CANCEL = "Отменить";

    SAV_ENTER_PROG_NAME = "Введите имя для Вашей программы";
    SAV_SAVE = "Сохранить";
    SAV_CANCEL = "Отменить";
    SAV_CHOOSE_DIFF_NAME_MSG = "Пожалуйста, выберите другое имя";
    SAV_CHOOSE_DIFF_NAME_TIT = "Выберите другое имя для программы";
    SAV_TYPE_PROG_NAME_MSG = "Введите имя, состоящее из букв и/или цифр";
    SAV_TYPE_PROG_NAME_TIT = "Не введено имя программы";
    SAV_ONLY_NUM_LET_SPACE_MSG = "В имени программы допускаются только буквы, цифры и пробелы";
    SAV_INV_PROG_NAME = "Неправильное имя программы";
    SAV_NO_SAV_PROG_WITH_NUM_MSG = "Не начинайте имя программы с цифры";

    IDE_DELETE_PROG_MSG = "There was an error deleting the Program \"%s\"";
    IDE_DELETE_PROG_ERR_TIT = "Error deleting Program";
    IDE_OPEN_SAMPLE_CONFIRM_MSG = "Вы действительно хотите открыть пример,\nне сохранив изменения в тексте?";
    IDE_OPEN_SAMPLE_CONFIRM_TIT = "Открыть пример без сохранения?";
    APP_EXCEPTION_OCCURED = "Runtime Error Occurred";
  }
}
