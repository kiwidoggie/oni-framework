.intel_syntax noprefix
.text
.global	kernelRdmsr

kernelRdmsr:
	mov	ecx, edi
	rdmsr
	shl	rdx, 32
	or	rax, rdx
	ret
