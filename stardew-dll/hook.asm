.data
money QWORD ?

.code
get_money_hook proc
	mov money, rax
	mov eax, dword ptr [rax+76]
	add rsp, 40
	ret
	get_money_hook endp

get_money proc
	mov rax, money
	mov rax, qword ptr [rax+76] ; eax, dword
	ret
	get_money endp

set_money proc        
    mov rax, money              
    mov qword ptr [rax+76], rcx
	ret
	set_money endp

end