	.data
	string: .asciz "hello,world!"
	.text			# Define beginning of text section
	.global	_start		# Define entry _start

_start:		
	li x5, 0	#cnt
	la x6, string
loop:
	lb x7,0(x6)
	beqz x7,stop

	addi x5,x5,1
	addi x6,x6,1
	j loop

stop:
	j stop			# Infinite loop to stop execution

	.end			# End of file
