	.data
	array: .word 0x11111111,0xffffffff
	.text			# Define beginning of text section
	.global	_start		# Define entry _start

_start:		
	
	la x5, array
	lw x6, 0(x5)
	lw x7, 4(x5)

stop:
	j stop			# Infinite loop to stop execution

	.end			# End of file
