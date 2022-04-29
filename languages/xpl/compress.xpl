1   /*  ~$F large free string space
2       A Text Compression Program.
3
4       Author -- Charles Wetherell  18 August 1976.
5       Last date modified -- 25 August 1976.
6
7       This program compresses text files using the Mayne-James dictionary
8       construction algorithm. Input is an arbitrary text file and output
9       is the compressed file along with an encoding dictionary.  Some
10      statistics on program efficiency and compression rate are kept.
11      Two passes by the source file are necessary. On the first pass the
12      dictionary is built and a record of all characters seen is kept.
13      Between passes the character record is used to add encodings to the
14      dictionary entries. During the second pass, long high frequency
15      strings are replaced by shorter encoding strings as the compressed
16      file is written. Further information about the technique can be
17      found in chapter 11 of
18
19          Wetherell, C.S.  Etudes For Programmers.  Prentice-Hall,
20              Englewood Cliffs, NJ. 1978.
21
22      In this version of the algorithm, ends of input lines stop string
23      matches during dictionary construction and text encoding; the
24      carriage return is not treated like a character.  Only characters
25      whose internal representations lie in the range 1 to 127 will be
26      considered for encoding pairs so that the pairs will have
27      reasonable print representations. In a full working implementation
28      all 256 available characters would be used for encoding.
29
30      The dictionary search, entry, and cleanup routines are written
31      so that they may be changed quite easily.
32      The algorithms can be modified by replacing the bodies of procedures
33      search.dictionary, clean.dictionary, and build.entry, along with
34      making any necessary changes to prepare.the.program.  The various
35      thresholds parameters are all calculated by functions and can be
36      be modifed by changing the function definitions.
37      If there is a data structure added for searching, make sure that
38      build.encoding.table leaves the structure in good shape after codes
39      are added and entries of length two and less are deleted.
40
41      This version uses simple linear search, a hyperbolic threshold
42      for coalescence, a mean threshold for deletion, and an initial
43      count of one for coalesced entries.
44  */
45
46  /* Some macros to improve XPL as a language.                            */
47
48  declare logical literally 'bit(1)',
49          true    literally '1',
50          false   literally '0',
51          not     literally '~',      /* to improve printing */
52          compute.time literally 'coreword("1e312")*2'; /* the clock */
53
54  /* Declarations for I/O units                                           */
55
56  declare source.file literally '1',
57          echo.file   literally '2',
58          print literally 'output(echo.file) =';
59
60  /* Declarations for the input routine                                   */
61
62  declare input.buffer character,
63          (print.source, check.characters) logical,
64          character.used("ff") logical;   /* i.e. 256 different entries   */
65
66  /* Declarations for the dictionary                                      */
67
68  declare dictionary.size literally '100',
69          dictionary.string(dictionary.size) character,
70          dictionary.count(dictionary.size) fixed,
71          dictionary.code(dictionary.size) character,
72          dictionary.usage(dictionary.size) fixed,
73          dictionary.top fixed;
74
75  /* Control for encoding print.                                          */
76
77  declare print.encoding logical;
78
79  /* Declarations for encoding statistics */
80
81  declare search.compares fixed,
82          build.compares fixed,
83          compress.compares fixed;
84
85  declare time.check(10) fixed;
86
87  declare (input.length, output.length) fixed;
88
89  i.format: procedure(number, width) character;
90
91      /* Function i.format converts its argument number into a string
92         and then pads the string on the left to bring the length up
93         to width characters. All of this is just the FORTRAN
94         integer format.
95      */
96
97      declare number fixed,
98              width fixed;
99
100     declare string character,
101             blanks character initial ('                     ');
102
103     string = num ber;
104     if length(string) < width then
105         string = substr(blanks, 0, width - length(string)) || string;
106     return string;
107
108 end i.format;
109
110 prepare.the.program: procedure;
111
112     /* Only simple clearing of the dictionary, the character record,
113        the statistics, and a few scalars is required.
114     */
115
116     declare i fixed;
117
118     do i = 0 to dictionary.size;
119         dictionary.string(i), dictionary.code(i) = '';
120         dictionary.count(i), dictionary.usage(i) = 0;
121     end;
122     dictionary.top = -1;
123
124     do i = 0 to "ff"; character.used(i) = false; end;
125     input.buffer = ";
126
127     search.compares = 0;
128     input.length, output.length = 0;
129
130 end prepare.the.program;
131
132 fill.input.buffer: procedure;
133
134     /* If input.buffer is empty, then this routine tries to read a
135        line from the source.file. The line read goes into
136        input.buffer with a null line the signal for end of file. If
137        flag print.source is on, then the input is echoed. If flag
138        check.characters is on, the input is scanned for character
139        usage.
140     */
141
142     declare i fixed;
143
144     if length(input.buffer) > 0 then return;
145     input.buffer = input(source.file);
146     if print.source then print input.buffer;
147     if check.characters then
148         do i = 0 to length(input.buffer)-1;
149             character.used(byte(input.buffer,i)) = true;
150         end;
151
152 end fill.input.buffer;
153
154 coalescence.threshold: procedure fixed;
155
156     /* This procedure calculates the threshold for coalescing
157        two dictionary entries into one. Here, the requirement is
158        that the entries have frequencies greater than the reciprocal
159        of the ratio of space remaining in the dictionary.
160     */
161
162     return dictionary.size/(dictionary.size-dictionary.top+1) + 1;
163
164 end coalescence.threshold;
165
166 deletion.threshold: procedure fixed;
167
168     /* This function returns the threshold necessary for an entry
169        to be retained in the dictionary at cleanup time.
170        In this version, the frequency must be greater than the
171        rounded up mean frequency.
172     */
173
174     declare sum fixed,
175             i fixed;
176
177     sum = 0;
178     do i = 0 to dictionary.top;
179         sum = sum + dictionary.count(i);
180     end;
181     return sum/(dictionary.top+1) + 1;
182
183 end deletion.threshold;
184
185 first.count: procedure fixed;
186
187     /* This function returns the count given a coalesced entry when
188        it is first entered in the dictionary.
189     */
190
191     return 1; /* currently give a count of 1. */
192
193 end first.count;
194
195 search.dictionary: procedure(test.string) fixed;
196
197     /* This function searches the dictionary for the longest match
198        with the head of the argument test.string.   If no match is
199        found, the routine returns -1 as value; if a match is found,
200        the index of the match is returned as value.
201
202        This routine performs a simple linear search of the dictionary
203        from the zeroth entry to the entry dictionary.top. If an
204        entry's length is longer than the longest current match and
205        still no longer than the argument, then the entry is matched
206        against the argument.  Equality will cause the match to be
207        updated. Notice that by starting the index at -1, the return
208        value will be proper even if no match is found.
209     */
210
211     declare test.string character;
212
213     declare index fixed,
214             (match.length, arg.length, entry.length) fixed,
215             i fixed;
216
217     index = -1;
218     match.length = 0;
219     arg.length = length(test.string);
220
221     do i = 0 to dictionary.top;
222         entry.length = length(dictionary.string(i));
223         if entry.length > match.length
224               & entry.length <= arg.length then
225             if dictionary.string(i)
226                   = substr(test.string,0,entry.length) then
227                 do;
228                     index = i;
229                     match.length = entry.length;
230                 end;
231     end;
232     search.compares = search.compares + dictionary.top + 1;
233     return index;
234
235 end search.dictionary;
236
237 clean.dictionary: procedure;
238
239     /* Clean.dictionary eliminates at least one low frequency entry
240        from the dictionary and restores the smaller dictionary to
241        the format it had before cleaning.
242        The while loop surrounding the body of the procedure guarantees
243        that at least one entry is deleted from the dictionary before
244        return. If the initial threshold is not high enough to delete
245        an entry, the threshold is incremented until something is
246        deleted.
247
248        The dictionary is just a linear table with no structure so
249        entries can be deleted by pushing the retained entries toward
250        the zero end of the table overwriting the removed entries.
251     */
252
253     declare i fixed,
254             threshold fixed,
255             old.top fixed,
256             new.top fixed;
257
258     old.top = dictionary.top;
259     threshold = deletion.threshold;
260     do while old.top = dictionary.top;
261         new.top = -1;
262         do i = 0 to dictionary.top;
263             if dictionary.count(i) >= threshold then
264                 do;
265                     new.top = new.top + 1;
266                     dictionary.string(new.top) = dictionary.string(i);
267                     dictionary.count(new.top) = dictionary.count(i);
268                 end;
269         end;
270         dictionary.top = new.top;
271         threshold = threshold + 1;
272     end;
273
274 end clean.dictionary;
275
276 build.entry: procedure(entry.string) fixed;
277
278     /* Build.entry adds entry.string to the dictionary with a count
279        of zero and returns as value the index of the new entry.
280
281        Because the dictionary is searched linearly, the new entry
282        can simply be added at the end.  The only requirement is that
283        the dictionary may need to be cleaned before the new entry
284        can be added.
285     */
286
287     declare entry.string character;
288
289     if dictionary.top+2 >= dictionary.size
290         then call clean.dictionary;
291     dictionary.top = dictionary.top + 1;
292     dictionary.string(dictionary.top) = entry.string;
293     dictionary.count(dictionary.top) = 0;
294     return dictionary.top;
295
296 end build.entry;
297
298 build.dictionary; procedure;
299
300     /* Dictionary construction continues until the input routine
301        fails to return any data.  The test for a null string is
302        simple if we check the length against zero.  The
303        dictionary.search routine returns -1 if no match is found and
304        then the first character of the input is forced as the match
305        and into the dictionary.  Notice that the actual string
306        matched is picked up from the dictionary entry.  Coalescence
307        takes place as necessary, the match is remembered, and the
308        input prepared for another cycle.
309     */
310
311     declare (match, last.match) character,
312             (count, last.count) fixed,
313             index fixed,            /* the entry location */
314             threshold fixed;        /* coalescence threshold */
315
316     last.match = '';
317     last.count = 0;
318
319     do w hile true;
320         call fill.input.buffer;
321         if length(input.buffer) = 0 then return;
322         index = search.dictionary(input.buffer);
323         if index = -1
324             then index = build.entry(substr(input.buffer,0,1));
325         match = dictionary.string(index);
326         count, dictionary.count(index) = dictionary.count(index) + 1;
327         threshold = coalescence.threshold;
328         if count >= threshold & last.count >= threshold then
329             dictionary.count(build.entry(last.match||match))=first.count;
330         last.match = match;
331         last.count = count;
332         input.buffer = substr(input.buffer, length(match));
333     end;
334
335 end build.dictionary;
336
337 build.encoding.table: procedure;
338
339     /* Code construction has two steps. In the first, every
340        dictionary entry of length two or one is thrown out because
341        there is no point in replacing such strings with a two
342        character code.  Second, codes are assigned using characters
343        unseen in the text as starters. When such pairs run out,
344        no more codes are assigned even if there are more entries in
345        the dictionary.
346
347        Notice the lines below which construct the dictionary code.
348        the apparently senseless catenation of two blanks builds a
349        completely new string into which the code characters can be
350        inserted.  This is a bad glitch in XPL and you probably won't
351        understand it unless you program in XPL for some time.
352     */
353
354     declare (i, j) fixed,
355             top fixed;
356
357     top = -1;
358     do i = 0 to dictionary.top;
359         if length(dictionary.string(i)) > 2 then
360             do;
361                 top = top + 1;
362                 dictionary.string(top) = dictionary.string(i);
363                 dictionary.count(top) = dictionary.count(i);
364             end;
365     end;
366     dictionary.top = top;
367
368     top = -1;
369     do i = 1 to "7f";   /* loop over eligible start characters */
370         if not character.used(i) then
371             do j = 1 to ”7f";   /* loop over seco nd characters */
372                 if top = dictionary.top then return;
373                 top = top + 1;
374                 dictionary.code(top) = ' ' || ' ';
375                 byte(dictionary.code(top),0) = i;
376                 byte(dictionary.code(top),1) = j;
377             end;
378     end;
379     dictionary.top = top;
380
381 end build.encoding.table;
382
383 compress.text: procedure;
384
385     /* Encoding works almost the same way as dictionary construction.
386        Here, though, the input stream is converted to output lines
387        as the encodings are found.  The loop runs for as long as
388        there is input.
389     */
390
391     declare output.buffer character,
392             index fixed;
393
394     input.buffer = '';
395     print '';
396     print '*** the compressed text ***';
397     print '';
398     call fill.input.buffer;
399     do while length(input.buffer) > 0;
400         input.length = input.length + length(input.buffer);
401         output.buffer = '';
402         do while length(input.buffer) > 0;
403             index = search.dictionary(input.buffer);
404             if index > -1 then
405                 do;
406                     output.buffer = output.buffer
407                                     || dictionary.code(index);
408                     dictionary.usage(index) = dictionary.usage(index) + 1;
409                     input.buffer = substr(input.buffer,
410                                           length(dictionary.string(index)));
411                 end;
412             else
413                 do;
414                     output.buffer = output.buffer
415                                     || substr(input.buffer,0,1);
416                     input.buffer = substr(input.buffer,1);
417                 end;
418         end;
419         output.length = output.length + length(output.buffer);
420         if print.encoding then print output.buffer;
421         call fill.input.buffer;
422     end;
423
424 end compress.text;
425
426 print.summary.statistics: procedure;
427
428     /* Summary statistics include a second printing of the search
429        statistics, the dictionary itself, and the compression rate.
430        In a working version, both the compressed text and the
431        dictionary would have also been printed on separate files for
432        re-expansion later.  Notice the complication to print a ratio
433        as a decimal in a language without floating point numbers.
434     */
435
436     declare i fixed,
437             ratio character;
438
439     print '';
440     print '*** compression statistics ***';
441     print '';
442     print 'code frequency   usage   string';
443     do i = 0 to dictionary.top;
444         print '' || dictionary.code(i) || ' '
445                  || i.format(dictionary.count(i), 9) || ' '
446                  || i.format(dictionary.usage(i), 9)
447                  || ' |' || dictionary.string(i) || '|';
448     end;
449     print '';
450     print '   characters in input = ' || input.length;
451     print '   characters in output = ' || output.length;
452     ratio = (1000*output.length)/input.length + 1000;
453     print '   compression rate = .' || substr(rat10,1);
454     print '   compares during dictionary construction = '
455                 || build.compares;
456     print '   compares during compression = '
457                 || compress.compares;
458     print '*** time checks in milleseconds ***';
459     print '   time to prepare = '
460                 || time.check(0) - time.check(1);
461     print '   time to build dictionary = '
462                 || time.check(1) - time.check(2);
463     print '   encoding table time = '
464                 || time.check(2) - time.check(3);
465     print '   compression time = '
466                 || time.check(3) - time.check(4);
467
468 end print.sum m ary.statist ics;
469
470 /* The main routine must assign the I/O units to files, initialize
471    needed variables, call the dictionary construction algorithm, build
472    the encoding table, and then encode the output.  Most of this work
473    is done in called procedures.
474 */
475
476 time.check(0) = compute.time;
477 call assign('text', source.file, "(1) 1000 0110");
478 call assign('compress', echo.file, "(1) 0010 1010");
479 print '*** begin the text compression. ***';
480 print '';
481
482 call prepare.the.program;
483 print.source = true; check.characters = true;
484 time.check(1) = compute.time;
485 call build.dictionary;
486 build.compares = search.compares;
487 time.check(2) = compute.time;
488 call build.encoding.table;
489
490 call assign('text', source.file, "(1) 1000 0110"); /* a rewind */
491 search.compares = 0;
492 print.source, check.characters = false;
493 print.encoding = true;
494 time.check(3) = compute.time;
495 call compress.text;
496 compress.compares = search.compares;
497
498 time.check(4) = compute.time;
499 call print.summary.statistics;
500
501 eof eof eof eof eof eof
