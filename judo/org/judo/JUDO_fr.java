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
 * French localization
 */
public class JUDO_fr extends JUDO_localization {
  public JUDO_fr() {
    IDE_DEFAULT_PROG_NAME = "Sans titre";
    IDE_SAVE_PROG = "Enregistrer le programme";
    IDE_ERR_INIT_JAVA = "Erreur d'initialisation de Java, consultez le fichier README.txt livrИ avec JUDO";

    IDE_FILE = "Fichier";
    IDE_FILE_NEW = "Nouveau";
    IDE_FILE_SAVE = "Enregistrer";
    IDE_FILE_SAVE_NEW_NAME = "Enregistrer sous...";
    IDE_FILE_SAVE_FLOPPY = "Enregistrer sur disquette";
    IDE_FILE_SAVE_FLOPPY_NEW_NAME = "Enregistrer sur disquette sous...";
    IDE_FILE_OPEN = "Ouvrir le programme...";
    IDE_FILE_OPEN_FLOPPY = "Ouvrir le programme de la disquette...";
    IDE_FILE_DELETE = "Effacer le programme...";
    IDE_FILE_DELETE_FLOPPY = "Effacer le programme de la disquette...";
    IDE_FILE_EXIT = "Quitter";

    IDE_EDIT = "Edition";
    IDE_EDIT_COPY = "Copier";
    IDE_EDIT_CUT = "Couper";
    IDE_EDIT_PASTE = "Coller";
    IDE_EDIT_UNDO = "Annuler";
    IDE_EDIT_GOTO_LINE = "Aller Ю la ligne...";

    IDE_PROG = "Programme";
    IDE_PROG_PROP = "PropriИtИs du programme...";

    IDE_RUN = "ExИcuter";
    IDE_RUN_RUN = "ExИcuter";
    IDE_RUN_COMPILE = "Compiler";

    IDE_HELP = "Aide";
    IDE_HELP_JUDO_HELP = "JUDO Aide";
    IDE_HELP_TUTORIALS = "Tutorials";
    IDE_HELP_FUNC_REF = "JUDO RИfИrence des fonctions";
    IDE_HELP_COLOR_REF = "RИfИrence des valeurs de couleurs";
    IDE_HELP_ABOUT_JUDO = "A propos de JUDO";
    IDE_HELP_JUDO_WEBSITE = "Le site web de JUDO";

    IDE_BUTTON_RUN = "Action";

    IDE_COMPILE_ERR = "Erreur de compilation interne (javac interrompu)";
    IDE_404_ERR = "Erreur: Fichier %s n'a pas ИtИ trouvИ.";
    IDE_READ_FILE_ERR = "Erreur: Lecture du fichier %s.";
    IDE_CREATE_FILE_ERR = "Erreur: Fichier %s ne peux pas etre crИИ.";
    IDE_EXIT_JUDO_NO_SAVE_MSG = "voulez vous quittr JUDO sans enregistrer ce Programme?";
    IDE_EXIT_NO_SAVE_TIT = "Quitter sans enregistrer?";
    IDE_SAVE_NEW_NAME_TIT = "Enregistrer sous...";
    IDE_SAVE_TIT = "Enregistrer";
    IDE_PROG_EXISTS_MSG = "Programme \"%s\" existe dИjЮ.  Le remplacer?";
    IDE_PROG_EXISTS_TIT = "Programme dИjЮ existant";
    IDE_NO_FLOPPY_MSG = "Il ne semble pas y avoir de disquette\ndans le lecteur.  Si c'est le cas, essayez d'ejecter la disquette \n, la remettre, et enregistrer Ю nouveau.";
    IDE_FLOPPY_ERR_TIT = "Erreur lors de l'enregistrement sur disquette";
    IDE_FLOPPY_WRITE_PROT_MSG = "Controler que la disquette n'est pas protИgИe en Иcriture.\nRegardez sur l'arriХre de la disquette.  Il y a une piХce amovible,\nsur le coin gauche. La presser vers le bas\nsi elle n'y Иtait pas encore et retenter l'enregistrement.";
    IDE_NO_PROG_DIR_MSG = "Le rИpertoire du programme n'existe pas et sa recrИation a ratИ.";
    IDE_ERR_SAVING_TIT = "Erreur d'enregistrement";
    IDE_PROG_DIR_NOT_WRIT = "Erreur d'enregistrement du rИpertoire.  on ne peut pas y Иcrire.";
    IDE_OPEN_NO_SAVE_MSG = "Voulez-vous ouvrir un Programme sans\nenregistrer vos changements sur l'actuel?";
    IDE_OPEN_NO_SAVE_TIT = "Ouvrir sans enregistrer?";
    IDE_OPEN_NO_FLOPPY_MSG = "Il ne semble pas y avoir de disquette\ndans le lecteur.  Si c'est le cas, essayez d'ejecter la disquette \n, la remettre, et ouvrir Ю nouveau.";
    IDE_OPEN_ERR_TIT = "Erreur en chargeant de la disquette";
    IDE_NO_PROG_MSG = "Il n'y a pas de programmes Ю ouvrir";
    IDE_NO_PROG_TIT = "Pas de programmes Ю ouvrir";
    IDE_OPEN_TIT = "Ouvrir Programme";
    IDE_OPEN_PROG_MSG = "Une erreur est survenue durant l'ouverture du Programme \"%s\"";
    IDE_OPEN_PROG_ERR_TIT = "Erreur durant l'ouverture du Programme";
    IDE_NO_PROG_DEL_MSG = "Il n'y a pas de programmes Ю effacer";
    IDE_NO_PROG_DEL_TIT = "=Pas de Programmes Ю effacer";
    IDE_DEL_NO_FLOPPY_MSG =  "Il ne semble pas y avoir de disquette\ndans le lecteur.  Si c'est le cas, essayez d'ejecter la disquette \n, la remettre, et effacer Ю nouveau.";
    IDE_DEL_ERR_TIT = "Erreur d'effacement de la disquette";
    IDE_DEL_TIT = "Effacer le Programme";
    IDE_NEW_WITHOUT_SAVE_MSG = "Voulez-vous dИmarrer und nouveau programme sans \nenregistrer les changements sur l'actuel?";
    IDE_NEW_WITHOUT_SAVE_TIT = "Nouveau Programme sans enregistrer?";
    IDE_GOTO_LINE_NUM_MSG = "Aller au numИro de ligne";
    IDE_GOTO_LINE_NUM_TIT = "Aller au numИro de ligne";
    IDE_LINE_NUM_GREATER_MSG = "numИro de ligne doit etre plus grand ou Иgal Ю 1";
    IDE_INV_LINE_NUM_TIT = "numИro de ligne pas valable";
    IDE_LINE_COUNT_MSG = "Il y a seulement %s lignes dans le Programme";
    IDE_NO_LINE_NUM_MSG = "Vous n'avez pas donner de numИro de ligne";
    IDE_LINE_NUM_INV_MSG = "numИro de ligne %s n'est pas valable.";

    IDE_VAR_VAL_DEC_ASS_TIT = "Variables, Valeurs, DИclarations, et Assignements";
    IDE_JUDO_ABOUT_MSG = "JUDO - Un IDE java pour les programmeurs dИbutants\nhttp://judo.sourceforge.net\nVersion: %s\n\nJUDO, Copyright (C) 2001-2003 Thomas J. Dunn";

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