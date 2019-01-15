.section .text ; Begin the .text (code) section
.global _main

_main:
	mov ebx, 50
	mov ecx, 50
	mov edx, ebx
	add edx, ecx

	mov ebx, 9
	sub edx, ebx
	mov eax, edx
	ret
