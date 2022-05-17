/*  ~$F large free string space
    A Text Compression Program.

    Author -- Charles Wetherell  18 August 1976.
    Last date modified -- 25 August 1976.

    This program compresses text files using the Mayne-James dictionary
    construction algorithm. Input is an arbitrary text file and output
    is the compressed file along with an encoding dictionary.  Some
    statistics on program efficiency and compression rate are kept.
    Two passes by the source file are necessary. On the first pass the
    dictionary is built and a record of all characters seen is kept.
    Between passes the character record is used to add encodings to the
    dictionary entries. During the second pass, long high frequency
    strings are replaced by shorter encoding strings as the compressed
    file is written. Further information about the technique can be
    found in chapter 11 of

        Wetherell, C.S.  Etudes For Programmers.  Prentice-Hall,
            Englewood Cliffs, NJ. 1978.

    In this version of the algorithm, ends of input lines stop string
    matches during dictionary construction and text encoding; the
    carriage return is not treated like a character.  Only characters
    whose internal representations lie in the range 1 to 127 will be
    considered for encoding pairs so that the pairs will have
    reasonable print representations. In a full working implementation
    all 256 available characters would be used for encoding.

    The dictionary search, entry, and cleanup routines are written
    so that they may be changed quite easily.
    The algorithms can be modified by replacing the bodies of procedures
    search_dictionary, clean_dictionary, and build_entry, along with
    making any necessary changes to prepare_the_program.  The various
    thresholds parameters are all calculated by functions and can be
    be modifed by changing the function definitions.
    If there is a data structure added for searching, make sure that
    build_encoding_table leaves the structure in good shape after codes
    are added and entries of length two and less are deleted.

    This version uses simple linear search, a hyperbolic threshold
    for coalescence, a mean threshold for deletion, and an initial
    count of one for coalesced entries.
*/

/* Some macros to improve XPL as a language.                            */

declare logical literally 'bit(1)',
        true    literally '1',
        false   literally '0',
        not     literally '~';      /* to improve printing */

/* Declarations for I/O units                                           */

declare source_file fixed,
        echo_file   fixed,
        print literally 'output(echo_file) =';

/* Declarations for the input routine                                   */

declare input_buffer character,
        (print_source, check_characters) logical,
        character_used("ff") logical;   /* i.e. 256 different entries   */

/* Declarations for the dictionary                                      */

declare dictionary_size literally '100',
        dictionary_string(dictionary_size) character,
        dictionary_count(dictionary_size) fixed,
        dictionary_code(dictionary_size) character,
        dictionary_usage(dictionary_size) fixed,
        dictionary_top fixed;

/* Control for encoding print.                                          */

declare print_encoding logical;

/* Declarations for encoding statistics */

declare search_compares fixed,
        build_compares fixed,
        compress_compares fixed;

declare time_check(10) fixed;

declare (input_length, output_length) fixed;

compute_time: procedure fixed;

    /* Return the time in nanoseconds,
       obtained via clock_gettime() system call.
     */

    call inline('#include <time.h>');
    call inline('struct timespec tv;');
    call inline('clock_gettime(CLOCK_REALTIME, &tv);');
    return inline('tv.tv_sec * 1000000000 + tv.tv_nsec');

end compute_time;

i_format: procedure(number, width) character;

    /* Function i_format converts its argument number into a string
       and then pads the string on the left to bring the length up
       to width characters. All of this is just the FORTRAN
       integer format.
    */

    declare number fixed,
            width fixed;

    declare string character,
            blanks character initial ('                     ');

    string = number;
    if length(string) < width then
        string = substr(blanks, 0, width - length(string)) || string;
    return string;

end i_format;

prepare_the_program: procedure;

    /* Only simple clearing of the dictionary, the character record,
       the statistics, and a few scalars is required.
    */

    declare i fixed;

    do i = 0 to dictionary_size;
        dictionary_string(i), dictionary_code(i) = '';
        dictionary_count(i), dictionary_usage(i) = 0;
    end;
    dictionary_top = -1;

    do i = 0 to "ff"; character_used(i) = false; end;
    input_buffer = '';

    search_compares = 0;
    input_length, output_length = 0;

end prepare_the_program;

fill_input_buffer: procedure;

    /* If input_buffer is empty, then this routine tries to read a
       line from the source_file. The line read goes into
       input_buffer with a null line the signal for end of file. If
       flag print_source is on, then the input is echoed. If flag
       check_characters is on, the input is scanned for character
       usage.
    */

    declare i fixed;

    if length(input_buffer) > 0 then return;
    input_buffer = input(source_file);
    if print_source then print input_buffer;
    if check_characters then
        do i = 0 to length(input_buffer)-1;
            character_used(byte(input_buffer,i)) = true;
        end;

end fill_input_buffer;

coalescence_threshold: procedure fixed;

    /* This procedure calculates the threshold for coalescing
       two dictionary entries into one. Here, the requirement is
       that the entries have frequencies greater than the reciprocal
       of the ratio of space remaining in the dictionary.
    */

    return dictionary_size/(dictionary_size-dictionary_top+1) + 1;

end coalescence_threshold;

deletion_threshold: procedure fixed;

    /* This function returns the threshold necessary for an entry
       to be retained in the dictionary at cleanup time.
       In this version, the frequency must be greater than the
       rounded up mean frequency.
    */

    declare sum fixed,
            i fixed;

    sum = 0;
    do i = 0 to dictionary_top;
        sum = sum + dictionary_count(i);
    end;
    return sum/(dictionary_top+1) + 1;

end deletion_threshold;

first_count: procedure fixed;

    /* This function returns the count given a coalesced entry when
       it is first entered in the dictionary.
    */

    return 1; /* currently give a count of 1. */

end first_count;

search_dictionary: procedure(test_string) fixed;

    /* This function searches the dictionary for the longest match
       with the head of the argument test_string.   If no match is
       found, the routine returns -1 as value; if a match is found,
       the index of the match is returned as value.

       This routine performs a simple linear search of the dictionary
       from the zeroth entry to the entry dictionary_top. If an
       entry's length is longer than the longest current match and
       still no longer than the argument, then the entry is matched
       against the argument.  Equality will cause the match to be
       updated. Notice that by starting the index at -1, the return
       value will be proper even if no match is found.
    */

    declare test_string character;

    declare index fixed,
            (match_length, arg_length, entry_length) fixed,
            i fixed;

    index = -1;
    match_length = 0;
    arg_length = length(test_string);

    do i = 0 to dictionary_top;
        entry_length = length(dictionary_string(i));
        if entry_length > match_length
              & entry_length <= arg_length then
            if dictionary_string(i)
                  = substr(test_string,0,entry_length) then
                do;
                    index = i;
                    match_length = entry_length;
                end;
    end;
    search_compares = search_compares + dictionary_top + 1;
    return index;

end search_dictionary;

clean_dictionary: procedure;

    /* Clean_dictionary eliminates at least one low frequency entry
       from the dictionary and restores the smaller dictionary to
       the format it had before cleaning.
       The while loop surrounding the body of the procedure guarantees
       that at least one entry is deleted from the dictionary before
       return. If the initial threshold is not high enough to delete
       an entry, the threshold is incremented until something is
       deleted.

       The dictionary is just a linear table with no structure so
       entries can be deleted by pushing the retained entries toward
       the zero end of the table overwriting the removed entries.
    */

    declare i fixed,
            threshold fixed,
            old_top fixed,
            new_top fixed;

    old_top = dictionary_top;
    threshold = deletion_threshold;
    do while old_top = dictionary_top;
        new_top = -1;
        do i = 0 to dictionary_top;
            if dictionary_count(i) >= threshold then
                do;
                    new_top = new_top + 1;
                    dictionary_string(new_top) = dictionary_string(i);
                    dictionary_count(new_top) = dictionary_count(i);
                end;
        end;
        dictionary_top = new_top;
        threshold = threshold + 1;
    end;

end clean_dictionary;

build_entry: procedure(entry_string) fixed;

    /* Build_entry adds entry_string to the dictionary with a count
       of zero and returns as value the index of the new entry.

       Because the dictionary is searched linearly, the new entry
       can simply be added at the end.  The only requirement is that
       the dictionary may need to be cleaned before the new entry
       can be added.
    */

    declare entry_string character;

    if dictionary_top+2 >= dictionary_size
        then call clean_dictionary;
    dictionary_top = dictionary_top + 1;
    dictionary_string(dictionary_top) = entry_string;
    dictionary_count(dictionary_top) = 0;
    return dictionary_top;

end build_entry;

build_dictionary: procedure;

    /* Dictionary construction continues until the input routine
       fails to return any data.  The test for a null string is
       simple if we check the length against zero.  The
       dictionary_search routine returns -1 if no match is found and
       then the first character of the input is forced as the match
       and into the dictionary.  Notice that the actual string
       matched is picked up from the dictionary entry.  Coalescence
       takes place as necessary, the match is remembered, and the
       input prepared for another cycle.
    */

    declare (match, last_match) character,
            (count, last_count) fixed,
            index fixed,            /* the entry location */
            threshold fixed;        /* coalescence threshold */

    last_match = '';
    last_count = 0;

    do while true;
        call fill_input_buffer;
        if length(input_buffer) = 0 then return;
        index = search_dictionary(input_buffer);
        if index = -1
            then index = build_entry(substr(input_buffer,0,1));
        match = dictionary_string(index);
        count, dictionary_count(index) = dictionary_count(index) + 1;
        threshold = coalescence_threshold;
        if count >= threshold & last_count >= threshold then
            dictionary_count(build_entry(last_match||match))=first_count;
        last_match = match;
        last_count = count;
        input_buffer = substr(input_buffer, length(match));
    end;

end build_dictionary;

build_encoding_table: procedure;

    /* Code construction has two steps. In the first, every
       dictionary entry of length two or one is thrown out because
       there is no point in replacing such strings with a two
       character code.  Second, codes are assigned using characters
       unseen in the text as starters. When such pairs run out,
       no more codes are assigned even if there are more entries in
       the dictionary.

       Notice the lines below which construct the dictionary code.
       the apparently senseless catenation of two blanks builds a
       completely new string into which the code characters can be
       inserted.  This is a bad glitch in XPL and you probably won't
       understand it unless you program in XPL for some time.
    */

    declare (i, j) fixed,
            top fixed;

    top = -1;
    do i = 0 to dictionary_top;
        if length(dictionary_string(i)) > 2 then
            do;
                top = top + 1;
                dictionary_string(top) = dictionary_string(i);
                dictionary_count(top) = dictionary_count(i);
            end;
    end;
    dictionary_top = top;

    top = -1;
    do i = 1 to "7f";   /* loop over eligible start characters */
        if not character_used(i) then
            do j = 1 to "7f";   /* loop over seco nd characters */
                if top = dictionary_top then return;
                top = top + 1;
                dictionary_code(top) = unique(' ' || ' ');
                byte(dictionary_code(top),0) = i;
                byte(dictionary_code(top),1) = j;
            end;
    end;
    dictionary_top = top;

end build_encoding_table;

compress_text: procedure;

    /* Encoding works almost the same way as dictionary construction.
       Here, though, the input stream is converted to output lines
       as the encodings are found.  The loop runs for as long as
       there is input.
    */

    declare output_buffer character,
            index fixed;

    input_buffer = '';
    print '';
    print '*** the compressed text ***';
    print '';
    call fill_input_buffer;
    do while length(input_buffer) > 0;
        input_length = input_length + length(input_buffer);
        output_buffer = '';
        do while length(input_buffer) > 0;
            index = search_dictionary(input_buffer);
            if index > -1 then
                do;
                    output_buffer = output_buffer
                                    || dictionary_code(index);
                    dictionary_usage(index) = dictionary_usage(index) + 1;
                    input_buffer = substr(input_buffer,
                                          length(dictionary_string(index)));
                end;
            else
                do;
                    output_buffer = output_buffer
                                    || substr(input_buffer,0,1);
                    input_buffer = substr(input_buffer,1);
                end;
        end;
        output_length = output_length + length(output_buffer);
        if print_encoding then print output_buffer;
        call fill_input_buffer;
    end;

end compress_text;

print_summary_statistics: procedure;

    /* Summary statistics include a second printing of the search
       statistics, the dictionary itself, and the compression rate.
       In a working version, both the compressed text and the
       dictionary would have also been printed on separate files for
       re-expansion later.  Notice the complication to print a ratio
       as a decimal in a language without floating point numbers.
    */

    declare i fixed,
            ratio character;

    print '';
    print '*** compression statistics ***';
    print '';
    print 'code frequency   usage   string';
    do i = 0 to dictionary_top;
        print '' || dictionary_code(i) || ' '
                 || i_format(dictionary_count(i), 9) || ' '
                 || i_format(dictionary_usage(i), 9)
                 || '   |' || dictionary_string(i) || '|';
    end;
    print '';
    print '   characters in input = ' || input_length;
    print '   characters in output = ' || output_length;
    ratio = (1000*output_length)/input_length + 1000;
    print '   compression rate = .' || substr(ratio,1);
    print '   compares during dictionary construction = '
                || build_compares;
    print '   compares during compression = '
                || compress_compares;
    print '*** time checks in milleseconds ***';
    print '   time to prepare = '
                || time_check(1) - time_check(0);
    print '   time to build dictionary = '
                || time_check(2) - time_check(1);
    print '   encoding table time = '
                || time_check(3) - time_check(2);
    print '   compression time = '
                || time_check(4) - time_check(3);

end print_summary_statistics;

compress_file: procedure(input_filename, output_filename);

    /* The main routine must assign the I/O units to files, initialize
       needed variables, call the dictionary construction algorithm, build
       the encoding table, and then encode the output.  Most of this work
       is done in called procedures.
    */

    declare (input_filename, output_filename) character;

    time_check(0) = compute_time;
    source_file = xfopen(input_filename, 'r');
    if source_file < 0 then do;
        output(1) = 'Cannot open input';
        call exit(1);
    end;
    echo_file = xfopen(output_filename, 'w');
    if echo_file < 0 then do;
        output(1) = 'Cannot open output';
        call exit(1);
    end;
    print '*** begin the text compression. ***';
    print '';

    call prepare_the_program;
    print_source = true; check_characters = true;
    time_check(1) = compute_time;
    call build_dictionary;
    build_compares = search_compares;
    time_check(2) = compute_time;
    call build_encoding_table;

    if xrewind(source_file) < 0 then do;
        output(1) = 'Rewind failed';
        call exit(2);
    end;
    search_compares = 0;
    print_source, check_characters = false;
    print_encoding = true;
    time_check(3) = compute_time;
    call compress_text;
    compress_compares = search_compares;

    time_check(4) = compute_time;
    call print_summary_statistics;

end compress_file;

call compress_file('text', 'compress');

eof eof eof eof eof eof
