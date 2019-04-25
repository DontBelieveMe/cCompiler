.model  flat

public  _main

_bobbythesecond segment
	_bob proc
		mov eax, 50
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
		ret
	_main endp
_text ends

end _main
