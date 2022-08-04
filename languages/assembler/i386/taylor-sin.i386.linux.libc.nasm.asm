bits 32

section .text

extern printf
extern atof
extern atoi
extern puts
global main

taylor_sin:
  push eax
  push ecx

  ; input :
  ;  st(0) = x, value to approximate sin(x) of
  ;  [esp+12] = number of taylor series terms

  ; variables we'll use :
  ; s = sum of all terms (final result)
  ; x = value we want to take the sin of
  ; fi = factorial index (1, 3, 5, 7, ...)
  ; fc = factorial current (1, 6, 120, 5040, ...)
  ; n = numerator of term (x, x^3, x^5, x^7, ...)

  ; setup state for each iteration (term)
  fldz ; s x
  fxch st1 ; x s
  fld1 ; fi x s
  fld1 ; fc fi x s
  fld st2 ; n fc fi x s

  ; first term
  fld st1 ; fc n fc fi x s
  fdivr st0,st1 ; r n fc fi x s
  faddp st5,st0 ; n fc fi x s

  ; loop through each term
  mov ecx,[esp+12] ; number of terms
  xor eax,eax ; zero add/sub counter

loop_term:
  ; calculate next odd factorial
  fld1 ; 1 n fc fi x s
  faddp st3 ; n fc fi x s
  fld st2 ; fi n fc fi x s
  fmulp st2,st0
  fld1 ; 1 n fc fi x s
  faddp st3 ; n fc fi x s
  fld st2 ; fi n fc fi x s
  fmulp st2,st0 ; n fc fi x s

  ; calculate next odd power of x
  fmul st0,st3 ; n*x fc fi x s
  fmul st0,st3 ; n*x*x fc fi x s

  ; divide power by factorial
  fld st1 ; fc n fc fi x s
  fdivr st0,st1 ; r n fc fi x s

  ; check if we need to add or subtract this term
  test eax,1
  jnz odd_term
  fsubp st5,st0 ; n fc fi x s
  jmp skip
odd_term:
  ; accumulate result
  faddp st5,st0 ; n fc fi x s
skip:
  inc eax ; increment add/sub counter
  loop loop_term

  ; unstack work variables
  fstp st0
  fstp st0
  fstp st0
  fstp st0

  ; result is in st(0)

  pop ecx
  pop eax

  ret

main:

  ; check if we have 2 command-line args
  mov eax, [esp+4]
  cmp eax, 3
  jnz error

  ; get arg 1 - value to calc sin of
  mov ebx, [esp+8]
  push dword [ebx+4]
  call atof
  add esp, 4

  ; get arg 2 - number of taylor series terms
  mov ebx, [esp+8]
  push dword [ebx+8]
  call atoi
  add esp, 4

  ; do the taylor series approximation
  push eax
  call taylor_sin
  add esp, 4

  ; output result
  sub esp, 8
  fstp qword [esp]
  push format
  call printf
  add esp,12

  ; return to libc
  xor eax,eax
  ret

error:
  push error_message
  call puts
  add esp,4
  mov eax,1
  ret

section .data

error_message: db "syntax: <x> <terms>",0
format: db "%0.10f",10,0

