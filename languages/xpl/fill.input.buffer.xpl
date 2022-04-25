132 fill.input.buffer: procedure;
133
134     /* If input.buffer is empty, then this routine tries to read a
135        line from the source.file.  The line read goes into
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
148     do i = 0 to length(input.buffer)-1;
149         character.used(byte(input.buffer,i)) = true;
150     end;
151
152 end fill.input.buffer;
