.section .text ; Begin the .text (code) section
.global _main

setupInputs:
	mov eax, 20
	mov ebx, 30
	ret

addInputs:
	mov ecx, eax
	add ecx, ebx 
	mov eax, ecx
	ret

_main:
	call setupInputs
	call addInputs
	sub eax, 20
	ret
