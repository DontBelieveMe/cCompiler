.section .text ; Begin the .text (code) section
.global _main

_main:
	mov eax, 123
	add eax, 10
	ret
