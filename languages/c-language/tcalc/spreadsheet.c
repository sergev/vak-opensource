// Spreadsheet core logic - separated from TUI
// Contains all spreadsheet operations without display dependencies

#include "spreadsheet.h"

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// Forward declarations for parser functions
extern double parse(char *s, int *att);
extern double cellvalue(int col, int row);
extern void set_parser_error_callback(void (*callback)(const char *));

// Spreadsheet instance - core data structure
static Spreadsheet *sheet;

// Get spreadsheet instance (for other modules)
Spreadsheet *get_spreadsheet(void)
{
    return sheet;
}

//
// Utility functions (moved from tcutil.c)
//

//
// Returns the width in spaces of row
//
int rowwidth(int row)
{
    return ((row == 0) ? 1 : (int)log10(row + 1) + 1);
}

//
// Returns true if the string is the start of a formula, false otherwise.
// Also returns the column and row of the formula.
//
bool formulastart(char **input, int *col, int *row)
{
    int len, maxlen = rowwidth(MAXROWS);
    char *start, numstring[10];

    if (!isalpha(**input))
        return (false);
    *col = *((*input)++) - 'A';
    if (isalpha(**input)) {
        *col *= 26;
        *col += *((*input)++) - 'A' + 26;
    }
    if (*col >= MAXCOLS)
        return (false);
    start = *input;
    for (len = 0; len < maxlen; len++) {
        if (!isdigit(*((*input)++))) {
            (*input)--;
            break;
        }
    }
    if (len == 0)
        return (false);
    strncpy(numstring, start, len);
    numstring[len] = 0;
    *row           = atoi(numstring) - 1;
    if ((*row >= MAXROWS) || (*row == -1))
        return (false);
    return (true);
}

//
// Swaps the first and second values
//
void swap(int *val1, int *val2)
{
    int temp;

    temp  = *val1;
    *val1 = *val2;
    *val2 = temp;
}

// Error callback for parser (forwards to TUI layer)
static void parser_error_handler(const char *msg)
{
    // This will be set up to call the TUI error function
    // For now, we'll need to link this properly
    extern void errormsg(char *s);
    errormsg((char *)msg);
}

// Initialize spreadsheet
void spreadsheet_init(Spreadsheet *init_sheet)
{
    int col, row;

    sheet              = init_sheet;
    sheet->lastcol     = 0;
    sheet->lastrow     = 0;
    sheet->memleft     = memsize;
    sheet->changed     = false;
    sheet->autocalc    = true;
    sheet->formdisplay = false;

    for (col = 0; col < MAXCOLS; col++) {
        sheet->colwidth[col] = DEFAULTWIDTH;
        for (row = 0; row < MAXROWS; row++) {
            sheet->cell[col][row]   = NULL;
            sheet->format[col][row] = DEFAULTFORMAT;
        }
    }

    // Set up parser error callback to isolate parser from TUI
    set_parser_error_callback(parser_error_handler);
}

// Clear spreadsheet (removes all cells but keeps structure)
void spreadsheet_clear(Spreadsheet *sheet)
{
    int col, row;

    for (row = 0; row <= sheet->lastrow; row++) {
        for (col = 0; col <= sheet->lastcol; col++)
            spreadsheet_delete_cell(sheet, col, row);
    }
    sheet->lastcol = 0;
    sheet->lastrow = 0;
    spreadsheet_set_last_col(sheet);
    spreadsheet_set_last_row(sheet);
    sheet->changed = false;
}

// Allocate space for a text cell
static bool alloctext_internal(Spreadsheet *sheet, int col, int row, const char *s)
{
    int size;
    CELLPTR cellptr;

    if (sheet->memleft < (size = textcellsize(s)))
        return false;
    sheet->memleft -= size;
    cellptr = (CELLPTR)(malloc(strlen(s) + 2));
    if (cellptr == NULL)
        return false;
    cellptr->attrib = TEXT;
    strcpy(cellptr->v.text, s);
    sheet->cell[col][row] = cellptr;
    return true;
}

// Allocate space for a value cell
static bool allocvalue_internal(Spreadsheet *sheet, int col, int row, double amt)
{
    if (sheet->memleft < (long)valuecellsize)
        return false;
    sheet->memleft -= valuecellsize;
    CELLPTR cellptr = (CELLPTR)(malloc(sizeof(double) + 1));
    if (cellptr == NULL)
        return false;
    cellptr->attrib       = VALUE;
    cellptr->v.value      = amt;
    sheet->cell[col][row] = cellptr;
    return true;
}

// Allocate space for a formula cell
static bool allocformula_internal(Spreadsheet *sheet, int col, int row, const char *s, double amt)
{
    int size;
    CELLPTR cellptr;

    if (sheet->memleft < (size = formulacellsize(s)))
        return false;
    sheet->memleft -= size;
    cellptr = (CELLPTR)(malloc(strlen(s) + sizeof(double) + 2));
    if (cellptr == NULL)
        return false;
    cellptr->attrib = FORMULA;
    strcpy(cellptr->v.f.formula, s);
    cellptr->v.f.fvalue   = amt;
    sheet->cell[col][row] = cellptr;
    return true;
}

// Set cell to text
bool spreadsheet_set_cell_text(Spreadsheet *sheet, int col, int row, const char *text)
{
    spreadsheet_delete_cell(sheet, col, row);
    bool result = alloctext_internal(sheet, col, row, text);
    if (result) {
        sheet->format[col][row] &= ~OVERWRITE;
        if (col > sheet->lastcol)
            sheet->lastcol = col;
        if (row > sheet->lastrow)
            sheet->lastrow = row;
    }
    return result;
}

// Set cell to value
bool spreadsheet_set_cell_value(Spreadsheet *sheet, int col, int row, double value)
{
    spreadsheet_delete_cell(sheet, col, row);
    bool result = allocvalue_internal(sheet, col, row, value);
    if (result) {
        if (col > sheet->lastcol)
            sheet->lastcol = col;
        if (row > sheet->lastrow)
            sheet->lastrow = row;
    }
    return result;
}

// Set cell to formula
bool spreadsheet_set_cell_formula(Spreadsheet *sheet, int col, int row, const char *formula)
{
    int attrib;
    double value;
    char formula_copy[MAXINPUT + 1];

    spreadsheet_delete_cell(sheet, col, row);
    strcpy(formula_copy, formula);
    value = spreadsheet_parse_formula(sheet, formula_copy, &attrib);
    if (attrib != FORMULA)
        return false;

    bool result = allocformula_internal(sheet, col, row, formula_copy, value);
    if (result) {
        sheet->format[col][row] &= ~OVERWRITE;
        if (col > sheet->lastcol)
            sheet->lastcol = col;
        if (row > sheet->lastrow)
            sheet->lastrow = row;
    }
    return result;
}

// Delete a cell
void spreadsheet_delete_cell(Spreadsheet *sheet, int col, int row)
{
    CELLPTR cellptr = sheet->cell[col][row];

    if (cellptr == NULL)
        return;

    switch (cellptr->attrib) {
    case TEXT:
        sheet->memleft += textcellsize(cellptr->v.text);
        break;
    case VALUE:
        sheet->memleft += valuecellsize;
        break;
    case FORMULA:
        sheet->memleft += formulacellsize(cellptr->v.f.formula);
        break;
    }
    sheet->format[col][row] &= ~OVERWRITE;
    free(sheet->cell[col][row]);
    sheet->cell[col][row] = NULL;

    if (col == sheet->lastcol)
        spreadsheet_set_last_col(sheet);
    if (row == sheet->lastrow)
        spreadsheet_set_last_row(sheet);

    sheet->changed = true;
}

// Get cell value (for formulas)
double spreadsheet_get_cell_value(Spreadsheet *sheet, int col, int row)
{
    if (sheet->cell[col][row] == NULL)
        return 0;
    if (sheet->cell[col][row]->attrib == TEXT)
        return HUGE_VAL;
    if (sheet->cell[col][row]->attrib == FORMULA)
        return sheet->cell[col][row]->v.f.fvalue;
    return sheet->cell[col][row]->v.value;
}

// Parse formula - wrapper around parse() that uses spreadsheet's cells
double spreadsheet_parse_formula(Spreadsheet *sheet, const char *s, int *attrib)
{
    // Parser now uses get_spreadsheet() internally, so no need to sync global array
    // Note: sheet parameter kept for API consistency, but parser accesses via get_spreadsheet()
    (void)sheet; // Suppress unused parameter warning
    char s_copy[MAXINPUT + 1];
    strcpy(s_copy, s);
    double result = parse(s_copy, attrib);
    strcpy((char *)s, s_copy); // Update s with parsed version
    return result;
}

// Recalculate all formulas
void spreadsheet_recalc(Spreadsheet *sheet)
{
    int col, row, dummy;

    for (col = 0; col <= sheet->lastcol; col++) {
        for (row = 0; row <= sheet->lastrow; row++) {
            if ((sheet->cell[col][row] != NULL) && (sheet->cell[col][row]->attrib == FORMULA)) {
                // Parser now uses get_spreadsheet() internally, so no need to sync global array
                sheet->cell[col][row]->v.f.fvalue =
                    parse(sheet->cell[col][row]->v.f.formula, &dummy);
            }
        }
    }
}

// Fix formula when columns/rows are inserted/deleted
void spreadsheet_fix_formula(Spreadsheet *sheet, int col, int row, int action, int place)
{
    char *colstart, *rowstart, s[6], newformula[MAXINPUT + 1], *curpos = newformula;
    int fcol, frow;
    CELLPTR cellptr = sheet->cell[col][row];
    double value;

    if (cellptr == NULL || cellptr->attrib != FORMULA)
        return;

    strcpy(newformula, cellptr->v.f.formula);
    while (*curpos != 0) {
        if (formulastart(&curpos, &fcol, &frow)) {
            rowstart = curpos - rowwidth(frow);
            colstart = rowstart - ((fcol > 25) ? 2 : 1);
            switch (action) {
            case COLADD:
                if (fcol < place)
                    break;
                if (fcol == 25) {
                    if (strlen(newformula) == MAXINPUT) {
                        spreadsheet_delete_cell(sheet, col, row);
                        spreadsheet_set_cell_text(sheet, col, row, newformula);
                        return;
                    }
                    memmove(colstart + 1, colstart, strlen(colstart) + 1);
                }
                spreadsheet_cell_to_string(fcol + 1, s);
                memcpy(colstart, s, strlen(s));
                break;
            case ROWADD:
                if (frow < place)
                    break;
                if (rowwidth(frow + 1) != rowwidth(frow)) {
                    if (strlen(newformula) == MAXINPUT) {
                        spreadsheet_delete_cell(sheet, col, row);
                        spreadsheet_set_cell_text(sheet, col, row, newformula);
                        return;
                    }
                    memmove(rowstart + 1, rowstart, strlen(rowstart) + 1);
                }
                sprintf(s, "%d", frow + 2);
                memcpy(rowstart, s, strlen(s));
                break;
            case COLDEL:
                if (fcol <= place)
                    break;
                if (fcol == 26)
                    memmove(colstart, colstart + 1, strlen(colstart) + 1);
                spreadsheet_cell_to_string(fcol - 1, s);
                memcpy(colstart, s, strlen(s));
                break;
            case ROWDEL:
                if (frow <= place)
                    break;
                if (rowwidth(frow) != rowwidth(frow - 1))
                    memmove(rowstart, rowstart + 1, strlen(rowstart) + 1);
                sprintf(s, "%d", frow);
                memcpy(rowstart, s, strlen(s));
                break;
            }
        } else
            curpos++;
    }
    if (strlen(newformula) != strlen(cellptr->v.f.formula)) {
        value = cellptr->v.f.fvalue;
        spreadsheet_delete_cell(sheet, col, row);
        spreadsheet_set_cell_formula(sheet, col, row, newformula);
        (void)value; // Suppress unused variable warning
    } else
        strcpy(cellptr->v.f.formula, newformula);
}

// Insert column
void spreadsheet_insert_col(Spreadsheet *sheet, int col)
{
    int counter, row;

    if (sheet->lastcol == MAXCOLS - 1) {
        for (counter = 0; counter <= sheet->lastrow; counter++)
            spreadsheet_delete_cell(sheet, sheet->lastcol, counter);
    }
    if (col != MAXCOLS - 1) {
        memmove(&sheet->cell[col + 1][0], &sheet->cell[col][0],
                MAXROWS * sizeof(CELLPTR) * (MAXCOLS - col - 1));
        memmove(&sheet->format[col + 1][0], &sheet->format[col][0], MAXROWS * (MAXCOLS - col - 1));
        memmove(&sheet->colwidth[col + 1], &sheet->colwidth[col], MAXCOLS - col - 1);
    }
    for (row = 0; row < MAXROWS; row++) {
        sheet->cell[col][row]   = NULL;
        sheet->format[col][row] = DEFAULTFORMAT;
    }
    sheet->colwidth[col] = DEFAULTWIDTH;
    sheet->lastcol       = MAXCOLS - 1;
    spreadsheet_set_last_col(sheet);

    for (counter = 0; counter <= sheet->lastcol; counter++) {
        for (row = 0; row <= sheet->lastrow; row++) {
            if ((sheet->cell[counter][row] != NULL) &&
                (sheet->cell[counter][row]->attrib == FORMULA))
                spreadsheet_fix_formula(sheet, counter, row, COLADD, col);
        }
    }
    sheet->changed = true;
}

// Delete column
void spreadsheet_delete_col(Spreadsheet *sheet, int col)
{
    int counter, row;

    for (counter = 0; counter <= sheet->lastrow; counter++)
        spreadsheet_delete_cell(sheet, col, counter);

    if (col != MAXCOLS - 1) {
        memmove(&sheet->cell[col][0], &sheet->cell[col + 1][0],
                MAXROWS * sizeof(CELLPTR) * (MAXCOLS - col - 1));
        memmove(&sheet->format[col][0], &sheet->format[col + 1][0], MAXROWS * (MAXCOLS - col - 1));
        memmove(&sheet->colwidth[col], &sheet->colwidth[col + 1], MAXCOLS - col - 1);
    }
    for (row = 0; row < MAXROWS; row++) {
        sheet->cell[MAXCOLS - 1][row]   = NULL;
        sheet->format[MAXCOLS - 1][row] = DEFAULTFORMAT;
    }
    sheet->colwidth[MAXCOLS - 1] = DEFAULTWIDTH;

    if ((sheet->lastcol >= col) && (sheet->lastcol > 0))
        sheet->lastcol--;
    spreadsheet_set_last_col(sheet);

    for (counter = 0; counter <= sheet->lastcol; counter++) {
        for (row = 0; row <= sheet->lastrow; row++) {
            if ((sheet->cell[counter][row] != NULL) &&
                (sheet->cell[counter][row]->attrib == FORMULA))
                spreadsheet_fix_formula(sheet, counter, row, COLDEL, col);
        }
    }
    sheet->changed = true;
}

// Insert row
void spreadsheet_insert_row(Spreadsheet *sheet, int row)
{
    int counter, rowc;

    if (sheet->lastrow == MAXROWS - 1) {
        for (counter = 0; counter <= sheet->lastcol; counter++)
            spreadsheet_delete_cell(sheet, counter, sheet->lastrow);
    }
    if (row != MAXROWS - 1) {
        for (counter = 0; counter < MAXCOLS; counter++) {
            memmove(&sheet->cell[counter][row + 1], &sheet->cell[counter][row],
                    sizeof(CELLPTR) * (MAXROWS - row - 1));
            memmove(&sheet->format[counter][row + 1], &sheet->format[counter][row],
                    MAXROWS - row - 1);
        }
    }
    for (counter = 0; counter < MAXCOLS; counter++) {
        sheet->cell[counter][row]   = NULL;
        sheet->format[counter][row] = DEFAULTFORMAT;
    }
    sheet->lastrow = MAXROWS - 1;
    spreadsheet_set_last_row(sheet);

    for (counter = 0; counter <= sheet->lastcol; counter++) {
        for (rowc = 0; rowc <= sheet->lastrow; rowc++) {
            if ((sheet->cell[counter][rowc] != NULL) &&
                (sheet->cell[counter][rowc]->attrib == FORMULA))
                spreadsheet_fix_formula(sheet, counter, rowc, ROWADD, row);
        }
    }
    sheet->changed = true;
}

// Delete row
void spreadsheet_delete_row(Spreadsheet *sheet, int row)
{
    int counter, rowc;

    for (counter = 0; counter <= sheet->lastcol; counter++)
        spreadsheet_delete_cell(sheet, counter, row);

    if (row != MAXROWS - 1) {
        for (counter = 0; counter < MAXCOLS; counter++) {
            memmove(&sheet->cell[counter][row], &sheet->cell[counter][row + 1],
                    sizeof(CELLPTR) * (MAXROWS - row - 1));
            memmove(&sheet->format[counter][row], &sheet->format[counter][row + 1],
                    MAXROWS - row - 1);
        }
    } else {
        for (counter = 0; counter <= sheet->lastcol; counter++) {
            sheet->cell[counter][MAXROWS - 1]   = NULL;
            sheet->format[counter][MAXROWS - 1] = DEFAULTFORMAT;
        }
    }

    if ((sheet->lastrow >= row) && (sheet->lastrow > 0))
        sheet->lastrow--;

    for (counter = 0; counter <= sheet->lastcol; counter++) {
        for (rowc = 0; rowc <= sheet->lastrow; rowc++) {
            if ((sheet->cell[counter][rowc] != NULL) &&
                (sheet->cell[counter][rowc]->attrib == FORMULA))
                spreadsheet_fix_formula(sheet, counter, rowc, ROWDEL, row);
        }
    }
    sheet->changed = true;
}

// Set column width
void spreadsheet_set_col_width(Spreadsheet *sheet, int col, int width)
{
    sheet->colwidth[col] = width;
    sheet->changed       = true;
}

// Format cells
void spreadsheet_format_cells(Spreadsheet *sheet, int col1, int row1, int col2, int row2,
                              int newformat)
{
    int col, row;

    for (col = col1; col <= col2; col++) {
        for (row = row1; row <= row2; row++) {
            sheet->format[col][row] = (sheet->format[col][row] & OVERWRITE) | newformat;
        }
    }
    sheet->changed = true;
}

// Load spreadsheet from file
int spreadsheet_load(Spreadsheet *sheet, const char *filename)
{
    int reallastcol = 0, reallastrow = 0;
    bool allocated;
    char line[1024];
    FILE *file;
    int curcol, currow, format_val;
    char type_str[16];
    char data_buffer[MAXINPUT + 1];
    double value = 0.0;
    int col;

    if (access(filename, F_OK))
        return -1; // File doesn't exist

    if ((file = fopen(filename, "r")) == NULL)
        return -2; // Can't open file

    // Read and validate header line
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return -3; // Not a valid spreadsheet file
    }
    // Remove newline if present
    line[strcspn(line, "\n\r")] = '\0';
    if (strcmp(line, MSGNAME) != 0) {
        fclose(file);
        return -3; // Not a valid spreadsheet file
    }

    // Clear current sheet
    for (int row = 0; row <= sheet->lastrow; row++) {
        for (int col = 0; col <= sheet->lastcol; col++)
            spreadsheet_delete_cell(sheet, col, row);
    }
    spreadsheet_init(sheet);

    // Read dimensions
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return -3;
    }
    sheet->lastcol = atoi(line);

    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return -3;
    }
    sheet->lastrow = atoi(line);

    // Read column widths (one per line)
    for (col = 0; col < MAXCOLS; col++) {
        if (fgets(line, sizeof(line), file) == NULL) {
            fclose(file);
            return -3;
        }
        sheet->colwidth[col] = atoi(line);
    }

    // Read cells (one per line)
    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove newline
        line[strcspn(line, "\n\r")] = '\0';

        // Skip empty lines
        if (line[0] == '\0')
            continue;

        // Parse cell line: col,row,format,type,data or col,row,format,type,data,value
        char *p = line;
        char *token;

        // Parse col
        token = p;
        while (*p && *p != ',')
            p++;
        if (*p == ',')
            *p++ = '\0';
        curcol = atoi(token);
        if (*p == '\0')
            break;

        // Parse row
        token = p;
        while (*p && *p != ',')
            p++;
        if (*p == ',')
            *p++ = '\0';
        currow = atoi(token);
        if (*p == '\0')
            break;

        // Parse format
        token = p;
        while (*p && *p != ',')
            p++;
        if (*p == ',')
            *p++ = '\0';
        format_val                    = atoi(token);
        sheet->format[curcol][currow] = format_val;
        if (*p == '\0')
            break;

        // Parse type
        token = p;
        while (*p && *p != ',')
            p++;
        if (*p == ',')
            *p++ = '\0';
        strncpy(type_str, token, sizeof(type_str) - 1);
        type_str[sizeof(type_str) - 1] = '\0';
        if (*p == '\0')
            break;

        // Parse data (may be quoted)
        if (*p == '"') {
            // Quoted field - handle escaping
            p++; // Skip opening quote
            int i = 0;
            while (*p && i < MAXINPUT) {
                if (*p == '"' && *(p + 1) == '"') {
                    // Escaped quote
                    data_buffer[i++] = '"';
                    p += 2;
                } else if (*p == '"') {
                    // Closing quote
                    p++;
                    break;
                } else {
                    data_buffer[i++] = *p++;
                }
            }
            data_buffer[i] = '\0';
            // Skip comma if present
            if (*p == ',')
                p++;
        } else {
            // Unquoted field - read until comma or end
            token = p;
            while (*p && *p != ',')
                p++;
            if (*p == ',')
                *p++ = '\0';
            strncpy(data_buffer, token, MAXINPUT);
            data_buffer[MAXINPUT] = '\0';
        }

        // For FORMULA, there's an additional value field
        if (strcmp(type_str, "FORMULA") == 0 && *p != '\0') {
            value = strtod(p, NULL);
        }

        // Create cell based on type
        if (strcmp(type_str, "TEXT") == 0) {
            allocated = spreadsheet_set_cell_text(sheet, curcol, currow, data_buffer);
        } else if (strcmp(type_str, "VALUE") == 0) {
            value     = strtod(data_buffer, NULL);
            allocated = spreadsheet_set_cell_value(sheet, curcol, currow, value);
        } else if (strcmp(type_str, "FORMULA") == 0) {
            allocated = spreadsheet_set_cell_formula(sheet, curcol, currow, data_buffer);
            if (allocated) {
                // Set the fvalue from file
                sheet->cell[curcol][currow]->v.f.fvalue = value;
            }
        } else {
            // Unknown type, skip this cell
            continue;
        }

        if (!allocated) {
            sheet->lastrow                = reallastrow;
            sheet->lastcol                = reallastcol;
            sheet->format[curcol][currow] = DEFAULTFORMAT;
            fclose(file);
            return -4; // Not enough memory
        } else {
            if (curcol > reallastcol)
                reallastcol = curcol;
            if (currow > reallastrow)
                reallastrow = currow;
        }
    }

    fclose(file);
    sheet->changed = false;
    return 0; // Success
}

// Save spreadsheet to file
int spreadsheet_save(Spreadsheet *sheet, const char *filename)
{
    int col, row;
    CELLPTR cellptr;
    FILE *file;
    const char *text_content;
    int needs_quote;

    if ((file = fopen(filename, "w")) == NULL)
        return -1; // Can't open file

    // Write header line with magic string
    fprintf(file, "%s\n", MSGNAME);

    // Write dimensions (one per line)
    fprintf(file, "%d\n", sheet->lastcol);
    fprintf(file, "%d\n", sheet->lastrow);

    // Write column widths (one per line)
    for (col = 0; col < MAXCOLS; col++) {
        fprintf(file, "%d\n", sheet->colwidth[col]);
    }

    // Write cells (one per line)
    for (row = 0; row <= sheet->lastrow; row++) {
        for (col = sheet->lastcol; col >= 0; col--) {
            if (sheet->cell[col][row] != NULL) {
                cellptr = sheet->cell[col][row];
                switch (cellptr->attrib) {
                case TEXT:
                    text_content = cellptr->v.text;
                    // Check if text contains comma or newline, needs quoting
                    needs_quote =
                        (strchr(text_content, ',') != NULL || strchr(text_content, '\n') != NULL ||
                         strchr(text_content, '"') != NULL);
                    if (needs_quote) {
                        // Escape quotes and wrap in quotes
                        fprintf(file, "%d,%d,%d,TEXT,\"", col, row, sheet->format[col][row]);
                        for (const char *p = text_content; *p; p++) {
                            if (*p == '"')
                                fputc('"', file); // Double quote for escape
                            fputc(*p, file);
                        }
                        fprintf(file, "\"\n");
                    } else {
                        fprintf(file, "%d,%d,%d,TEXT,%s\n", col, row, sheet->format[col][row],
                                text_content);
                    }
                    break;
                case VALUE:
                    fprintf(file, "%d,%d,%d,VALUE,%.*g\n", col, row, sheet->format[col][row],
                            MAXPLACES, cellptr->v.value);
                    break;
                case FORMULA:
                    text_content = cellptr->v.f.formula;
                    // Check if formula contains comma or newline, needs quoting
                    needs_quote =
                        (strchr(text_content, ',') != NULL || strchr(text_content, '\n') != NULL ||
                         strchr(text_content, '"') != NULL);
                    if (needs_quote) {
                        fprintf(file, "%d,%d,%d,FORMULA,\"", col, row, sheet->format[col][row]);
                        for (const char *p = text_content; *p; p++) {
                            if (*p == '"')
                                fputc('"', file); // Double quote for escape
                            fputc(*p, file);
                        }
                        fprintf(file, "\",%.*g\n", MAXPLACES, cellptr->v.f.fvalue);
                    } else {
                        fprintf(file, "%d,%d,%d,FORMULA,%s,%.*g\n", col, row,
                                sheet->format[col][row], text_content, MAXPLACES,
                                cellptr->v.f.fvalue);
                    }
                    break;
                }
            }
        }
    }

    fclose(file);
    sheet->changed = false;
    return 0; // Success
}

// Set last column
void spreadsheet_set_last_col(Spreadsheet *sheet)
{
    int row, col;

    for (col = sheet->lastcol; col >= 0; col--) {
        for (row = 0; row <= sheet->lastrow; row++) {
            if (sheet->cell[col][row] != NULL) {
                sheet->lastcol = col;
                return;
            }
        }
    }
    sheet->lastcol = 0;
}

// Set last row
void spreadsheet_set_last_row(Spreadsheet *sheet)
{
    int row, col;

    for (row = sheet->lastrow; row >= 0; row--) {
        for (col = 0; col <= sheet->lastcol; col++) {
            if (sheet->cell[col][row] != NULL) {
                sheet->lastrow = row;
                return;
            }
        }
    }
    sheet->lastrow = 0;
}

// Get last column
int spreadsheet_get_last_col(const Spreadsheet *sheet)
{
    return sheet->lastcol;
}

// Get last row
int spreadsheet_get_last_row(const Spreadsheet *sheet)
{
    return sheet->lastrow;
}

// Get memory left
long spreadsheet_get_memleft(const Spreadsheet *sheet)
{
    return sheet->memleft;
}

// Check if changed
bool spreadsheet_is_changed(const Spreadsheet *sheet)
{
    return sheet->changed;
}

// Set changed flag
void spreadsheet_set_changed(Spreadsheet *sheet, bool changed)
{
    sheet->changed = changed;
}

// Get autocalc
bool spreadsheet_get_autocalc(const Spreadsheet *sheet)
{
    return sheet->autocalc;
}

// Set autocalc
void spreadsheet_set_autocalc(Spreadsheet *sheet, bool autocalc)
{
    sheet->autocalc = autocalc;
}

// Get formdisplay
bool spreadsheet_get_formdisplay(const Spreadsheet *sheet)
{
    return sheet->formdisplay;
}

// Set formdisplay
void spreadsheet_set_formdisplay(Spreadsheet *sheet, bool formdisplay)
{
    sheet->formdisplay = formdisplay;
}

// Convert column number to string
char *spreadsheet_cell_to_string(int col, char *colstr)
{
    memset(colstr, 0, 3);
    if (col < 26)
        colstr[0] = col + 'A';
    else {
        colstr[0] = (col / 26) - 1 + 'A';
        colstr[1] = (col % 26) + 'A';
    }
    return colstr;
}

// Center column string
void spreadsheet_center_col_string(int col, int colwidth, char *colstr)
{
    char s[3];
    int spaces1, spaces2;

    spreadsheet_cell_to_string(col, s);
    spaces1 = (colwidth - strlen(s)) >> 1;
    spaces2 = colwidth - strlen(s) - spaces1;
    sprintf(colstr, "%*s%s%*s", spaces1, "", s, spaces2, "");
}

// Set overwrite flags
int spreadsheet_set_overwrite_flags(Spreadsheet *sheet, int col, int row)
{
    int len;
    CELLPTR cellptr = sheet->cell[col][row];

    if (cellptr == NULL || cellptr->attrib != TEXT)
        return col + 1;

    len = strlen(cellptr->v.text) - sheet->colwidth[col];
    while ((++col < MAXCOLS) && (len > 0) && (sheet->cell[col][row] == NULL)) {
        sheet->format[col][row] |= OVERWRITE;
        len -= sheet->colwidth[col];
    }
    return col;
}

// Clear overwrite flags
void spreadsheet_clear_overwrite_flags(Spreadsheet *sheet, int col, int row)
{
    while ((sheet->format[col][row] >= OVERWRITE) && (col < MAXCOLS) &&
           (sheet->cell[col][row] == NULL)) {
        sheet->format[col][row] &= ~OVERWRITE;
        col++;
    }
}

// Update overwrite flags
void spreadsheet_update_overwrite_flags(Spreadsheet *sheet, int col, int row)
{
    while ((sheet->cell[col][row] == NULL) && (col-- > 0))
        ;
    if ((sheet->cell[col][row] != NULL) && (sheet->cell[col][row]->attrib == TEXT) && (col >= 0))
        spreadsheet_set_overwrite_flags(sheet, col, row);
}

// Get cell display information
int spreadsheet_get_cell_display(Spreadsheet *sheet, int col, int row, CellDisplay *display,
                                 int formatting)
{
    static char s[81], temp[MAXCOLWIDTH + 1];
    CELLPTR cellptr = sheet->cell[col][row];
    int newcol, formatvalue;
    char *p;
    double value;

    if (cellptr == NULL) {
        if (!formatting || (sheet->format[col][row] < OVERWRITE)) {
            sprintf(s, "%*s", sheet->colwidth[col], "");
            display->color    = BLANKCOLOR;
            display->celltype = TEXT; // Empty cell
        } else {
            newcol = col;
            while (sheet->cell[--newcol][row] == NULL)
                ;
            p = sheet->cell[newcol][row]->v.text;
            while (newcol < col)
                p += sheet->colwidth[newcol++];
            strncpy(temp, p, sheet->colwidth[col]);
            temp[sheet->colwidth[col]] = 0;
            sprintf(s, "%s%*s", temp, (int)(sheet->colwidth[col] - strlen(temp)), "");
            display->color    = TEXTCOLOR;
            display->celltype = TEXT;
        }
    } else {
        formatvalue = sheet->format[col][row];
        switch (cellptr->attrib) {
        case TEXT: {
            char *just, *ljust = "%-*s", *rjust = "%*s";
            if ((formatvalue & RJUSTIFY) && formatting)
                just = rjust;
            else
                just = ljust;
            sprintf(s, just, sheet->colwidth[col], cellptr->v.text);
            if (formatting)
                s[sheet->colwidth[col]] = 0;
            display->color    = TEXTCOLOR;
            display->celltype = TEXT;
            break;
        }
        case FORMULA:
            if (sheet->formdisplay) {
                char *just, *ljust = "%-*s", *rjust = "%*s";
                if ((formatvalue & RJUSTIFY) && formatting)
                    just = rjust;
                else
                    just = ljust;
                sprintf(s, just, sheet->colwidth[col], cellptr->v.f.formula);
                if (formatting)
                    s[sheet->colwidth[col]] = 0;
                display->color    = FORMULACOLOR;
                display->celltype = FORMULA;
                break;
            } else
                value = cellptr->v.f.fvalue;
            // Fall through to VALUE case
        case VALUE:
            if (cellptr->attrib == VALUE)
                value = cellptr->v.value;

            if (value == HUGE_VAL) {
                strcpy(s, MSGERROR);
                display->color = ERRORCOLOR;
            } else {
                if (formatting) {
                    sprintf(s, "%1.*f", formatvalue & 15, value);
                    if (formatvalue & COMMAS) {
                        int pos = strcspn(s, ".");
                        while (pos > 3) {
                            pos -= 3;
                            if (s[pos - 1] != '-') {
                                memmove(&s[pos + 1], &s[pos], strlen(s) - pos + 1);
                                s[pos] = ',';
                            }
                        }
                    }
                    char fstring[4];
                    int width;
                    if (formatvalue & DOLLAR) {
                        if (s[0] == '-') {
                            strcpy(fstring, " $");
                            width = sheet->colwidth[col] - 2;
                        } else {
                            strcpy(fstring, " $ ");
                            width = sheet->colwidth[col] - 3;
                        }
                    } else {
                        fstring[0] = 0;
                        width      = sheet->colwidth[col];
                    }
                    char s_copy[81];
                    strcpy(s_copy, s);
                    if (formatvalue & RJUSTIFY) {
                        if ((int)strlen(s_copy) > width)
                            s_copy[width] = 0;
                        else
                            sprintf(s, "%*s", width, s_copy);
                    } else
                        sprintf(s, "%-*s", width, s_copy);
                    memmove(&s[strlen(fstring)], s, strlen(s) + 1);
                    memcpy(s, fstring, strlen(fstring));
                } else
                    sprintf(s, "%.*f", MAXPLACES, value);
                display->color = VALUECOLOR;
            }
            if (cellptr->attrib == FORMULA)
                display->celltype = FORMULA;
            else
                display->celltype = VALUE;
            break;
        }
    }

    display->text = s;
    return 0;
}
