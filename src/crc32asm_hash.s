.globl	tbl_crc32asm_hash
.text

tbl_crc32asm_hash:
	xorq	%rax, %rax
	movb	(%rdi), %dl
	testb	%dl, %dl
	jz	exit

	incq	%rdi
loop:
	crc32	%dl, %eax	# while([%rdi++] != 0)	%rax = crc32(%rax)
	movb	(%rdi), %dl
	incq	%rdi
	testb	%dl, %dl
	jnz	loop

exit:
	ret
