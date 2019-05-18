.model  flat

public  _main

_bobbythesecond segment
	_bob proc
		mov eax, 50
		mov ebx, 50
		mov eax, ebx
	_bob endp
_bobbythesecond ends

_text   segment
	func proc
		mov eax, 2
		ret
	func endp

	_main proc
		call func
		add eax, 10
		mov eax, 50
		mov ebx, 50
		mov eax, ebx
		ret
	_main endp
_text ends

end _main
