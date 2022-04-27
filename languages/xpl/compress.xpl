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
/* only sim ple clearing of the dictionary, the character record,
the statistics, and a few scalars is required.
112
113 114 v
115
116 declare i fixed;
117
do i = 0 to dictionary.size;
dictionary.string(i), dictionary.code(i) = "
dictionary.count(i), dictionary.usage(i) = 0;
118
119
120
end;
dictionary.top = -1;
121
122
123
do i = 0 to "ff"; character.used(i) = false; end;
input. buffer = ";
124
125
126
search.co m pares = 0;
input.leng th, output.leng th = 0;
127
128
129
130 end prepare.the.pro g ram ;
131
132 fill.input.buffer: procedure;
133
/* if input.buffer is em pty, then this routine tries to read a
line fro m the so urce.file. the line read goes into
input.buffer w ith a null line the sig nal fo r end of file. if
flag print.so urce is on, then the input is echo ed. if flag
check.characters is on, the input is scanned fo r character
usage.
134
135
136
137
138
139
140 v
141
142 declare i fixed;
143
182 compressed solutions
144 145 sssrsstos.--*
146
147
148
149
150 end; r)) = true;
151
152 end fill.input.buffer;
154 coalescence.thresho ld
153
155 : pro cedure ffxed;
156 /* this pro cedure calculates
tw o dictionary entries
that the entries
of the ratio of
the thresho ld fo r
into one.
have freq uencies
space rem aining
157 coalescing
the requirem ent is
greater than the
in the dictionary.
158 here,
159
160 v recipro cal 161
162 return dictionary.
164 end coalescence.threshold;
166 deletion.threshold;
/* jhis functio n returns the thresho rn
to be retained in the dictionary
in this versio n, the freq uency m ust
rounded up m ean freq uency.
163 size/(dictio nary.size-dictio nary.to p+1) + 1;
167 ; pro cedure fixed;
168
169 necessary fo r
at cleanup tim e.
greater than
170 an entry
171 the 172 v 173
174 declare sum fixed,
175 i fixed;
176
177 sum = 0;
do i s
sum =
178 0 to dictionary. to p;
179 sum + dictio nary.
[80 end; co unt(i);
81 return sum/(dictio nary.
82 to p+1) + 1;
8843 end deletio n.thresho ld
85 first.co unt:
;
96 pro cedure fixed;
37 38 7 ithilfpus0e^sn^™ 'd?mo°s.a c0alesced
19 v entry when
10
1 return l; /* currently give a count of 1. v
183 compressed solutions
192
193 end first.co unt;
194
195 search.dictio nary: pro cedure(test.string ) fixed;
196
/* this functio n searches the dictionary fo r the lo ng est m atch
if no m atch is
fo und, the routine returns -1 as value; if a m atch is fo und,
the index of the m atch is returned as value.
197
w ith the head of 198 the arg um ent test.string .
199
200
201
this routine perfo rm s a sim ple linear search of the dictionary
fro m the zero th entry to the entry dictionary.top. if an
entry's leng th is lo ng er than the lo ng est current m atch and
still no lo ng er than the arg um ent, then the entry is m atched
ag ainst the arg um ent. eq uality w ill cause the m atch to be
updated. notice that by starting the index at -1, the return
value w ill be pro per even if no m atch is fo und.
202
203
204
205
206
207
208
209 */
210
211 declare test.string character;
212
index fixed,
(m atch.leng th, arg .leng th, entry.leng th) fixed,
i fixed;
213 declare
214
215
216
index = -1;
m atch.length = 0;
arg.leng th = leng th(test.string );
217
218
219
220
do i = 0 to dictionary.top;
entry.leng th = leng th(dictio nary.string (i));
if entry.length > m atch.length
& entry.leng th <= arg .leng th then
if dictionary.string(i)
= substr(test.string,0,entry.length) then
221
222
223
224
225
226
227 do ;
index = i;
m atch.length = entry.leng th;
228
229
230 end;
end;
search.co m pares = search.co m pares + dictionary.top + 1;
return index;
231
232
233
234
235 end search.dictio nary;
236
237 clean.dictionary: procedure;
238
184 com pressed so lutions
/* clean.dictionary elim inates at least one lo w freq uency entry
fro m the dictionary and restores the sm aller dictionary to
the fo rm at it had befo re cleaning.
the w hile lo o p surro und in g.the bo dy of the pro cedure guarantees
that at least one entry is deleted fro m the dictionary befo re
return.
an entry, the thresho ld is increm ented until so m ething is
deleted.
239
240
241
242
243
244 if the in itial thresho ld is not high enoug h to delete
245
246
247
248 the dictionary is just a linear table w ith no structure so
entries can be deleted by pushing the retained entries to w ard
the zero end of the table overw riting the rem oved entries.
249
250
251 */
252
253 declare i fixed,
thresho ld fixed,
old.top fixed,
new .top fixed;
254
255
256
257
250 old.top a dictionary.top;
thresho ld = delet io n.thresho ld;
do w hile old.top = dictionary.top;
new .top = -1;
do i = 0 to dictionary.top;
if dictionary.count(i) >= thresho ld then
259
260
261
262
263
264 do;
265 new .top = new .top + 1;
dictionary.string (new .to p) = dictionary.string(i);
dictionary.count(new .top) = dictionary.count(i);
266
267
268 end;
269 end;
dictionary.top = new .top;
thresho ld = thresho ld + 1;
270
271
272 end;
273
274 end clean.dictionary;
275
276 build.entry: pro cedure(entry.string ) fixed;
277
/* build.entry adds entry.string to the dictionary w ith a count
of zero and returns as value the index of the. new entry.
278
279
280
281 because the dictionary is searched linearly, the new entry
•can sim ply be added at the end. the only requirem ent is that
the dictionary m ay need to be cleaned befo re the new entry
can be added.
282
283
284
285 v
286
185 compressed solutions
287 declare entry.string character;
288
if dictionary.to p+2 >= dictionary.size
then call clean.dictionary;
dictionary.top = dictionary.top + 1;
dictionary.string(dictionary.top) = entry.string ;
dictionary.count(dictionary.top) = 0;
return dictionary.top;
289
290
291
292
293
294
295
296 end build.entry;
297
298 build.dictio nary; pro cedure;
299
/* dictionary construction continues until the input routine
the test fo r a null string is
300
fails to return any data.
sim ple if w e check the leng th ag ainst zero .
dictionary.search routine returns -1 if no m atch is fo und and
then the first character of the input is fo rced as the m atch
301
302 the
303
304
notice that the actual string
coalescence
305 and into the dictionary.
m atched is picked up fro m the dictionary entry.
takes place as necessary, the m atch is rem em bered, and the
input prepared fo r ano ther cycle.
306
307
308
309 v
310
(m atch, last.m atch) character,
(co unt, last.co unt) fixed,
index fixed,
thresho ld fixed;
311 declare
312
/* the entry lo catio n */
/ coalescence thresho ld */
313
314
315
last.m atch = "
last.co unt = 0;
316
317
318
do w hile true;
call fill.input.buffer;
if leng th(input.buffer) = 0 then return;
index = search.dictio nary(input.buffer);
if index = -1
then index = build.entry(substr(input.buffer,0,1));
m atch = dictionary.string(index);
count, dictionary.count(index) = dictionary.count(index) + 1;
thresho ld = coalescence.thresho ld;
if count >= thresho ld & last.co unt >= thresho ld then
dictionary.count(build.entry(last.m atch||m atch))=first.count;
last.m atch = m atch;
last.co unt ^ count;
input.buffer = substr(input.buffer, leng th (m atch).);
319
320
321
322
323
324
325
326
327
328
329
330
331
332
333 end;
334
186 compressed solutions
335 end build.dictionary;
336
337 build.enco ding .table: procedure;
338
/ code construction has tw o steps. in the first, every
dictionary entry of leng th tw o or one is thro w n out because
there is no po int in replacing such string s w ith a tw o
character code. second, codes are assig ned using characters
unseen in the text as starters. w hen such pairs run out,
no m ore codes are assig ned even if there are m ore entries in
the dictionary.
339
340
341
342
343
344
345
346
347 notice the lines belo w w hich construct the dictionary code.
the apparently senseless catenation of tw o blanks builds a
com pletely new string into w hich the code characters can be
inserted.
understand it unless yo u pro g ram in xpl fo r so m e tim e.
348
349
350 this is a bad glitch in xpl and you probably won't
351
352 v
353
354 declare (i, j) fixed,
355 to p fixed;
356
357 to p = -1;
do i = 0 to dictionary.top;
if leng th(dictio nary.string (i)) > 2 then
358
359
360 do;
361 to p = to p + 1;
dictionary.string(top) = dictionary.string(i);
dictionary.count(top) = dictionary.count(i);
362
363
364 end;
365 end;
366 dictio nary.to p = to p;
367
368 to p = -1;
369 do i = 1 to "7f"; /* lo o p o ver elig ible start characters /
if not character.used(i) then
do j = 1 to ”7f";
370
371 /* lo o p over seco nd characters
if to p = dictionary.top then return;
to p = to p + 1;
dictionary.code(top) = '
byte(dictio nary.co de(to p),0) = i;
byte(dictio nary.co de(to p),1) = j;
*/
372
373
374 ii'';
375
376
377 end;
378 end;
379 dictio nary.to p = to p;
380
381 end build.encoding.table;
382
187 compressed solutions
383 compress.text: procedure;
384
/* enco ding w orks alm o st the sam e w ay as dictionary construction.
here, tho ug h, the input stream is converted to output lines
the lo o p runs fo r as lo ng as
385
386
as the enco ding s are fo und.
there is input.
387
388 389 v
390
declare output.buffer character,
index fixed;
391
392
393
input.buffer =
print ";
print
print
call fill.input.buffer;
do w hile leng th(input.buffer) > 0;
input.leng th = input.leng th + leng th(input.buffer);
output. buffer =
do w hile leng th(input.buffer) > 0;
index = search.dictio nary(input.buffer);
if index > -1 then
394
395
396 ' *** the co m pressed text ***';
397 ;
398
399
400
401
402
403
404
405 do;
406 output.buffer = output.buffer
m dictionary.code(index);
dictionary.usage(index) = dictionary.usage(index) + 1;
input.buffer = substr(input.buffer,
407
408
409
410 leng th(dictio nary.string (index)));
411 end;
412 else
413 do ;
414 o utput.buffer = o utput.buffer
|| substr(input.buffer,0,1);
input.buffer = substr(input.buffer,1);
415
416
417 end;
end;
output.leng th = output.leng th + leng th(o utput.buffer);
if print.enco ding then print output.buffer;
call fill.input.buffer;
418
419
420
421
422 end;
423
424 end com press.text;
425
426 print.sum m ary.statistics: pro cedure;
427
188 com pressed so lutio ns
/* sum m ary statistics include a seco nd printing of the search
statistics, the dictionary itself, and the com pression rate.
in a w orking versio n, bo th the com pressed text and the
dictionary w ould have also been printed on separate files fo r
re-expansion later.
as a decim al in a lang uag e w ithout flo ating po int num bers.
428
429
430
431
432 notice the com plication to print a ratio
433
434 v
435
436 declare i fixed,
437 ratio character;
438
439 print ";
print '*** com pression statistics ***';
print ";
print 'co de freq uency usage string ';
do i = 0 to dictionary.top;
print ''ll dictionary.code(i) || ' '
440
441
442
443
444
445 || i.fo rm at(dictio nary.co unt(i), 9) || ' '
|| i.fo rm at(dictio nary.usag e(i), 9)
ii ' i' ii dictionary.string(i) || '|';
446
447
448 end;
print
print '
print '
449
450 characters in input = ' || input.leng th;
451 characters in o utput = ' || o utput.leng th;
452 ratio = (1000*o utput.leng th)/input.leng th + 1000;
453 print ' co m pressio n rate = .' || substr(rat10,1);
454 print ' co m pares during dictio nary co nstructio n = '
455 || build.co m pares;
456 print ' co m pares during co m pressio n = '
457 ii com press.com pares;
print '*** tim e checks in m illeseconds * *';
print '
458
459 tim e to prepare = '
|| tim e.check(0) - tim e.check(1);
tim e to build dictionary = '
460
461 print '
462 || tim e.check(1) - tim e.check(2);
enco ding table tim e = '
|| tim e.check(2) - tim e.check(3);
com pression tim e = '
|| tim e.check(3) - tim e.check(4);
463 print '
464
465 print '
466
467
468 end print.sum m ary.statist ics;
469
470 /* the m ain routine m ust assig n the i/o units to files, in itialize
needed variables, call the dictionary construction alg orithm , build
the enco ding table, and then enco de the output. m ost of this w ork
is done in called pro cedures.
471
472
473
474 */
475
189 compressed solutions
476 time.check(o) = compute.time;
477 call assig n('text', so urce.file, "(1) 1000 0110");
478 call ass ig n('co m press', echo .file, "(1) 0010 1010");
479 print '*** beg in the text com pression.
480 print "}
* * *' •
481
482 call prepare.the.pro g ram ;
483 print.so urce = true; check.characters = true;
484 tim e.check(1) = com pute.tim e;
485 call build.dictio nary;
486 build.co m pares = search.co m pares;
487 tim e.check(2) = com pute.tim e;
488 call build.enco ding .table;
489
490 call assig n('text', so urce.file, "(1) 1000 0110"); /* a rew ind /
491 search.co m pares = 0;
492 print.so urce, check.characters = false;
493 print.enco ding = true;
494 tim e.check(3) = com pute.tim e;
495 call com press.text;
496 com press.com pares = search.com pares;
497
498 tim e,check(4) = com pute.tim e;
499 call print.sum m ary.stat ist ics;
500
501 eo f eo f eo f eo f eo f eo f
