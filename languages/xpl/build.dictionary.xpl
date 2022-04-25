298 build.dictionary: procedure;
299
300     /* Dictionary construction continues until the input routine
301        fails to return any data. The test for a null string is
302        simple if we check the length against zero. The
303        dictionary.search routine returns -1 if no match is found and
304        then the first character of the input is forced as the match
305        and into the dictionary. Notice that the actual string
306        matched is picked up from the dictionary entry. Coalescence
307        takes place as necessary, the match is remembered, and the
308        input prepared for another cycle.
309     */
310
311     declare (match, last.match) character,
312             (count, last.count) fixed,
313             index fixed,            /* the entry location */
314             threshold fixed;        /* coalescence threshold */
315
316     last.match = ''
317     last.count = 0;
318
319     do while true;
320         call fill.input.buffer;
321         if length(input.buffer) = 0 then return;
322         index = search.dictionary(input.buffer);
323         if index = -1
324         then index = build.entry(substr(input.buffer,0,1));
325         match = dictionary.string(index);
326         count, dictionary.count(index) = dictionary.count(index) + 1;
327         threshold = coalescence.threshold;
328         if count >= threshold & last.count >= threshold then
329         dictionary.count(build.entry(last.match||match))=first.count;
330         last.match = match;
331         last.count = count;
332         input.buffer = substr(input.buffer, length(match));
333     end;
334
335 end build.dictionary;
