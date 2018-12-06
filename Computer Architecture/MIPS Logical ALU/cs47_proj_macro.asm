#<------------------ MACRO DEFINITIONS ---------------------->#

        .macro extract_nth_bit($out, $input, $shift)
	srlv	$out, $input, $shift
	and	$out, 1
	.end_macro
	
	.macro insert_nth_bit($out, $input, $shift, $temp)
	li	$temp, 1
	sllv	$temp, $temp, $shift
	not	$temp, $temp
	and	$out, $out, $temp
	sllv	$input, $input, $shift
	or	$out, $out, $input
	.end_macro
	
	.macro twos_complement_if_neg()
	move	$v0, $a0
	bgtz	$a0, if_neg_end
	twos_complement()
	if_neg_end:
	.end_macro 
	
	.macro twos_complement()
	not	$a0, $a0	# Invert input
	li	$a1, 1		# For 2's comp
	or	$a2,$zero,0x2B	# Set $a2 for add
	jal 	au_logical	# Recurse for add
	.end_macro
	
	.macro twos_complement_64bit()
	move	$t5, $a1	# Store upper input
	twos_complement()
	not	$a0, $t5	# Invert upper input into $a0
	move	$t5, $v0	# Rotate out lower output
	move	$a1, $v1	# Add carry out from lower
	or	$a2,$zero,0x2B	# Set $a2 for add
	jal 	au_logical	# Recurse for add
	move	$v1, $v0	# Rotate upper output
	move	$v0, $t5	# Restore lower output
	.end_macro
	
	.macro bit_replicator()
	move	$v0, $zero
	beqz	$a0, replicate_zero
	not	$v0, $v0
	replicate_zero:
	.end_macro
	
