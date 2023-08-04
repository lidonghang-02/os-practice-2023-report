	.text			# Define beginning of text section
	.global	_start		# Define entry _start

_start:		
	li x7, 1
	li x8, 2
	li x9, 3
	
	add x5,x7,x8
	sub x6,x5,x9

stop:
	j stop			# Infinite loop to stop execution

	.end			# End of file
