;for fasm assembler

D=9360  ;digits, 9360 is the limit for 64 KB ra-array
N=D*7/2

format ELF64 executable 3

segment readable executable

entry $
         xor esi,esi
         mov ecx,N/4   ;fill r-array
         mov rax,2000*0x1000100010001
         mov rdi,ra+2
         rep stosq

         mov [cv],cx
         mov [kv],N
.l0:     xor edi,edi          ;d <- 0
         mov si,[kv]
         add esi,esi            ;i <-k*2
         mov cx,10000           ;r[i]*10000, mul16x16
         jmp .l2

.l4:     sub edi,edx
         sub edi,eax
         shr edi,1
.l2:     movzx eax,[rsi+ra]     ; r[i]
         mul ecx
         add eax,edi
         mov edi,eax

         dec esi        ;b <- 2*i-1
         ;xor edx,edx
         div esi
         mov [rsi+ra+1],dx   ;r[i] <- d%b
         dec esi      ;i <- i - 1
         jne .l4

         mov eax,edi
         xor edx,edx
         div ecx
         add ax,[cv]  ;c + d/10000
         mov [cv],dx     ;c <- d%10000
         mov cx,ax
         call PR0000
         mov    edx,4
         mov    rsi,wb
	 mov	eax,1		; STDOUT
	 mov	rdi,rax		; sys_write
         syscall
         xor rsi,rsi
         xor ecx,ecx

         sub [kv],14      ;k <- k - 14
         jne .l0

.l5:    xor	edi,edi		; exit code 0
	mov	eax,60		; sys_exit
	syscall

PR0000:     ;prints cx
        mov rdi,wb
        mov bx,1000
	CALL .l0
        mov bx,100
	CALL .l0
        mov bx,10
	CALL .l0
	mov al,cl
.l2:	add al,'0'
        stosb
        retn

.l0:    mov al,0ffh
.l4:	inc al
        mov bp,cx
	sub cx,bx
	jnc .l4

	mov cx,bp
	jmp .l2

segment readable writeable
    align 8
cv  dw 0
kv  dw 0
wb  dw 0
ra  dw 0
    rw N
