	.text			# Define beginning of text section
	.global	_start		# Define entry _start

_start:		
	li x7, 1
	li x8, 2
	li x9, 3
	li x10, 4
	
	add x5,x7,x8
	add x6,x9,x10
	
	sub x5,x5,x6

stop:
	j stop			# Infinite loop to stop execution

	.end			# End of file
