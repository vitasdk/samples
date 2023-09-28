	.align 2
	.balign 4
	.text
	.cpu cortex-a9
	.arch armv7-a
	.syntax unified
	.thumb
	.thumb_func
	.fpu neon

	.align	2
	.global vitasdk_sample_math_eor
	.type   vitasdk_sample_math_eor, %function

vitasdk_sample_math_eor:
	eors r0, r1
	bx lr

	.global vitasdk_sample_math_test
	.type   vitasdk_sample_math_test, %function

	# make no export this function
	.hidden vitasdk_sample_math_test

vitasdk_sample_math_test:
	bx lr

	.data
