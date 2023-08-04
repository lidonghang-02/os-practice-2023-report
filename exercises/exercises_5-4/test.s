	.text			# Define beginning of text section
	.global	_start		# Define entry _start

_start:		
	li x7, 0x87654321
	li x8,0xffff	
	and x5,x7,x8
	
	srl x9,x7,16
	and x6,x9,x8

stop:
	j stop			# Infinite loop to stop execution

	.end			# End of file
