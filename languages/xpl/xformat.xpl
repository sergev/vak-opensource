/*
**	XPL formatter and pretty printer
**
**	Author: Daniel Weaver
*/

declare ident(255) bit(1);      /* TRUE if valid <identifier> */
declare decode(255) bit(8);     /* Used to select character type in the scanner */
declare lower(255) bit(8);      /* Map Upper Case characters to Lower Case */
declare alpha(255) bit(8);      /* TRUE if upper or lower case alphabetic */
declare buffer character;       /* Input buffer */
declare scan_state fixed;       /* input_scanner state */
declare margin_chop fixed;      /* Margin control from dollar | */
declare line_count fixed;       /* Count the number of input lines */

declare cp fixed;

declare call_usage fixed;       /* TRUE if Usage() should be called */
declare control(255) bit(8);    /* Option control */
declare comment_multiple fixed;
declare tab_width fixed initial(8);
declare output_tab fixed initial(8);
declare indent_width fixed initial(3);
declare comment_space fixed initial(2);
declare comment_start fixed initial(0);
declare line_limit fixed initial(4096);
declare length_warning fixed initial(4096);

declare source_unit fixed;      /* Input unit number */
declare output_unit fixed;      /* Output unit number */

declare TAB literally '"(c)\t"';
declare x1 character initial(' ');
declare alphabet character initial('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_$@#');
declare upper_case character initial('ABCDEFGHIJKLMNOPQRSTUVWXYZ');
declare lower_case character initial('abcdefghijklmnopqrstuvwxyz');
declare x70 character initial(
    '                                                                      ');

/* Literally table */
declare LIT_MAX literally '512';
declare name(LIT_MAX) character;
declare lit fixed;
declare last_identifier character;      /* The previous <identifier> scanned */

declare word_count fixed;               /* Number of tokens in one line */
declare MAX_TOKENS literally '255';
declare blanks(MAX_TOKENS) fixed;       /* Number of blanks before the token */
declare text(MAX_TOKENS) character;     /* Token as a character string */
declare type(MAX_TOKENS) fixed;         /* Token type */
declare comment_column(MAX_TOKENS) fixed;  /* Column number for start of comment */

/* Indentation stacks */
declare previous_token fixed;           /* Previous token scanned */
declare thens(255) fixed;               /* Count the number of THEN tokens */
declare indents(255) fixed;             /* Count the number of indents */
declare if_then_else(255) fixed;        /* Indents for if-then-else statements */
declare nest_level fixed;               /* Nesting indent level */
declare next_indent fixed;              /* The number of columns to indent */
declare inside_statement fixed;         /* Inner statement indent */

declare double_wide(32) character initial('/*', '*/', '//', '||');
declare DOUBLE_WIDE_TOKEN literally '256';
declare COMMENT_START_TOKEN literally '256';
declare COMMENT_END_TOKEN literally '257';
declare COMMENT_LINE_TOKEN literally '258';
declare CONCATENATE_TOKEN literally '259';

declare TAB_TOKEN literally '512';
declare ELSE_TOKEN literally '513';
declare NUMBER_TOKEN literally '514';
declare IDENTIFIER_TOKEN literally '515';

declare state fixed;                    /* Formatter state */
declare NORMAL_STATE literally '0';
declare COMMENT_STATE literally '1';
declare LINE_COMMENT_STATE literally '2';
declare CHAR_STATE literally '3';
declare BITSTRING_STATE literally '4';

declare TRUE literally '1';
declare FALSE literally '0';

/*
**	Pad a STRING with blanks to WIDTH characters
*/
pad:
procedure(string, width) character;
    declare string character, (width, l) fixed;

    l = length(string);
    if l >= width then return string;
    do while width - l > length(x70);
        string = string || x70;
        l = l + length(x70);
    end;
    return string || substr(x70, 0, width - l);
end pad;


/*
**	Add blanks to the string
*/
white_space:
procedure(string, width) character;
    declare string character, width fixed;

    if width <= 0 then return string;
    do while width > length(x70);
        string = string || x70;
        width = width - length(x70);
    end;
    return string || substr(x70, 0, width);
end white_space;

/*
**	Find the byte b in the string
**
**	return the position in the string
**	return -1 if not found
*/
find_byte: procedure(string, b) fixed;
    declare string character;
    declare (b, i) fixed;

    do i = 0 to length(string) - 1;
        if byte(string, i) = b then return i;
    end;
    return - 1;
end find_byte;

/*
**	Convert to lower case
*/
go_lower:
procedure(s);
    declare s character;
    declare i fixed;

    /* The string is modified in place */
    do i = 0 to length(s) - 1;
        byte(s, i) = lower(byte(s, i));
    end;
end go_lower;

/*
**	Compare two strings.  Ignore case.
**
**	if a > b returns positive number;
**	if a < b returns negative number;
**	if a = b returns zero;
*/
case_compare:
procedure(s, t) fixed;
    declare (s, t) character;
    declare (i, a, b) fixed;

    a = length(s);
    b = length(t);
    if a ~= b then return a - b;
    do i = 0 to length(s) - 1;
        a = lower(byte(s, i));
        b = lower(byte(t, i));
        if a ~= b then return a - b;
    end;
    return 0;
end case_compare;

/*
**	Return TRUE if the identifier is in the symbol table
*/
normal:
procedure(ident) bit(8);
    declare ident character;
    declare i fixed;

    do i = 0 to lit - 1;
        if ident = name(i) then return 0;
    end;
    return 1;
end normal;

/*
**	Calculate the line length
*/
line_length: procedure fixed;
    declare (i, n) fixed;

    n = blanks(word_count);
    do i = 1 to word_count - 1;
        n = n + length(text(i)) + blanks(i);
    end;
    return n;
end line_length;

/*
**	Return the length of the line if the tabs were expanded.
*/
expand_length: procedure(s) fixed;
    declare s character;
    declare (len, i, j, ch) fixed;

    len = 0;
    do i = 0 to length(s) - 1;
        ch = byte(s, i);
        if ch = byte(TAB) then do;
                j = tab_width - (len mod tab_width);
                len = len + j;
            end;
        else len = len + 1;
    end;
    return len;
end expand_length;

/*
**	Scan a special character
*/
special_character: procedure;
    declare i fixed;

    i = 0;
    do while length(double_wide(i)) > 0;
        if substr(buffer, cp, 2) = double_wide(i) then do;
                text(word_count) = substr(buffer, cp, 2);
                type(word_count) = DOUBLE_WIDE_TOKEN + i;
                if i = COMMENT_LINE_TOKEN - DOUBLE_WIDE_TOKEN then do;
                        comment_column(word_count) = line_length;
                        if scan_state = NORMAL_STATE then
                            scan_state = LINE_COMMENT_STATE;
                    end;  else
                if i = COMMENT_START_TOKEN - DOUBLE_WIDE_TOKEN then do;
                        comment_column(word_count) = line_length;
                        if scan_state = NORMAL_STATE then
                            scan_state = COMMENT_STATE;
                    end;  else
                if i = COMMENT_END_TOKEN - DOUBLE_WIDE_TOKEN then do;
                        if scan_state = COMMENT_STATE then
                            scan_state = NORMAL_STATE;
                    end;
                word_count = word_count + 1;
                blanks(word_count) = 0;
                cp = cp + 2;
                return;
            end;
        i = i + 1;
    end;
    text(word_count) = substr(buffer, cp, 1);
    type(word_count) = byte(buffer, cp);
    word_count = word_count + 1;
    blanks(word_count) = 0;
    cp = cp + 1;
end special_character;

/*
**	Read an input line and scan it.
**	Extract all the blanks from the text.
*/
input_scanner:
procedure;
    /* Read a line and count all the blanks */
    declare line_size fixed;
    declare (i, j) fixed;

    if scan_state = LINE_COMMENT_STATE then
        scan_state = NORMAL_STATE;
    buffer = input(source_unit);
    line_count = line_count + 1;
    if margin_chop > 0 & margin_chop < length(buffer) then do;
            /* margin control from dollar | */
            buffer = substr(buffer, 0, margin_chop);
        end;
    line_size = length(buffer);
    cp = 0;
    word_count = 1;
    blanks(1) = 0;
    do while cp < line_size;
        do case decode(byte(buffer, cp));
            /* Case 0: Special characters */
            do;
                text(word_count) = substr(buffer, cp, 1);
                type(word_count) = byte(buffer, cp);
                if type(word_count) = byte('''') then do;
                        if scan_state = NORMAL_STATE then
                            scan_state = CHAR_STATE;
                        else
                        if scan_state = CHAR_STATE then
                            scan_state = NORMAL_STATE;
                    end;  else
                if type(word_count) = byte('"') then do;
                        if scan_state = NORMAL_STATE then
                            scan_state = BITSTRING_STATE;
                        else
                        if scan_state = BITSTRING_STATE then
                            scan_state = NORMAL_STATE;
                    end;
                word_count = word_count + 1;
                blanks(word_count) = 0;
                cp = cp + 1;
            end;
            /* Case 1: Double wide special characters */
            call special_character;
            /* Case 2: Spaces */
            do;
                do while byte(buffer, cp) = byte(' ');
                    blanks(word_count) = blanks(word_count) + 1;
                    cp = cp + 1;
                end;
            end;
            /* Case 3: TAB */
            if scan_state = NORMAL_STATE then do;
                    i = line_length;
                    j = tab_width - (i mod tab_width);
                    blanks(word_count) = blanks(word_count) + j;
                    cp = cp + 1;
                end;
            else do;
                    i = cp;
                    do while decode(byte(buffer, cp)) = 3;
                        cp = cp + 1;
                    end;
                    text(word_count) = substr(buffer, i, cp - i);
                    type(word_count) = TAB_TOKEN;
                    word_count = word_count + 1;
                    blanks(word_count) = 0;
                end;
            /* Case 4: Numbers */
            do;
                i = cp;
                do while decode(byte(buffer, cp)) = 4;
                    cp = cp + 1;
                end;
                text(word_count) = substr(buffer, i, cp - i);
                type(word_count) = NUMBER_TOKEN;
                word_count = word_count + 1;
                blanks(word_count) = 0;
            end;
            /* Case 5: Identifier */
            do;
                i = cp;
                do while ident(byte(buffer, cp));
                    cp = cp + 1;
                end;
                text(word_count) = substr(buffer, i, cp - i);
                type(word_count) = IDENTIFIER_TOKEN;
                if scan_state = NORMAL_STATE then
                    do;
                        if case_compare(text(word_count), 'literally') = 0 then do;
                                name(lit) = last_identifier;
                                lit = lit + 1;
                            end;
                        last_identifier = text(word_count);
                    end;
                if byte(text(word_count)) = byte('$')
                    & byte(text(word_count), 1) = byte('F') then
                    control(byte('F')) = ~control(byte('F'));
                i = byte(text(word_count), length(text(word_count)) - 1);
                word_count = word_count + 1;
                blanks(word_count) = 0;
                if i = byte('$') then
                    if byte(buffer, cp) = byte('|') then
                        do;
                            control(byte('|')) = ~control(byte('|'));
                            if control(byte('|')) then
                                margin_chop = line_length;
                            else margin_chop = 0;
                        end;
            end;
        end;
    end;
    text(word_count) = '';
    type(word_count) = 0;
end input_scanner;

/*
**	Initialize the scanner tables
*/
initialize:
procedure;
    declare i fixed;

    do i = 0 to 255;
        ident(i) = 0;
        lower(i) = i;
        decode(i) = 0;
    end;
    i = 0;
    do while length(double_wide(i)) > 0;
        decode(byte(double_wide(i))) = 1;
        i = i + 1;
    end;
    do i = 0 to length(upper_case) - 1;
        lower(byte(upper_case, i)) = byte(lower_case, i);
    end;
    decode(byte(' ')) = 2;
    decode(byte(TAB)) = 3;
    do i = byte('0') to byte('9');
        decode(i) = 4;
        ident(i) = 1;
    end;
    do i = 0 to length(alphabet) - 1;
        decode(byte(alphabet, i)) = 5;
        ident(byte(alphabet, i)) = 1;
    end;
end initialize;

/*
**	Align the start of a comment
**
**	Implements: -C, -c<number>, -m<number> and -n<number>
*/
align_comments: procedure(ll, n);
    declare (ll, n, b) fixed;

    if n = 1 & comment_start > 0 then do;
            if blanks(1) < comment_start then
                blanks(1) = comment_start;
        end;  else
    if control(byte('C')) then do;
            /* Comments have a Fixed position */
            if ll >= comment_column(n) then blanks(n) = 0;
            else blanks(n) = comment_column(n) - ll;
        end;  else
    if control(byte('c')) then do;
            b = (ll + blanks(n)) mod comment_multiple;
            if b * 2 > comment_multiple then do;
                    /* Shift the comment to the right */
                    blanks(n) = blanks(n) + comment_multiple - b;
                end;  else
            if blanks(n) > b then do;
                    /* Shift the comment to the left */
                    blanks(n) = blanks(n) - b;
                end;
        end;
    if blanks(n) < comment_space then do;
            if n > 1 then blanks(n) = comment_space;
        end;
end align_comments;

/*
**	Upper/Lower case converter for identifiers
*/
process_identifier: procedure(n);
    declare n fixed;
    declare left_type fixed;

    /* Convert to lower case. */
    if state = CHAR_STATE then do;
            if control(byte('s')) then do;
                    /* Convert everything to lower case */
                    call go_lower(text(n));
                    return;
                end;
            if control(byte('S')) then do;  /* Smart lower case */
                    /* Convert most stuff to lower case */
                    left_type = type(n - 1);
                    if left_type = byte('''') |
                        left_type = byte('.') then do;
                            call go_lower(substr(text(n), 1));
                            return;
                        end;
                    call go_lower(text(n));
                end;
            return;
        end;
    if state = NORMAL_STATE then do;
            if control(byte('L')) then do;
                    /* If it's not a literal it gets converted to lower case */
                    if normal(text(n)) then
                        call go_lower(text(n));
                end;
            return;
        end;
    if state = COMMENT_STATE | state = LINE_COMMENT_STATE then do;
            if control(byte('L')) then do;
                    /* Do not convert <dollar><letter> options */
                    if byte(text(n)) = byte('$') & length(text(n)) = 2 then do;
                            return;
                        end;
                    /* Convert most stuff to lower case */
                    left_type = type(n - 1);
                    if left_type = COMMENT_START_TOKEN |
                        left_type = COMMENT_LINE_TOKEN |
                        left_type = byte('.') then do;
                            call go_lower(substr(text(n), 1));
                            return;
                        end;
                    call go_lower(text(n));
                end;
            return;
        end;
end process_identifier;

/*
**	Track if then else blocks to get proper indent
*/
if_then_else_stack: procedure(n);
    declare n fixed;

    if state = COMMENT_STATE | state = LINE_COMMENT_STATE then do;
            /* Comment start tokens come here */
            return;
        end;
    if type(n) = IDENTIFIER_TOKEN then do;
            if case_compare(text(n), 'then') = 0 then do;
                    if_then_else(nest_level) = thens(nest_level) + 1;
                    thens(nest_level) = thens(nest_level) + 1;
                    next_indent = indents(nest_level) + if_then_else(nest_level);
                    inside_statement = 0;
                    previous_token = 0;
                    return;
                end;
            if previous_token = byte(';') then do;
                    if case_compare(text(n), 'else') = 0 then do;
                            if thens(nest_level) > 0 then do;
                                    if_then_else(nest_level) = thens(nest_level);
                                    thens(nest_level) = thens(nest_level) - 1;
                                end;
                            else if_then_else(nest_level) = 0;
                            next_indent = indents(nest_level) + thens(nest_level);
                            inside_statement = 0;
                            previous_token = ELSE_TOKEN;
                            return;
                        end;
                end;
            if previous_token = ELSE_TOKEN then do;
                    if case_compare(text(n), 'if') = 0 then do;
                            next_indent = indents(nest_level) + thens(nest_level);
                            inside_statement = 0;
                        end;
                    previous_token = 0;
                    return;
                end;
        end;
    if previous_token = byte(';') then do;
            if_then_else(nest_level) = 0;
            if thens(nest_level) > 0 then do;
                    thens(nest_level) = 0;
                    next_indent = indents(nest_level);
                    inside_statement = 1;
                end;
        end;
    previous_token = type(n);
end if_then_else_stack;

/*
**	Handle spacing for special characters
**	This is the function that does all the formatting
*/
normal_spacing: procedure(n);
    declare (n, i, left, right, tyn) fixed;
    declare text_op(1) character initial('xor', 'mod');

    call if_then_else_stack(n);
    left = type(n - 1);
    right = type(n + 1);
    tyn = type(n);
    if tyn = byte(';') then do;
            blanks(n) = 0;
            if blanks(n + 1) < 2 then blanks(n + 1) = 2;
            if_then_else(nest_level) = 0;           /* Close the IF statement */
            inside_statement = 0;
            return;
        end;
    if tyn = byte(',') then do;
            blanks(n) = 0;
            if blanks(n + 1) < 1 then blanks(n + 1) = 1;
            return;
        end;
    if tyn = byte(')') then do;
            blanks(n) = 0;
            return;
        end;
    if tyn = byte('(') then do;
            declare left_paren(6) character initial('declare', 'to', 'by',
                'if', 'case', 'while', 'return');

            blanks(n + 1) = 0;
            if left = IDENTIFIER_TOKEN then do;
                    do i = 0 to 6;
                        if case_compare(text(n - 1), left_paren(i)) = 0 then
                            return;
                    end;
                    blanks(n) = 0;
                end;
            return;
        end;
    if tyn = byte('=') then do;
            blanks(n + 1) = 1;
            blanks(n) = 1;
            if find_byte('~^<>', left) >= 0 then blanks(n) = 0;
            return;
        end;
    if tyn = byte(':') then do;
            blanks(n) = 0;
            if blanks(n + 1) < 1 then blanks(n + 1) = 1;
            inside_statement = 0;
            next_indent = indents(nest_level) + if_then_else(nest_level);
            return;
        end;
    if tyn = byte('-') &
        (right = NUMBER_TOKEN | right = IDENTIFIER_TOKEN) then do;
            /* Unary minus sign (-) */
            if find_byte(',=+-*/<>&|', left) >= 0 then do;
                    blanks(n) = 1;
                    blanks(n + 1) = 0;
                    return;
                end;
            if left = CONCATENATE_TOKEN then do;
                    blanks(n) = 1;
                    blanks(n + 1) = 0;
                    return;
                end;
            if left = byte('(') then do;
                    blanks(n) = 0;
                    blanks(n + 1) = 0;
                    return;
                end;
            if left = IDENTIFIER_TOKEN then do;
                    do i = 0 to 1;
                        if case_compare(text(n - 1), text_op(i)) = 0 then do;
                                blanks(n) = 1;
                                blanks(n + 1) = 0;
                                return;
                            end;
                    end;
                end;
        end;
    if find_byte('|&+-*/', tyn) >= 0 then do;
            blanks(n) = 1;
            blanks(n + 1) = 1;
            return;
        end;
    if find_byte('<>', tyn) >= 0 then do;
            if type(n + 1) ~= byte('=') then blanks(n + 1) = 1;
            blanks(n) = 1;
            if find_byte('~^', left) >= 0 then blanks(n) = 0;
            return;
        end;
    if find_byte('~^', tyn) >= 0 then do;
            if left = byte('(') then blanks(n) = 0;
            else blanks(n) = 1;
            if find_byte('<>=', right) >= 0 then blanks(n + 1) = 0;
            if right = NUMBER_TOKEN | right = IDENTIFIER_TOKEN then
                blanks(n + 1) = 0;
            return;
        end;
    if tyn = CONCATENATE_TOKEN then do;
            blanks(n) = 1;
            blanks(n + 1) = 1;
            return;
        end;
    if tyn = IDENTIFIER_TOKEN then do;
            if right = byte(':') then do;
                    /* Make the label stick out */
                    if next_indent > 0 then next_indent = next_indent - 1;
                end;
            if case_compare(text(n), 'procedure') = 0 then do;
                    indents(nest_level + 1) = indents(nest_level) + 1;
                    nest_level = nest_level + 1;
                    if_then_else(nest_level), thens(nest_level) = 0;
                end;  else
            if case_compare(text(n), 'do') = 0 then do;
                    inside_statement = 0;
                    next_indent = indents(nest_level) + if_then_else(nest_level);
                    nest_level = nest_level + 1;
                    indents(nest_level) = next_indent + 1;
                    if_then_else(nest_level), thens(nest_level) = 0;
                end;  else
            if case_compare(text(n), 'end') = 0 then do;
                    if nest_level > 0 then nest_level = nest_level - 1;
                    inside_statement = 0;
                    next_indent = indents(nest_level) + if_then_else(nest_level);
                    if blanks(n + 1) > 1 then blanks(n + 1) = 1;
                end;  else
            if case_compare(text(n), 'then') = 0 then do;
                    if blanks(n) = 0 then blanks(n) = 1;
                end;  else
                do i = 0 to 1;
                    if case_compare(text(n), text_op(i)) = 0 then do;
                            blanks(n) = 1;
                            blanks(n + 1) = 1;
                            return;
                        end;
                end;
        end;
end normal_spacing;

/*
**	Main loop.
*/
process: procedure;
    declare (i, j, indent_state) fixed;
    declare line character;

    call initialize;
    type(0) = 0;
    nest_level = 0;
    indents(0) = 0;
    scan_state = NORMAL_STATE;
    call input_scanner;
    state = NORMAL_STATE;
    do while word_count + blanks(1) > 1;
        line = '';
        next_indent = indents(nest_level) + if_then_else(nest_level)
            + inside_statement;
        indent_state = state;
        do i = 1 to word_count - 1;
            if state = NORMAL_STATE then do;
                    if type(i) = COMMENT_START_TOKEN then
                        state = COMMENT_STATE;
                    else
                    if type(i) = COMMENT_LINE_TOKEN then
                        state = LINE_COMMENT_STATE;
                    else
                    if type(i) = byte('''') then
                        state = CHAR_STATE;
                    else
                    if type(i) = byte('"') then
                        state = BITSTRING_STATE;
                    else do;
                            inside_statement = 1;
                        end;
                    if control(byte('F')) = 0 then
                        do;
                            call normal_spacing(i);
                            if i = 1 then
                                do;
                                    blanks(1) = next_indent * indent_width;
                                end;
                            if type(i) = COMMENT_START_TOKEN |
                                type(i) = COMMENT_LINE_TOKEN then do;
                                    call align_comments(length(line), i);
                                end;
                            next_indent = indents(nest_level)
                                + if_then_else(nest_level) + inside_statement;
                        end;
                end;  else
            if state = COMMENT_STATE then do;
                    if type(i) = COMMENT_END_TOKEN then
                        state = NORMAL_STATE;
                end;  else
            if state = CHAR_STATE then do;
                    if type(i) = byte('''') then
                        state = NORMAL_STATE;
                end;  else
            if state = BITSTRING_STATE then do;
                    if type(i) = byte('"') then
                        state = NORMAL_STATE;
                end;
            if type(i) = IDENTIFIER_TOKEN & control(byte('F')) = 0 then
                call process_identifier(i);
            line = white_space(line, blanks(i)) || text(i);
        end;
        if state = CHAR_STATE | state = BITSTRING_STATE then do;
                line = white_space(line, blanks(word_count));
            end;
        j = expand_length(line);
        if j > line_limit & indent_state = NORMAL_STATE then
            do;
                i = 0;
                do while i < length(line) & byte(line, i) = byte(' ');
                    i = i + 1;
                end;
                if j - line_limit < i then
                    line = substr(line, j - line_limit);
                else line = substr(line, i);
                j = expand_length(line);
            end;
        if j > length_warning then do;
                output(1) = line_count || '  ' || line;
                output(1) = 'Text exceeds warning threshold.';
            end;
        if output_tab > 1 & indent_state = NORMAL_STATE then do;
                declare tabs character;

                /* Convert leading blanks to tabs */
                i, j = 0;
                tabs = '';
                do while i < length(line) & byte(line, i) = byte(' ');
                    i = i + 1;
                    j = j + 1;
                    if j >= output_tab then do;
                            tabs = tabs || TAB;
                            line = substr(line, i);
                            i, j = 0;
                        end;
                end;
                line = tabs || line;
            end;
        output(output_unit) = line;
        if state = LINE_COMMENT_STATE then
            state = NORMAL_STATE;
        call input_scanner;
    end;
end process;

/*
**	Get the numeric value of an option
*/
numeric_option:
procedure(opt) fixed;
    declare opt character;
    declare (i, n, ch) fixed;

    n = 0;
    do i = 0 to length(opt) - 1;
        ch = byte(opt, i);
        if ch >= byte('0') & ch <= byte('9') then
            n = n * 10 + (ch - byte('0'));
        else return n;
        cp = cp + 1;
    end;
    return n;
end numeric_option;

/*
**	Print a brief description of how to use the program
*/
usage:
procedure;
    output(1) = 'Usage: ' || argv(0) || ' [-options] [-o <out-file>] <source-file>';
    output(1) = '   -o <out-file>    Output filename for formatted text';
    output(1) = '   <source-file>    Input file';
    output(1) = 'Options:';
    output(1) = '   C          Do not realign comments';
    output(1) = '   L          Translate to lower case';
    output(1) = '   S          Smart strings lower case conversion';
    output(1) = '   s          Convert all strings to lower case';
    output(1) = '   c<number>  Comment indent multiple (0->indent width)';
    output(1) = '   i<number>  Indent width (default 3)';
    output(1) = '   l<number>  Line limit (default 4096)';
    output(1) = '   m<number>  Minimum number of spaces before a comment not in column one (default 2)';
    output(1) = '   n<number>  Minimum number of spaces before a comment that starts a line (default 0)';
    output(1) = '   t<number>  Input TAB width (default 8)';
    output(1) = '   w<number>  Print a warning if the line exceeds this length (default 4096)';
    output(1) = '   T<number>  Output TAB width (default 8).  Use zero for no output TABs';
    output(1) = 'Control toggles in the target program:';
    output(1) = '   F          Disable formatting';
end usage;

/* Execution starts here */

declare (i, j) fixed;

do i = 0 to length(upper_case) - 1;
    alpha(byte(upper_case, i)) = 1;
    alpha(byte(lower_case, i)) = 1;
end;

source_unit = 0;
output_unit = 0;
do i = 1 to argc - 1;
    if byte(argv(i)) ~= byte('-') then do;
            source_unit = xfopen(argv(i), 'r');
            if source_unit < 0 then do;
                    output(1) = 'Open file error: ' || argv(i);
                    call_usage = TRUE;
                end;
        end;
    else
    if argv(i) = '-o' & i + 1 < argc then do;
            i = i + 1;
            output_unit = xfopen(argv(i), 'w');
            if output_unit < 0 then do;
                    output(1) = 'Open file error: ' || argv(i);
                    call_usage = TRUE;
                end;
        end;
    else
        do cp = 1 to length(argv(i)) - 1;
            j = byte(argv(i), cp);
            if j = byte('c') then do;
                    control(j) = 1;
                    comment_multiple = numeric_option(substr(argv(i), cp + 1));
                end;  else
            if j = byte('i') then do;
                    indent_width = numeric_option(substr(argv(i), cp + 1));
                end;  else
            if j = byte('l') then do;
                    line_limit = numeric_option(substr(argv(i), cp + 1));
                end;  else
            if j = byte('m') then do;
                    comment_space = numeric_option(substr(argv(i), cp + 1));
                end;  else
            if j = byte('n') then do;
                    comment_start = numeric_option(substr(argv(i), cp + 1));
                end;  else
            if j = byte('t') then do;
                    tab_width = numeric_option(substr(argv(i), cp + 1));
                    if tab_width <= 0 then tab_width = 1;
                end;  else
            if j = byte('w') then do;
                    length_warning = numeric_option(substr(argv(i), cp + 1));
                end;  else
            if j = byte('T') then do;
                    output_tab = numeric_option(substr(argv(i), cp + 1));
                end;  else
            if alpha(j) then control(j) = 1;
            else call_usage = TRUE;
        end;
end;

if call_usage then do;
        call usage;
        return 1;
    end;

if comment_multiple = 0 then do;
        comment_multiple = indent_width;
        if comment_multiple = 0 then control(byte('c')) = 0;
    end;

call process;

return 0;

eof
