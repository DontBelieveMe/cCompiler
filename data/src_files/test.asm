.model  flat

public  _main

_text   segment

func proc
	mov eax, 1234
func endp

_main   proc
	call func
	ret 0
_main   endp
_text   ends
end _main
