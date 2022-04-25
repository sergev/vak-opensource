 /*   Skeleton
                The proto-compiler of the XPL system
 
 
W. M. McKeeman         J. J. Horning           D. B. Wortman
 
Information &          Computer Science        Computer Science
Computer Science,      Department,             Department,
 
University of          Stanford                Stanford
California at          University,             University,
 
Santa Cruz,            Stanford,               Stanford,
California             California              California
95060                  94305                   94305
 
Developed at the Stanford Computation Center, Campus Facility,   1966-69
and the University of California Computation Center, Santa Cruz, 1968-69.
 
Distributed through the Share organization.
                                                                              */
 
//
// $Id: skeleton.xpl,v 1.1.1.2 1996/06/19 16:53:18 DaveBodenstab Exp $
//
// $Log: skeleton.xpl,v $
// Revision 1.1.1.2  1996/06/19 16:53:18  DaveBodenstab
// Add tags to the source so the Makefile can rebuild skeleton from scratch
//    using analyzer
//
// Revision 1.1.1.1  1996/05/17  21:08:20  DaveBodenstab
// Expand the character set to include lower case
// Explicit EOF line is no longer required
// Eliminate explicit EBCDIC character set dependencies
// Don't print previous error location if there was no previous error
// Support a new compiler toggle: $D to print the production being reduced
// Revise grammar to support:
//     relations !=, <= and >=
//     arith expressions support unary + and -, and binary %
// Add RCS $Id and $log
// Use RCS $Revison in the banner printed at the top of the source listing
// Include a sample program as comments
//
//
//
//
// Control toggles:
//  
//    Code  Default    Description
//   +----+----------+----------------------------------------------------------
//   | $D |  False   | Toggle debugging mode
//   | $L |  True    | Toggle source listing
//   | $M |  False   | Toggle input source printing; takes precedence over $L
//   | $T |  False   | Call TRACE
//   | $U |  False   | Call UNTRACE
//   | $| | Infinity | Set right margin for successive input records
//   +----+----------+----------------------------------------------------------
 
 /*                                                        __GRAMMAR__START__
 
This version of skeleton is a syntax checker for the following grammar:
 
  <program>  ::=  <statement list>
 
  <statement list>  ::=  <statement>
                      |  <statement list> <statement>
 
  <statement>  ::=  <assignment> ;
 
  <assignment>  ::=  <variable> = <expression>
 
  <expression>  ::=  <arith expression>
                  |  <if clause> then <expression> else <expression>
 
  <if clause>  ::=  if <log expression>
 
  <log expression>  ::=  true
                      |  false
                      |  <expression> <relation> <expression>
                      |  <if clause> then <log expression> else <log expression>
 
  <relation>  ::=  =
                |  ! =
                |  <
                |  < =
                |  >
                |  > =
 
  <arith expression>  ::=  <term>
                        |  <arith expression> + <term>
                        |  <arith expression> - <term>
                        |  + <term>
                        |  - <term>
 
  <term>  ::=  <primary>
            |  <term> * <primary>
            |  <term> / <primary>
            |  <term> % <primary>
 
  <primary>  ::=  <variable>
               |  <number>
               |  ( <expression> )
 
  <variable>  ::=  <identifier>
                |  <variable> ( <expression> )
                                                             __GRAMMAR__END__ */
 
 
 /*
An example <program> would be:
 
                                                           __EXAMPLE__START__
   v = a % b;
   v = b + c * sin(33) - ( e + f / atan(0) );
   v = if true then -1 else 1;
   v = if b = 99 then c + d else c - d;
   v = ( if b <= 55 then b else b % 55 ) +
       ( if if d != e then e < d else e > d then d * e else d / e );
 
                                                             __EXAMPLE__END__ */
 
   /*  First we initialize the global constants that depend upon the input
      grammar.  The following cards are punched by the syntax pre-processor  */
 
 /*                                                         __TABLES__START__ */
   declare NSY literally '34', NT literally '20';
   declare v(NSY) character initial ( '<error: token = 0>', ';', '=', '!', '<',
      '>', '+', '-', '*', '/', '%', '(', ')', 'if', '_|_', 'then', 'else',
      'true', 'false', '<number>', '<identifier>', '<term>', '<program>',
      '<primary>', '<variable>', '<relation>', '<statement>', '<if clause>',
      '<assignment>', '<expression>', '<statement list>', '<log expression>',
      '<arith expression>', 'else', 'else');
   declare v_index(12) bit(8) initial ( 1, 13, 14, 15, 18, 19, 19, 19, 20, 20,
      20, 20, 21);
   declare c1(NSY) character initial (
      "(2) 00000 00000 00000 00000 0",
      "(2) 00000 00000 00002 00000 2",
      "(2) 00000 03300 03030 00003 3",
      "(2) 00100 00000 00000 00000 0",
      "(2) 00100 02200 02020 00002 2",
      "(2) 00100 02200 02020 00002 2",
      "(2) 00000 00000 01000 00001 1",
      "(2) 00000 00000 01000 00001 1",
      "(2) 00000 00000 01000 00001 1",
      "(2) 00000 00000 01000 00001 1",
      "(2) 00000 00000 01000 00001 1",
      "(2) 00000 01100 01010 00001 1",
      "(2) 02222 22222 22200 22000 0",
      "(2) 00000 01100 01010 00111 1",
      "(2) 00000 00000 00000 00000 1",
      "(2) 00000 01100 01010 00111 1",
      "(2) 00000 02200 02020 00222 2",
      "(2) 00000 00000 00000 22000 0",
      "(2) 00000 00000 00000 22000 0",
      "(2) 02222 22222 20200 22000 0",
      "(2) 02222 22222 22200 22000 0",
      "(2) 02222 22211 10200 22000 0",
      "(2) 00000 00000 00002 00000 0",
      "(2) 02222 22222 20200 22000 0",
      "(2) 02322 22222 21200 22000 0",
      "(2) 00000 01100 01010 00001 1",
      "(2) 00000 00000 00002 00000 2",
      "(2) 00000 00000 00000 10000 0",
      "(2) 01000 00000 00000 00000 0",
      "(2) 02333 30000 00300 23000 0",
      "(2) 00000 00000 00002 00000 1",
      "(2) 00000 00000 00000 23000 0",
      "(2) 02222 21100 00200 22000 0",
      "(2) 00000 01100 01010 00001 1",
      "(2) 00000 01100 01010 00111 1");
   declare nc1TRIPLES literally '22';
   declare c1TRIPLES(nc1TRIPLES) fixed initial ( 728332, 859394, 859395, 859396,
      859397, 923650, 990466, 990467, 990468, 990469, 990480, 990992, 1573382,
      1573383, 1573387, 1573389, 1573395, 1573396, 1972226, 2235650, 2235651,
      2235652, 2235653);
   declare prtb(34) fixed initial (0, 28, 3, 4, 5, 0, 0, 0, 1575709, 2845, 0, 0,
      0, 0, 0, 0, 8198, 8199, 6, 7, 0, 5384, 5385, 5386, 0, 0, 30, 0, 453975329,
      6146, 7449, 0, 453975842, 13, 0);
   declare prdtb(34) bit(8) initial (0, 4, 14, 16, 18, 13, 15, 17, 32, 30, 0, 0,
      9, 10, 29, 31, 20, 21, 22, 23, 19, 25, 26, 27, 24, 28, 3, 2, 7, 5, 11, 1,
      12, 8, 6);
   declare hdtb(34) bit(8) initial (0, 26, 25, 25, 25, 25, 25, 25, 24, 23, 33,
      34, 31, 31, 23, 24, 32, 32, 32, 32, 32, 21, 21, 21, 21, 23, 30, 30, 29,
      28, 31, 22, 31, 27, 29);
   declare prlength(34) bit(8) initial (0, 2, 2, 2, 2, 1, 1, 1, 4, 3, 1, 1, 1,
      1, 1, 1, 3, 3, 2, 2, 1, 3, 3, 3, 1, 1, 2, 1, 5, 3, 3, 1, 5, 2, 1);
   declare context_case(34) bit(8) initial (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
   declare left_context(0) bit(8) initial ( 29);
   declare left_index(14) bit(8) initial ( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 1, 1);
   declare context_triple(0) fixed initial ( 0);
   declare triple_index(14) bit(8) initial ( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 1);
   declare pr_index(34) bit(8) initial ( 1, 2, 6, 6, 7, 8, 8, 8, 8, 8, 8, 8, 10,
      10, 10, 10, 12, 13, 14, 15, 16, 21, 21, 25, 26, 26, 28, 28, 28, 31, 32,
      34, 35, 35, 35);
 
   /*  End of cards punched by syntax                       __TABLES__END__ */
 
   /*  Declarations for the scanner                                        */
 
   /* token is the index into the vocabulary v() of the last symbol scanned,
      cp is the pointer to the last character scanned in the cardimage,
      bcd is the last symbol scanned (literal character string). */
   declare (token, cp) fixed, bcd character;
 
   /* Set up some convenient abbreviations for printer control */
   declare EJECT_PAGE literally 'output(1) = page',
      page character initial ('1'), double character initial ('0'),
      DOUBLE_SPACE literally 'output(1) = double',
      x70 character initial ('
                    ');
 
   /* Length of longest symbol in v */
   declare (reserved_limit, margin_chop) fixed;
 
   /* chartype() is used to distinguish classes of symbols in the scanner.
      tx() is a table used for translating from one character set to another.
      control() holds the value of the compiler control toggles set in $ cards.
      not_letter_or_digit() is similiar to chartype() but used in scanning
      identifiers only.
 
      All are used by the scanner and control() is set there.
   */
   declare (chartype, tx) (255) bit(8),
           (control, not_letter_or_digit)(255) bit(1);
 
   /* Alphabet consists of the symbols considered alphabetic in building
      identifiers     */
   declare alphabet character initial (
      'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_$@#');
 
   /* buffer holds the latest cardimage,
      text holds the present state of the input text
      (not including the portions deleted by the scanner),
      text_limit is a convenient place to store the pointer to the end of text,
      card_count is incremented by one for every source card read,
      error_count tabulates the errors as they are detected,
      severe_errors tabulates those errors of fatal significance.
   */
   declare (buffer, text) character,
      (text_limit, card_count, error_count, severe_errors, previous_error) fixed
      ;
 
   /* number_value contains the numeric value of the last constant scanned,
   */
   declare number_value fixed;
 
   /* Each of the following contains the index into v() of the corresponding
      symbol.   we ask:    if token = ident    etc.    */
   declare (ident, number, divide, eofile) fixed;
 
   /* stopit() is a table of symbols which are allowed to terminate the error
      flush process.  In general they are symbols of sufficient syntactic
      hierarchy that we expect to avoid attempting to start checking again
      right into another error producing situation.  The token stack is also
      flushed down to something acceptable to a stopit() symbol.
      failsoft is a bit which allows the compiler one attempt at a gentle
      recovery.   Then it takes a strong hand.   When there is real trouble
      compiling is set to false, thereby terminating the compilation.
   */
   declare stopit(100) bit(1), (failsoft, compiling) bit(1);
 
   declare s character;  /* A temporary used various places */
 
   /* The entries in prmask() are used to select out portions of coded
      productions and the stack top for comparison in the analysis algorithm */
   declare prmask(5) fixed initial (0, 0, "ff", "ffff", "ffffff", "ffffffff");
 
 
   /*The proper substring of pointer is used to place an  |  under the point
      of detection of an error during checking.  It marks the last character
      scanned.  */
   declare pointer character initial ('
                                           |');
   declare callcount(20) fixed   /* Count the calls of important procedures */
      initial(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
 
   /* Record the times of important points during checking */
   declare clock(5) fixed;
 
 
   /* Commonly used strings */
   declare x1 character initial(' '), x4 character initial('    ');
   declare period character initial ('.');
 
   /* Temporaries used throughout the compiler */
   declare (i, j, k, l) fixed;
 
   declare TRUE literally '1', FALSE literally '0', FOREVER literally 'while 1';
 
   /*  The stacks declared below are used to drive the syntactic
      analysis algorithm and store information relevant to the interpretation
      of the text.  The stacks are all pointed to by the stack pointer sp.  */
 
   declare STACKSIZE literally '75';  /* size of stack  */
   declare parse_stack (STACKSIZE) bit(8); /* tokens of the partially parsed
                                              text */
   declare var (STACKSIZE) character;/* character text name of item */
   declare fixv (STACKSIZE) fixed;   /* fixed (numeric) value */
 
   /* sp points to the right end of the reducible string in the parse stack,
      mp points to the left end, and
      mpp1 = mp+1.
   */
   declare (sp, mp, mpp1) fixed;
 
 
 
 
 
   /*               P R O C E D U R E S                                  */
 
 
 
pad:
   procedure (string, width) character;
      declare string character, (width, l) fixed;
 
      l = length(string);
      if l >= width then return string;
      else return string || substr(x70, 0, width-l);
   end pad;
 
i_format:
   procedure (number, width) character;
      declare (number, width, l) fixed, string character;
 
      string = number;
      l = length(string);
      if l >= width then return string;
      else return substr(x70, 0, width-l) || string;
   end i_format;
 
error:
   procedure(msg, severity);
      /* Prints and accounts for all error messages */
      /* If severity is not supplied, 0 is assumed */
      declare msg character, severity fixed;
      error_count = error_count + 1;
      /* If listing is suppressed, force printing of this line */
      if ~ control(byte('L')) then
         output = i_format (card_count, 4) || ' |' || buffer || '|';
      output = substr(pointer, text_limit-cp+margin_chop);
      if previous_error = 0 then
         output = '*** error, ' || msg || '.  ***';
      else
         output = '*** error, ' || msg ||
               '.  last previous error was detected on line ' ||
               previous_error || '.  ***';
      previous_error = card_count;
      if severity > 0 then
         if severe_errors > 25 then
            do;
               output = '*** too many severe errors, checking aborted ***';
               compiling = FALSE;
            end;
         else severe_errors = severe_errors + 1;
   end error;
 
 
 
 
 
  /*                   Card image handling procedure                      */
 
 
get_card:
   procedure;
      /* Does all card reading and listing                                 */
      declare i fixed, (temp, temp0, rest) character;
            buffer = input;
            if length(buffer) = 0 then
               do; /* signal for EOF */
                  buffer = pad ('eof', 80);
                  control(byte('L')) = FALSE;
               end;
            else card_count = card_count + 1;  /* Used to print on listing */
      if margin_chop > 0 then
         do; /* The margin control from dollar | */
            i = length(buffer) - margin_chop;
            rest = substr(buffer, i);
            buffer = substr(buffer, 0, i);
         end;
      else rest = '';
      text = buffer;
      text_limit = length(text) - 1;
      if control(byte('M')) then output = buffer;
      else if control(byte('L')) then
         output = i_format (card_count, 4) || ' |' || buffer || '|' || rest;
      cp = 0;
   end get_card;
 
 
   /*                The scanner procedures              */
 
 
char:
   procedure;
      /* Used for strings to avoid card boundary problems */
      cp = cp + 1;
      if cp <= text_limit then return;
      call get_card;
   end char;
 
 
scan:
   procedure;
      declare (s1, s2) fixed;
      callcount(3) = callcount(3) + 1;
      failsoft = TRUE;
      bcd = '';  number_value = 0;
      do FOREVER;
         if cp > text_limit then call get_card;
         else
            do; /* Discard last scanned value */
               text_limit = text_limit - cp;
               text = substr(text, cp);
               cp = 0;
            end;
         /*  Branch on next character in text                  */
         do case chartype(byte(text));
 
            /*  case 0  */
 
            /* Illegal characters fall here  */
            call error ('illegal character: ' || substr(text, 0, 1), 0);
 
            /*  case 1  */
 
            /*  Blank  */
            do;
               cp = 1;
               do while byte(text, cp) = byte(' ') & cp <= text_limit;
                  cp = cp + 1;
               end;
               cp = cp - 1;
            end;
 
            /*  case 2  */
 
        ;   /*  Not used in skeleton (but used in xcom)  */
 
            /*  case 3  */
 
        ;   /*  Not used in skeleton (but used in xcom)  */
 
            /*  case 4  */
 
            do FOREVER;  /* A letter:  Identifiers and reserved words */
               do cp = cp + 1 to text_limit;
                  if not_letter_or_digit(byte(text, cp)) then
                     do;  /* End of identifier  */
                        if cp > 0 then bcd = bcd || substr(text, 0, cp);
                        s1 = length(bcd);
                        if s1 > 1 then if s1 <= reserved_limit then
                           /* Check for reserved words */
                           do i = v_index(s1-1) to v_index(s1) - 1;
                              if bcd = v(i) then
                                 do;
                                    token = i;
                                    return;
                                 end;
                           end;
                        /*  Reserved words exit higher: therefore <identifier>*/
                        token = ident;
                        return;
                     end;
               end;
               /*  End of card  */
               bcd = bcd || text;
               call get_card;
               cp = -1;
            end;
 
            /*  case 5  */
 
            do;      /*  Digit:  a number  */
               token = number;
               do FOREVER;
                  do cp = cp to text_limit;
                     s1 = byte(text, cp);
                     if s1 < byte('0') | byte('9') < s1 then return;
                     number_value = 10*number_value + s1 - byte('0');
                  end;
                  call get_card;
               end;
            end;
 
            /*  case 6  */
 
            do;      /*  A /:  May be divide or start of comment  */
               call char;
               if byte(text, cp) ~= byte('*') then
                  do;
                     token = divide;
                     return;
                  end;
               /* We have a comment  */
               s1, s2 = byte(' ');
               do while s1 ~= byte('*') | s2 ~= byte('/');
                  if s1 = byte('$') then
                     do;  /* A control character  */
                        control(s2) = ~ control(s2);
                        if s2 = byte('T') then call trace;
                        else if s2 = byte('U') then call untrace;
                        else if s2 = byte('|') then
                           if control(s2) then
                              margin_chop = text_limit - cp + 1;
                           else
                              margin_chop = 0;
                     end;
                  s1 = s2;
                  call char;
                  s2 = byte(text, cp);
               end;
            end;
 
            /*  case 7  */
            do;      /*  Special characters  */
               token = tx(byte(text));
               cp = 1;
               return;
            end;
 
            /*  case 8  */
        ;   /*  Not used in skeleton (but used in xcom)  */
 
         end;     /* Of case on chartype  */
         cp = cp + 1;  /* Advance scanner and resume search for token  */
      end;
   end scan;
 
 
 
 
  /*                       Time and Date                                 */
 
 
print_time:
   procedure (message, t);
      declare message character, t fixed;
      message = message || t/360000 || ':' || t mod 360000 / 6000 || ':'
         || t mod 6000 / 100 || '.';
      t = t mod 100;  /* decimal fraction  */
      if t < 10 then message = message || '0';
      output = message || t || '.';
   end print_time;
 
print_date_and_time:
   procedure (message, d, t);
      declare message character, (d, t, year, day, m) fixed;
      declare month(11) character initial ('january', 'february', 'march',
         'april', 'may', 'june', 'july', 'august', 'september', 'october',
         'november', 'december'),
      days(12) fixed initial (0, 31, 60, 91, 121, 152, 182, 213, 244, 274,
         305, 335, 366);
      year = d/1000 + 1900;
      day = d mod 1000;
      if (year & "3") ~= 0 then if day > 59 then day = day + 1; /* ~ leap year*/
      m = 1;
      do while day > days(m);  m = m + 1;  end;
      call print_time(message || month(m-1) || x1 || day-days(m-1) ||  ', '
         || year || '.  clock time = ', t);
   end print_date_and_time;
 
  /*                       Initialization                                     */
 
 
 
initialization:
   procedure;
      declare revision character initial('$revision: 1.1.1.2 $');
      EJECT_PAGE;
      call print_date_and_time ('syntax check -- skeleton '
         || substr(revision,11,length(revision)-13) || ' version of ',
         date_of_generation, time_of_generation);
      DOUBLE_SPACE;
      call print_date_and_time ('today is ', date, time);
      DOUBLE_SPACE;
      do i = 1 to NT;
         s = v(i);
         if s = '<number>' then number = i;  else
         if s = '<identifier>' then ident = i;  else
         if s = '/' then divide = i;  else
         if s = '_|_' then eofile = i;  else
         if s = ';' then stopit(i) = TRUE;  else
         ;
      end;
      if ident = NT then reserved_limit = length(v(NT-1));
      else reserved_limit = length(v(NT));
      v(eofile) = 'eof';
      stopit(eofile) = TRUE;
      chartype(byte(' ')) = 1;
      do i = 0 to 255;
         not_letter_or_digit(i) = TRUE;
      end;
      do i = 0 to length(alphabet) - 1;
         j = byte(alphabet, i);
         tx(j) = i;
         not_letter_or_digit(j) = FALSE;
         chartype(j) = 4;
      end;
      do i = 0 to 9;
         j = byte('0123456789', i);
         not_letter_or_digit(j) = FALSE;
         chartype(j) = 5;
      end;
      do i = v_index(0) to v_index(1) - 1;
         j = byte(v(i));
         tx(j) = i;
         chartype(j) = 7;
      end;
      chartype(byte('/')) = 6;
      /* First set up global variables controlling scan, then call it */
      cp = 0;  text_limit = -1;
      text = '';
      control(byte('L')) = TRUE;
      call scan;
 
      /* Initialize the parse stack */
      sp = 1;  parse_stack(sp) = eofile;
 
   end initialization;
 
 
 
 
 
 
dumpit:
   procedure;    /* Dump out the statistics collected during this run  */
      DOUBLE_SPACE;
      /*  Put out the entry count for important procedures */
 
      output = 'stacking decisions= ' || callcount(1);
      output = 'scan              = ' || callcount(3);
      output = 'free string area  = ' || freelimit - freebase;
   end dumpit;
 
 
stack_dump:
   procedure;
      declare line character;
      line = 'partial parse to this point is: ';
      do i = 2 to sp;
         if length(line) > 105 then
            do;
               output = line;
               line = x4;
            end;
         line = line || x1 || v(parse_stack(i));
      end;
      output = line;
   end stack_dump;
 
 
  /*                  The synthesis algorithm for XPL                      */
 
 
synthesize:
procedure(production_number);
   declare (i, production_number) fixed;
   declare line character;
 
   /*  This procedure is responsible for the semantics (code synthesis), if
      any, of the skeleton compiler.  Its argument is the number of the
      production which will be applied in the pending reduction.  The global
      variables mp and sp point to the bounds in the stacks of the right part
      of this production.
       Normally, this procedure will take the form of a giant case statement
      on production_number.  However, the syntax checker has semantics (the
      termination of checking) only for production 1.                     */
 
   if production_number > 0 then
      do;
         if control(byte('D')) then
            do;
               declare npr fixed initial("ffffffff"), prd fixed;
               if npr = -1 then
                  do;
                     do i = 0 to NSY;
                        if pr_index(i) > npr then npr = pr_index(i);
                     end;
                     npr = npr - 1;
                  end;
               prd = 0;
               do i = 0 to npr;
                  if prdtb(i) = production_number then prd = i;
               end;
               if prd = 0 then
                  line = '  <*****> ::=';
               else
                  line = '  ' || v(hdtb(prd)) || ' ::=';
               do i = mp to sp;
                  if parse_stack(i) = ident then
                     line = line || x1 || '<identifier=' || var(i) || '>';
                  else
                     if parse_stack(i) = number then
                        line = line || x1 || '<number=' || fixv(i) || '>';
                     else
                        line = line || x1 || v(parse_stack(i));
               end;
               output = 'reduce ' || i_format(production_number,3) || line;
            end;
      end;
 
   if production_number = 1 then
 
 /*  <program>  ::=  <statement list>    */
   do;
      if mp ~= 2 then  /* We didn't get here legitimately  */
         do;
            call error ('eof at invalid point', 1);
            call stack_dump;
         end;
      compiling = FALSE;
   end;
end synthesize;
 
 
 
 
  /*              Syntactic parsing functions                              */
 
 
right_conflict:
   procedure (left) bit(1);
      declare left fixed;
      /*  This procedure is TRUE if token is ~ a legal right context of left*/
      return ("c0" & shl(byte(c1(left), shr(token,2)), shl(token,1)
         & "06")) = 0;
   end right_conflict;
 
 
recover:
   procedure;
      /* If this is the second successive call to recover, discard one symbol */
      if ~ failsoft then call scan;
      failsoft = FALSE;
      do while ~ stopit(token);
         call scan;  /* To find something solid in the text  */
      end;
      do while right_conflict (parse_stack(sp));
         if sp > 2 then sp = sp - 1;  /* And in the stack  */
         else call scan;  /* But don't go too far  */
      end;
      output = 'resume:' || substr(pointer, text_limit-cp+margin_chop+7);
   end recover;
 
stacking:
   procedure bit(1);  /* Stacking decision function */
      callcount(1) = callcount(1) + 1;
      do FOREVER;    /* Until return  */
         do case shr(byte(c1(parse_stack(sp)),shr(token,2)),shl(3-token,1)&6)&3;
 
            /*  case 0  */
            do;    /* Illegal symbol pair  */
               call error('illegal symbol pair: ' || v(parse_stack(sp)) || x1 ||
                  v(token), 1);
               call stack_dump;
               call recover;
            end;
 
            /*  case 1  */
 
            return TRUE;      /*  Stack token  */
 
            /*  case 2  */
 
            return FALSE;     /* Don't stack it yet  */
 
            /*  case 3  */
 
            do;      /* Must check TRIPLES  */
               j = shl(parse_stack(sp-1), 16) + shl(parse_stack(sp), 8) + token;
               i = -1;  k = nc1TRIPLES + 1;  /* Binary search of TRIPLES  */
               do while i + 1 < k;
                  l = shr(i+k, 1);
                  if c1TRIPLES(l) > j then k = l;
                  else if c1TRIPLES(l) < j then i = l;
                  else return TRUE;  /* It is a valid triple  */
               end;
               return FALSE;
            end;
 
         end;    /* Of do case  */
      end;   /*  Of do FOREVER */
   end stacking;
 
pr_ok:
   procedure(prd) bit(1);
      /* Decision procedure for context check of equal or imbedded right parts*/
      declare (h, i, j, prd) fixed;
      do case context_case(prd);
 
         /*  case 0 -- No check required  */
 
         return TRUE;
 
         /*  case 1 -- Right context check  */
 
         return ~ right_conflict (hdtb(prd));
 
         /*  case 2 -- Left context check  */
 
         do;
            h = hdtb(prd) - NT;
            i = parse_stack(sp - prlength(prd));
            do j = left_index(h-1) to left_index(h) - 1;
               if left_context(j) = i then return TRUE;
            end;
            return FALSE;
         end;
 
         /*  case 3 -- Check TRIPLES  */
 
         do;
            h = hdtb(prd) - NT;
            i = shl(parse_stack(sp - prlength(prd)), 8) + token;
            do j = triple_index(h-1) to triple_index(h) - 1;
               if context_triple(j) = i then return TRUE;
            end;
            return FALSE;
         end;
 
      end;  /* Of do case  */
   end pr_ok;
 
 
  /*                     Analysis algorithm                                  */
 
 
 
reduce:
   procedure;
      declare (i, j, prd) fixed;
      /* Pack stack top into one word */
      do i = sp - 4 to sp - 1;
         j = shl(j, 8) + parse_stack(i);
      end;
 
      do prd = pr_index(parse_stack(sp)-1) to pr_index(parse_stack(sp)) - 1;
         if (prmask(prlength(prd)) & j) = prtb(prd) then
            if pr_ok(prd) then
            do;  /* An allowed reduction */
               mp = sp - prlength(prd) + 1; mpp1 = mp + 1;
               call synthesize(prdtb(prd));
               sp = mp;
               parse_stack(sp) = hdtb(prd);
               return;
            end;
      end;
 
      /* Look up has failed, error condition */
      call error('no production is applicable',1);
      call stack_dump;
      failsoft = FALSE;
      call recover;
   end reduce;
 
compilation_loop:
   procedure;
 
      compiling = TRUE;
      do while compiling;     /* Once around for each production (reduction)  */
         do while stacking;
            sp = sp + 1;
            if sp = STACKSIZE then
               do;
                  call error ('stack overflow *** checking aborted ***', 2);
                  return;   /* Thus aborting checking */
               end;
            parse_stack(sp) = token;
            var(sp) = bcd;
            fixv(sp) = number_value;
            call scan;
         end;
 
         call reduce;
      end;     /* Of do while compiling  */
   end compilation_loop;
 
 
 
 
print_summary:
   procedure;
      declare i fixed;
      call print_date_and_time ('end of checking ', date, time);
      output = '';
      output = card_count || ' cards were checked.';
      if error_count = 0 then output = 'No errors were detected.';
      else if error_count > 1 then
         output = error_count || ' errors (' || severe_errors
            || ' severe) were detected.';
      else if severe_errors = 1 then output = 'One severe error was detected.';
         else output = 'One error was detected.';
      if previous_error > 0 then
         output = 'The last detected error was on line ' || previous_error
            || period;
      if control(byte('D')) then call dumpit;
      DOUBLE_SPACE;
      clock(3) = time;
      do i = 1 to 3;   /* Watch out for midnight */
         if clock(i) < clock(i-1) then clock(i) = clock(i) +  8640000;
      end;
      call print_time ('Total time in checker    ', clock(3) - clock(0));
      call print_time ('Set up time              ', clock(1) - clock(0));
      call print_time ('Actual checking time     ', clock(2) - clock(1));
      call print_time ('Clean-up time at end     ', clock(3) - clock(2));
      if clock(2) > clock(1) then   /* Watch out for clock being off */
      output = 'Checking rate: ' || 6000*card_count/(clock(2)-clock(1))
         || ' cards per minute.';
   end print_summary;
 
main_procedure:
   procedure;
      clock(0) = time;  /* Keep track of time in execution */
      call initialization;
 
      clock(1) = time;
 
      call compilation_loop;
 
      clock(2) = time;
 
      /* clock(3) gets set in print_summary */
      call print_summary;
 
   end main_procedure;
 
 
call main_procedure;
return severe_errors;
 
/* vi: set sw=3 ai sm expandtab: */
eof eof eof
