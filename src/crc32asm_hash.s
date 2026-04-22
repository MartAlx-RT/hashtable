.globl	tbl_hash
.text

tbl_hash:
	xorq	%rax, %rax
	movb	(%rdi), %dl
	testb	%dl, %dl
	jz	exit

	incq	%rdi
loop:
	movb	(%rdi), %dl
	crc32	%dl, %eax	# while([%rdi++] != 0)	%rax = crc32(%rax)
	incq	%rdi
	testb	%dl, %dl
	jnz	loop

exit:
	ret
