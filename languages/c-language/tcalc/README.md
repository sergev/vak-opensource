# TCALC - Terminal Spreadsheet Application

TCALC is a simple spreadsheet application with a text-based user interface (TUI). This version has been converted from the original Borland C++ 2.0 DOS implementation to use the ncurses library for Unix/Linux compatibility.

## Features

- Text-based spreadsheet with 100x100 cell grid (maximum 702 columns supported)
- Support for text, numeric values, and formulas
- Cell formatting options (right-justify, dollar format, commas, decimal places)
- Column and row insertion/deletion
- Auto-calculation mode
- Formula display mode
- Save and load spreadsheet files
- Print to file or printer

## Requirements

- A Unix-like operating system (Linux, macOS, BSD, etc.)
- GCC compiler (or compatible C compiler)
- ncurses library development files

### Installing ncurses

**Ubuntu/Debian:**
```bash
sudo apt-get install libncurses5-dev libncursesw5-dev
```

**Fedora/RHEL/CentOS:**
```bash
sudo dnf install ncurses-devel
```

**macOS (using Homebrew):**
```bash
brew install ncurses
```

**FreeBSD:**
```bash
sudo pkg install ncurses
```

## Building

Simply run `make` in the project directory:

```bash
make
```

This will compile all source files and create the `tcalc` executable.

## Usage

### Running the Program

```bash
./tcalc
```

Or to load a spreadsheet file:

```bash
./tcalc filename.tcalc
```

### Keyboard Controls

#### Navigation
- **Arrow Keys**: Move cursor
- **Home**: Go to cell A1
- **End**: Go to last used cell
- **Page Up/Down**: Scroll up/down 20 rows
- **Ctrl+Left/Right**: Scroll left/right one screen

#### Editing
- **Any printable character**: Start entering data in current cell
- **F2**: Edit current cell
- **Delete**: Clear current cell
- **Enter**: Confirm input
- **Escape**: Cancel input

#### Commands (Press `/` for menu)
- **S**: Spreadsheet menu (Load, Save, Print, Clear)
- **F**: Format cells
- **D**: Delete current cell
- **G**: Goto cell
- **C**: Column menu (Insert, Delete, Width)
- **R**: Row menu (Insert, Delete)
- **E**: Edit current cell
- **U**: Utility menu (Recalc, Formula display)
- **A**: Toggle AutoCalc
- **Q**: Quit

#### Function Keys
- **F1**: Recalculate all formulas
- **F2**: Edit current cell

### Cell Input

Formulas are entered as text. TCALC will automatically determine if what you entered is a legal formula or text.

- **Text**: Just type the text
- **Numbers**: Type the number (e.g., `123.45`)
- **Formulas**: Start with a number or cell reference

#### Formula Examples

Basic arithmetic:
- `A1+A2` - Add two cells
- `B6^5` - Raise cell B6 to the 5th power
- `A1*B2-C3` - Multiply A1 by B2, then subtract C3

Range summation:
- `A1:A10` - Sums all cells from A1 to A10 and puts the result in the current cell

Nested functions:
- `TRUNC(A1)` - Return the whole part of cell A1
- `SQRT(SQR(34.5))` - Square root of the square of 34.5
- `ABS(TRUNC(B16))` - Absolute value of the truncated value in B16

### Supported Functions

The available TCALC functions are:

- **ABS** - absolute value
- **ACOS** - arc cosine
- **ASIN** - arc sine
- **ATAN** - arc tangent
- **COS** - cosine
- **COSH** - hyperbolic cosine
- **EXP** - exponential function
- **LOG** - logarithm
- **LOG10** - base 10 logarithm
- **POW10** - raise argument to the 10th power
- **ROUND** - round to the nearest whole number
- **SIN** - sine
- **SINH** - hyperbolic sine
- **SQR** - square
- **SQRT** - square root
- **TAN** - tangent
- **TANH** - hyperbolic tangent
- **TRUNC** - return the whole part of a number

### Cell References

Cell names in formulas are typed with the column followed by the row.

- **Single cell**: `A1`, `B5`, `Z10`
- **Two-letter columns**: `AA1`, `AB5`, etc.
- **Ranges**: `A1:A10` - sums all cells from A1 to A10 (put a colon between the first and last cell in the group)

## File Format

TCALC saves spreadsheets in a binary format. Files can be saved and loaded using the Spreadsheet menu (press `/` then `S`).

## Technical Details

### Parser Grammar

The formula parser in TCALC uses a grammar based on the UNIX YACC utility. The parser grammar defines the structure of valid formulas:

```
%token CONST CELL FUNC
%%
e : e '+' t
  | e '-' t
  | t
  ;
t : t '*' f
  | t '/' f
  | f
  ;
f : x '^' f
  | x
  ;
x : '-' u
  | u
  ;
u : CELL ':' CELL
  | o
  ;
o : CELL
  | '(' e ')'
  | CONST
  | FUNC '(' e ')'
  ;
%%
```

This grammar defines:
- **e** (expression): Handles addition and subtraction operations
- **t** (term): Handles multiplication and division operations
- **f** (factor): Handles exponentiation (right-associative)
- **x**: Handles unary minus (negation)
- **u**: Handles cell ranges (e.g., `A1:A10`)
- **o** (operand): Handles cells, constants, parentheses, and function calls

The parser ensures proper operator precedence: exponentiation has the highest precedence, followed by multiplication/division, then addition/subtraction.

## Limitations

- Maximum 100 rows and 100 columns (can be extended to 702 columns in code)
- Screen size: 80 columns by 25 rows
- Memory limited by available system memory

## Troubleshooting

### Compilation Errors

If you get errors about missing ncurses:
- Make sure ncurses development files are installed
- On some systems, you may need to use `-lncursesw` instead of `-lncurses` in the Makefile

### Runtime Issues

- Make sure your terminal supports at least 80x25 characters
- If colors don't display correctly, your terminal may not support colors
- Some terminals may not support all function keys

## License

Original code: Copyright 1987-1991 by Borland International
This converted version is provided as-is for educational purposes.

## Credits

Original implementation: Borland C++ 2.0 demonstration program
Conversion to ncurses: Modernized for Unix/Linux compatibility

### Historical Note

The original DOS version was compiled using Borland's TC.EXE (Turbo C IDE) or TCC.EXE (command-line compiler). The project file was `TCALC.PRJ`. This modern version uses standard Unix build tools (make, GCC) and the ncurses library for cross-platform compatibility.
