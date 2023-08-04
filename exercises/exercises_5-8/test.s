	.text			# Define beginning of text section
	.global	_start		# Define entry _start

_start:
	la sp, stack_end	# prepare stack for calling functions

	li a0, 3
	call sum_squares

stop:
	j stop			# Infinite loop to stop execution

sum_squares:
	addi sp, sp, -16
	sw s0, 0(sp)
	sw s1, 4(sp)
	sw s2, 8(sp)
	sw ra, 12(sp)
	
	mv s0,a0	#参数i
	li s1, 0	#sum = 0
	li s2, 1	#j = 1
loop:
	mv a0,s2
	jal square
	add s1, s1, a0	#sum+=squre(i)
	addi s2, s2, 1	#j++
	bgtu s2, s0, end_loop
	
	j loop
end_loop:
	mv a0, s1

	lw s0, 0(sp)
	lw s1, 4(sp)
	lw s2, 8(sp)
	lw ra, 12(sp)
	addi sp, sp, 16

	ret

# int square(int num)
square:
	# prologue
	addi sp, sp, -8
	sw s0, 0(sp)
	sw s1, 4(sp)

	# `mul a0, a0, a0` should be fine,
	# programing as below just to demo we can contine use the stack
	mv s0, a0
	mul s1, s0, s0
	mv a0, s1

	# epilogue
	lw s0, 0(sp)
	lw s1, 4(sp)
	addi sp, sp, 8

	ret

	# add nop here just for demo in gdb
	nop

	# allocate stack space
stack_start:
	.rept 12
	.word 0
	.endr
stack_end:

	.end			# End of file
