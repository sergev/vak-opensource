/*
memcpy_slow - Incredibly slow but correct memcpy.
Reads an entire qword from the source and dest, then writes
an entire qword for each byte copied.
example:
        source          dest
        v               v
________abcdefgh________stuvwxyz________
0123456701234567012345670123456701234567
read aligned qword from source (abcdefgh)
extract byte                   (a-------)
read aligned qword from dest   (stuvwxyz)
mask out dest byte             (-tuvwxyz)
combine byte with masked read  (atuvwxyz)
write aligned qword to dest    (atuvwxyz)
increment source/dest, decrement count, and repeat
*/

.section .text
.global memcpy_slow
memcpy_slow:
# $16 = dest
# $17 = src
# $18 = n
lda   $6,0($16)  # save dest for return
again:
beq   $18,done
lda   $8,1($31)  # $8 = 1 : source/dest increment
lda   $1,7($31)  # $1 = 7
eqv   $31,$1,$2  # $2 = ~7
and   $17,$1,$3  # $3 = source offset
and   $17,$2,$4  # $4 = aligned source
ldq   $0,0($4)   # source qword
extbl $0,$3,$5   # $5 = byte from offset
and   $16,$1,$3  # $3 = dest offset
and   $16,$2,$4  # $4 = aligned dest
ldq   $0,0($4)   # dest qword
mskbl $0,$3,$0   # masked dest qword
insbl $5,$3,$5   # shift source byte
or    $0,$5,$0   # combine
stq   $0,0($4)   # write to dest
addq  $16,$8,$16 # advance dest
addq  $17,$8,$17 # advance source
subq  $18,$8,$18 # decrease count
jmp   again
done:
lda   $0,0($6)  # return saved dest
ret

