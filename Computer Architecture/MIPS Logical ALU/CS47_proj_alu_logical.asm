.include "./cs47_proj_macro.asm"
.text
.globl au_logical
# TBD: Complete your project procedures
# Needed skeleton is given
#####################################################################
# Implement au_logical
# Argument:
# 	$a0: First number
#	$a1: Second number
#	$a2: operation code ('+':add, '-':sub, '*':mul, '/':div)
# Return:
#	$v0: ($a0+$a1) | ($a0-$a1) | ($a0*$a1):LO | ($a0 / $a1)
# 	$v1: ($a0 * $a1):HI | ($a0 % $a1)
# Notes:
#####################################################################
au_logical:
	# Frame store
	addi	$sp, $sp, -8
	sw	$fp, 8($sp)
	sw	$ra, 4($sp)
	addi	$fp, $sp, 8
	
	beq 	$a2, 0x2B, logical_add_init	# ADD
	beq 	$a2, 0x2D, logical_sub_init	# SUB
	beq 	$a2, 0x2A, logical_mul_div	# MUL
	beq 	$a2, 0x2F, logical_mul_div	# DIV
	
logical_add_init:	
	li	$a2, 0	# Carry In
	j	logical_add

logical_sub_init:
	not	$a1, $a1
	li	$a2, 1	# Carry In
	j	logical_add

logical_add:	
	move	$t0, $zero	# Index
	move	$v0, $zero	# Output, Sum
	
	logical_add_loop:
		extract_nth_bit($t1, $a0, $t0)	# A bit at index
		extract_nth_bit($t2, $a1, $t0)	# B bit at index
	
		xor	$t3, $t1, $t2	# A XOR B
		and	$t4, $t1, $t2	# A AND B
		xor	$t1, $a2, $t3	# CI XOR (A XOR B), Output bit at index
		and	$t2, $a2, $t3	# CI AND (A XOR B)
		or	$a2, $t2, $t4	# (CI AND (A XOR B)) OR (A AND B), Carry Out
		
		insert_nth_bit($v0, $t1, $t0, $t3)
		addi	$t0, $t0, 1	# Index = Index + 1
		bne	$t0, 32, logical_add_loop
	
	move	$v1, $a2	# Move final carry out to $v1
	j	logical_end
	
logical_mul_div:
	li	$t2, 31
	extract_nth_bit($t0, $a0, $t2)
	extract_nth_bit($t1, $a1, $t2)
	move	$t6, $a2	# Store op type
	xor	$t7, $t0, $t1	# Check for negative final result
	addi	$sp, $sp, -4	# Stack push
	sw	$t0, 4($sp)	# Store MSB of $a0 for div remainder sign
	move	$t5, $a1	# Rotate out second input
	twos_complement_if_neg()
	move	$a0, $t5	# Rotate in second input
	move	$t5, $v0	# Rotate out first output
	twos_complement_if_neg()
	move	$a0, $t5	# Rotate in first output for mul/div
	move	$a1, $v0	# Move second output for mul/div
	
	beq 	$t6, 0x2A, logical_mul	# Pass to mul
	j	logical_div		# Pass to div
	
	logical_mul:
		jal	logical_mul_unsigned	# Call to unsigned mul
		beqz	$t7, no_64bit		# Skip two's comp if not negative
	
		move	$a0, $v0		# Moves LO to $a0
		move	$a1, $v1		# Moves HI to $a1
		twos_complement_64bit()	# Returns two's comp of result in $v0, $v1
	
		no_64bit:
			addi	$sp, $sp, 4	# Adjust $sp ignoring pushed value
			j	logical_end
		
	logical_div:
		jal	logical_div_unsigned	# Call to unsigned div
		beqz	$t7, no_q		# Skips two's comp of quotient if not negative
		move	$t7, $v1		# Rotates out remainder to avoid overwrite
		move	$a0, $v0		# Moves quotient into $a0
		twos_complement()		# Finds two's comp of $a0
		move	$v1, $t7		# Rotates in remainder
		no_q:
			lw	$t7, 4($sp)	# Pulls sign definer for remainder from stack
			addi	$sp, $sp, 4	# Adjusts $sp for pop
			beqz	$t7, no_r	# Skips two's comp of remainder if not negative
			move	$t7, $v0	# Rotates out quotient to avoid overwrite
			move	$a0, $v1	# Moves remainder into $a0
			twos_complement()	# Finds two's comp of $a0
			move	$v1, $v0	# Rotates remainder to $v1
			move	$v0, $t7	# Rotates in quotient to $v0
			no_r:
				j	logical_end
	
logical_mul_unsigned:
	# Frame restore
	addi	$sp, $sp, -20
	sw	$fp, 20($sp)
	sw	$ra, 16($sp)
	sw	$s0, 12($sp)
	sw	$s1, 8($sp)
	sw	$s2, 4($sp)
	addi	$fp, $sp, 20
	
	li	$t6, 31		# For bit insert
	move	$t5, $zero	# Index
	move	$v0, $zero	# Output HI
	move	$s0, $a0	# Multiplicand store
	move	$s1, $a1	# Multiplier, LO store
	
	logical_mul_unsigned_loop:
		move	$s2, $v0	# HI store
		extract_nth_bit($a0, $s1, $zero) 	# $a0 = LO[0]
		bit_replicator()	# $v0 = rep($a0)
		move	$a0, $s0	# Multiplicand load
		and	$a0, $a0, $v0	# Multiplicand AND Replication = X
		move	$a1, $s2	# Load HI into $a1 for add
		or	$a2,$zero,0x2B	# Set $a2 for add
		jal 	au_logical	# Recurse for add, HI = HI + X
		srl	$s1, $s1, 1	# LO = LO >> 1
		extract_nth_bit($t0, $v0, $zero)	# $t0 = HI[0]
		insert_nth_bit($s1, $t0, $t6, $t3)	# LO[31] = $t0 = HI[0]
		srl	$v0, $v0, 1	# HO = HO >> 1
		addi	$t5, $t5, 1	# Index = Index + 1
		bne 	$t5, 32, logical_mul_unsigned_loop
	
	move	$v1, $v0	# Rotate HI into $v1
	move	$v0, $s1	# Move LO into $v0
	
	# Frame restore
	lw	$fp, 20($sp)
	lw	$ra, 16($sp)
	lw	$s0, 12($sp)
	lw	$s1, 8($sp)
	lw	$s2, 4($sp)
	addi	$sp, $sp, 20
	jr	$ra

logical_div_unsigned:
	# Frame store
	addi	$sp, $sp, -16
	sw	$fp, 16($sp)
	sw	$ra, 12($sp)
	sw	$s0, 8($sp)
	sw	$s1, 4($sp)
	addi	$fp, $sp, 16
	
	li	$t6, 31		# For bit insert
	move	$t5, $zero	# Index
	move	$s0, $a0	# Store Dividend, Output Quotient
	move	$s1, $a1	# Store Divisor
	move	$a0, $zero	# Output Remainder
	
	logical_div_unsigned_loop:
		sll	$a0, $a0, 1	# R = R << 1
		extract_nth_bit($t0, $s0, $t6)		# $t0 = Q[31]
		insert_nth_bit($a0, $t0, $zero, $t3)	# R[0] = $t0 = Q[31]
		sll	$s0, $s0, 1	# Q = Q << 1
		move	$a1, $s1	# Moving Divisor for sub
		or	$a2,$zero,0x2D	# Set $a2 for sub
		jal 	au_logical	# Recurse for sub, S = R - D
		bltz	$v0, div_skip	# S < 0?
		move	$a0, $v0	# R = S
		li	$t0, 1		# $t0 = 1
		insert_nth_bit($s0, $t0, $zero, $t3)	# Q[0] = $t0 = 1
		div_skip:
			addi	$t5, $t5, 1	# Index = Index + 1
			bne 	$t5, 32, logical_div_unsigned_loop
	
	move	$v0, $s0	# Move Quotient to $v0
	move	$v1, $a0	# Move remainder to $v1
	
	# Frame restore
	lw	$fp, 16($sp)
	lw	$ra, 12($sp)
	lw	$s0, 8($sp)
	lw	$s1, 4($sp)
	addi	$sp, $sp, 16
	jr	$ra
	
logical_end:
	# Frame restore
	lw	$fp, 8($sp)
	lw	$ra, 4($sp)
	addi	$sp, $sp, 8
	jr 	$ra
