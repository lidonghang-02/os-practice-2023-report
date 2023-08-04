	.data
	struct: .word 0,0
	.text			# Define beginning of text section
	.global	_start		# Define entry _start

.macro set_struct s,a,b
	sw \a, 0(\s)
	sw \b, 4(\s)
.endm
.macro get_struct s,a,b
	lw \a, 0(\s)
	lw \b, 4(\s)
.endm

_start:		
	li x5, 0x12345678	#a
	li x6, 0x87654321	#b
	la x7,struct
	
	set_struct x7,x5,x6
	
	li x5,0
	li x6,0
	
	get_struct x7,x5,x6

stop:
	j stop			# Infinite loop to stop execution

	.end			# End of file
