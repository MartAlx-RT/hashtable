.globl	key_cmp

.text

# %rdi - key1
# %rsi - key2
key_cmp:
	vlddqu	(%rdi), %ymm0		# %ymm0 = low1
	vlddqu	(%rsi), %ymm1		# %ymm1 = low2
	vpsubb	%ymm1, %ymm0, %ymm0	# %ymm0 = cmp(low1, low2)
	vptest	%ymm0, %ymm0
	jnz	exit

	vlddqu	32(%rdi), %ymm0		# %ymm0 = hi1
	vlddqu	32(%rsi), %ymm1		# %ymm0 = hi2
	vpsubb	%ymm1, %ymm0, %ymm0	# %ymm0 = cmp(hi1, hi2)
	vptest	%ymm0, %ymm0

exit:
	setnz	%al
	movzbq	%al, %rax
	ret
