// Spreadsheet API - Core spreadsheet logic separated from TUI
// This header defines the interface for spreadsheet operations

#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <stdbool.h>

// Spreadsheet dimensions
#define MAXCOLS 100 // Maximum is 702
#define MAXROWS 100

// Cell types
enum CellType {
    TEXT    = 0, //
    VALUE   = 1, //
    FORMULA = 2, //
};

// Format flags
#define OVERWRITE 0X80
#define RJUSTIFY  0X40
#define COMMAS    0X20
#define DOLLAR    0X10

// Action constants (for row/column operations)
enum RowColAction {
    COLADD = 0, //
    COLDEL = 1, //
    ROWADD = 2, //
    ROWDEL = 3, //
};

// Defaults
#define DEFAULTWIDTH  10
#define DEFAULTFORMAT 0X42

// Limits
#define MAXINPUT    79
#define MAXPLACES   8
#define MAXCOLWIDTH 80 - 3 // 80 - LEFTMARGIN (LEFTMARGIN = 3)

// Parser constants
#define PARSERSTACKSIZE 20

// Memory size calculation
#define memsize            1000000
#define textcellsize(s)    (strlen(s) + sizeof(char *) + 1)
#define valuecellsize      (sizeof(double) + sizeof(char *) + 1)
#define formulacellsize(s) (strlen(s) + sizeof(double) + sizeof(char *) + 1)

// Message strings
#define MSGNAME       "Turbo C++ TurboCalc Spreadsheet"
#define MSGERROR      "ERROR"
#define MSGSTACKERROR "Parser stack overflow."

// Color constants (for display)
enum DisplayColor {
    TEXTCOLOR           = 1,  //
    ERRORCOLOR          = 2,  //
    VALUECOLOR          = 3,  //
    FORMULACOLOR        = 4,  //
    BLANKCOLOR          = 5,  //
    HEADERCOLOR         = 6,  //
    HIGHLIGHTCOLOR      = 7,  //
    HIGHLIGHTERRORCOLOR = 8,  //
    MSGAUTOCALCCOLOR    = 9,  //
    MSGFORMDISPLAYCOLOR = 10, //
    MSGMEMORYCOLOR      = 11, //
    MSGHEADERCOLOR      = 12, //
    PROMPTCOLOR         = 13, //
    COMMANDCOLOR        = 14, //
    LOWCOMMANDCOLOR     = 15, //
    MEMORYCOLOR         = 16, //
    CELLTYPECOLOR       = 17, //
    CELLCONTENTSCOLOR   = 18, //
};

// Cell structure definition
struct CELLREC {
    char attrib;
    union {
        char text[MAXINPUT + 1];
        double value;
        struct {
            double fvalue;
            char formula[MAXINPUT + 1];
        } f;
    } v;
};

typedef struct CELLREC *CELLPTR;

// Spreadsheet state structure
typedef struct Spreadsheet {
    CELLPTR cell[MAXCOLS][MAXROWS];
    unsigned char format[MAXCOLS][MAXROWS];
    unsigned char colwidth[MAXCOLS];
    int lastcol, lastrow;
    long memleft;
    bool changed;
    bool autocalc;
    bool formdisplay;
} Spreadsheet;

struct Spreadsheet *get_spreadsheet(void);

// Cell content information for display
typedef struct {
    char *text;   // String representation of cell
    int color;    // Color code for display
    int celltype; // TEXT, VALUE, or FORMULA
} CellDisplay;

// Initialize spreadsheet
void spreadsheet_init(Spreadsheet *sheet);

// Clear spreadsheet (removes all cells but keeps structure)
void spreadsheet_clear(Spreadsheet *sheet);

// Cell operations
bool spreadsheet_set_cell_text(Spreadsheet *sheet, int col, int row, const char *text);
bool spreadsheet_set_cell_value(Spreadsheet *sheet, int col, int row, double value);
bool spreadsheet_set_cell_formula(Spreadsheet *sheet, int col, int row, const char *formula);
void spreadsheet_delete_cell(Spreadsheet *sheet, int col, int row);
int spreadsheet_get_cell_display(Spreadsheet *sheet, int col, int row, CellDisplay *display,
                                 int formatting);

// Cell value retrieval (for formulas)
double spreadsheet_get_cell_value(Spreadsheet *sheet, int col, int row);

// Formula operations
double spreadsheet_parse_formula(Spreadsheet *sheet, const char *s, int *attrib);
void spreadsheet_recalc(Spreadsheet *sheet);
void spreadsheet_fix_formula(Spreadsheet *sheet, int col, int row, int action, int place);

// Row/Column operations
void spreadsheet_insert_col(Spreadsheet *sheet, int col);
void spreadsheet_delete_col(Spreadsheet *sheet, int col);
void spreadsheet_insert_row(Spreadsheet *sheet, int row);
void spreadsheet_delete_row(Spreadsheet *sheet, int row);
void spreadsheet_set_col_width(Spreadsheet *sheet, int col, int width);

// Format operations
void spreadsheet_format_cells(Spreadsheet *sheet, int col1, int row1, int col2, int row2,
                              int newformat);

// File operations
int spreadsheet_load(Spreadsheet *sheet, const char *filename);
int spreadsheet_save(Spreadsheet *sheet, const char *filename);

// Utility functions
void spreadsheet_set_last_col(Spreadsheet *sheet);
void spreadsheet_set_last_row(Spreadsheet *sheet);
int spreadsheet_get_last_col(const Spreadsheet *sheet);
int spreadsheet_get_last_row(const Spreadsheet *sheet);
long spreadsheet_get_memleft(const Spreadsheet *sheet);
bool spreadsheet_is_changed(const Spreadsheet *sheet);
void spreadsheet_set_changed(Spreadsheet *sheet, bool changed);
bool spreadsheet_get_autocalc(const Spreadsheet *sheet);
void spreadsheet_set_autocalc(Spreadsheet *sheet, bool autocalc);
bool spreadsheet_get_formdisplay(const Spreadsheet *sheet);
void spreadsheet_set_formdisplay(Spreadsheet *sheet, bool formdisplay);

// Cell string conversion (for formulas and display)
char *spreadsheet_cell_to_string(int col, char *colstr);
void spreadsheet_center_col_string(int col, int colwidth, char *colstr);

// Overwrite flag operations (for text cells that span multiple columns)
int spreadsheet_set_overwrite_flags(Spreadsheet *sheet, int col, int row);
void spreadsheet_clear_overwrite_flags(Spreadsheet *sheet, int col, int row);
void spreadsheet_update_overwrite_flags(Spreadsheet *sheet, int col, int row);

// Parser error callback setup (to isolate parser from TUI)
void set_parser_error_callback(void (*callback)(const char *));

// Parser function declarations (used by spreadsheet.c)
double parse(char *s, int *att);
bool formulastart(char **input, int *col, int *row);
int rowwidth(int row);
double cellvalue(int col, int row);

#endif // SPREADSHEET_H
