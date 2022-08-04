#  By: R0B : witchdoctor.mdf@gmail.com 
#  http://mdf0.blogspot.com && irc.efnet #asm
#
#PURPOSE: This progi finds the max number in a data set
#
#INPUT: None from user but we declare the data set
#
#OUTPUT:
#
#VARIABLES:
# 	%edi - Hold index of data currently examined
# 	%ebx - Largest data item found
#	%eax - Current item examined
#MEMORY:
#	data_items - contains item data. 0 is used to terminate data
#

.section .data                                                         
data_items:
  .long 3,67,34,255,45,75,54,34,44,33,22,11,66,0
	
.section .text

.globl _start

_start:			      	# 32-bit instructions and registers
	
 movl $0, %edi			# move 0 into the index register
 movl data_items(,%edi,4), %eax	# load the first byte of data
 movl %eax, %ebx		# since this is the first item we see 
				# then it is the biggest

start_loop:			    # begin loop 

 cmpl $0, %eax			# check to see if we hit the end
 je end_loop
 incl %edi			    # load next value
 movl data_items(,%edi,4), %eax
 cmpl %ebx, %eax		# compare values
 jle start_loop			# jump to loop start if new value
				# is not bigger
 movl %eax, %ebx		# move value as largest
 jmp start_loop			# jump to loop start

# %ebx is the status code for the exit system call
# and it already has the max number
# exit() only returns an 8-bit value maximum (0-255) decimal
# i.e largest allowed exit status

end_loop:

 movl $1, %eax			# 1 is the exit() syscall
 int $0x80			    # linux interrupt
