#include <asm/pal.h>
#include <asm/unistd.h>

#define AF_INET 2
#define SOCKET_STREAM 1
#define SOL_SOCKET 1
#define SO_REUSEADDR 4

.arch ev4
.set noreorder
.set nomacro
.set noat
.set volatile

.section .rodata
$message:

.section .text
.align 2
.globl _start

_start:

#define frame$accept_addr       0
#define frame$bind_addr        16
#define frame$accept_addr_size 32
#define frame$server_fd        40
#define frame$client_fd        48
#define frame$client_state     56
#define frame$sockopt_value    64
#define frame$client_buffer    128
#define client_buffer_size     64

/* setup gp */
	bsr  $27,0 
	ldah $29,0($27)!gpdisp!1
	lda  $29,0($29)!gpdisp!1

/* setup stack frame */
	lda  $sp,-256($sp)

/* create socket */
	lda  $0, __NR_socket($31)
	lda  $16,AF_INET($31)
	lda  $17,SOCKET_STREAM($31)
	lda  $18,0($31)
	callsys
	stq  $0,frame$server_fd($sp)

/* set reuseaddr */
	ldq  $16,frame$server_fd($sp)
	lda  $17,SOL_SOCKET($31)
	lda  $18,frame$sockopt_value($sp)
	lda  $0,1($31)
	stl  $0,0($18)
	lda  $19,4($31)
	lda  $0, __NR_setsockopt($31)
	callsys

/* setup sockaddr_in and bind */
	ldah $0,80*256($31)
	lda  $0,AF_INET($0)
	stl  $0,frame$bind_addr($sp)
	lda  $0,0($31)
	stl  $0,frame$bind_addr+4($sp)
	stq  $0,frame$bind_addr+8($sp)
	lda  $0, __NR_bind($31)
	ldq  $16,frame$server_fd($sp)
	lda  $17,frame$bind_addr($sp)
	lda  $18,16($31)
	callsys

/* listen */
	lda  $0, __NR_listen($31)
	ldq  $16,frame$server_fd($sp)
	lda  $17,1($31)
	callsys

/* accept */
	lda  $0, __NR_accept($31)
	ldq  $16,frame$server_fd($sp)
	ldq  $17,frame$accept_addr($sp)
	ldq  $18,frame$accept_addr_size($sp)
	callsys
	stq  $0,frame$client_fd($sp)

/* read client */
	lda  $0, __NR_read($31)
	ldq  $16,frame$server_fd($sp)
	ldq  $17,frame$client_buffer($sp)
	ldq  $18,client_buffer_size($31)
	callsys




	ldah $17,$message($29)!gprelhigh  /* a1 = message (relative to gp) */
	lda  $17,$message($17)!gprellow
	lda  $18,6($31)                   /* a2 = 6 bytes (write length) */

	lda  $0, __NR_exit($31)           
	lda  $16,0($31)                   /* a0 = exitcode 0 */
	callsys                           /* do syscall specified in v0 (exit) */

