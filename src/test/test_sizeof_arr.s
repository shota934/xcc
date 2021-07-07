	.text
	.global main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $48, %rsp
	movq $40,%rax
	pushq %rax
	movq $4,%rax
	movslq %eax,%rax
	movq %rax,%rcx
	popq %rax
	movq $0,%rdx
	cltd
	idivq %rcx
	movl %eax,-44(%rbp)
	movl -44(%rbp),%eax
	pushq %rax
	movq $10,%rax
	popq %rcx
	cmp %eax, %ecx
	sete %al
	movzb %al, %eax
	test %rax, %rax
	je .L0
	pushq %rdi
	.data
.L1:
	.string "OK\n"
	.text
	lea .L1(%rip), %rax
	pushq %rax
	popq %rdi
	subq $8,%rsp
	callq printf
	addq $8,%rsp
	popq %rdi
	movq $0,%rax
	movl %eax,-48(%rbp)
	jmp .L2
.L0:
	pushq %rdi
	.data
.L3:
	.string "NG\n"
	.text
	lea .L3(%rip), %rax
	pushq %rax
	popq %rdi
	subq $8,%rsp
	callq printf
	addq $8,%rsp
	popq %rdi
	movq $1,%rax
	movl %eax,-48(%rbp)
.L2:
	movl -48(%rbp),%eax
	addq $48, %rsp
	movq %rbp ,%rsp
	popq %rbp
	ret
	addq $48, %rsp
	movq %rbp ,%rsp
	popq %rbp
	ret
