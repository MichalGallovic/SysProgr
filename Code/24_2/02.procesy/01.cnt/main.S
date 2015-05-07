	.file	"main.c"
.globl cnt
	.bss
	.align 4
	.type	cnt, @object
	.size	cnt, 4
cnt:
	.zero	4
	.text
.globl count
	.type	count, @function
count:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	$0, -4(%ebp)
	jmp	.L2
.L3:
	movl	cnt, %eax
	movl	%eax, -8(%ebp)
	addl	$1, -8(%ebp)
	movl	-8(%ebp), %eax
	movl	%eax, cnt
	addl	$1, -4(%ebp)
.L2:
	cmpl	$99999, -4(%ebp)
	jle	.L3
	movl	$0, %eax
	leave
	ret
	.size	count, .-count
	.section	.rodata
.LC0:
	.string	"main.c"
.LC1:
	.string	"err == 0"
.LC2:
	.string	"ERROR: cnt = %i\n"
.LC3:
	.string	"OK: cnt = %i\n"
	.text
.globl main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	subl	$32, %esp
	movl	$0, 12(%esp)
	movl	$count, 8(%esp)
	movl	$0, 4(%esp)
	leal	24(%esp), %eax
	movl	%eax, (%esp)
	call	pthread_create
	movl	%eax, 28(%esp)
	cmpl	$0, 28(%esp)
	je	.L6
	movl	$__PRETTY_FUNCTION__.2298, 12(%esp)
	movl	$26, 8(%esp)
	movl	$.LC0, 4(%esp)
	movl	$.LC1, (%esp)
	call	__assert_fail
.L6:
	movl	$0, 12(%esp)
	movl	$count, 8(%esp)
	movl	$0, 4(%esp)
	leal	20(%esp), %eax
	movl	%eax, (%esp)
	call	pthread_create
	movl	%eax, 28(%esp)
	cmpl	$0, 28(%esp)
	je	.L7
	movl	$__PRETTY_FUNCTION__.2298, 12(%esp)
	movl	$28, 8(%esp)
	movl	$.LC0, 4(%esp)
	movl	$.LC1, (%esp)
	call	__assert_fail
.L7:
	movl	24(%esp), %eax
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	pthread_join
	movl	20(%esp), %eax
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	pthread_join
	movl	cnt, %eax
	cmpl	$200000, %eax
	je	.L8
	movl	cnt, %ecx
	movl	$.LC2, %edx
	movl	stderr, %eax
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	fprintf
	jmp	.L9
.L8:
	movl	cnt, %ecx
	movl	$.LC3, %edx
	movl	stderr, %eax
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	fprintf
.L9:
	movl	$0, %eax
	leave
	ret
	.size	main, .-main
	.section	.rodata
	.type	__PRETTY_FUNCTION__.2298, @object
	.size	__PRETTY_FUNCTION__.2298, 5
__PRETTY_FUNCTION__.2298:
	.string	"main"
	.ident	"GCC: (Debian 4.4.5-8) 4.4.5"
	.section	.note.GNU-stack,"",@progbits
