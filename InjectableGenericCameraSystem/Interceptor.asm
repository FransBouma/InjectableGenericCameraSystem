PUBLIC interceptor

_DATA SEGMENT
_DATA ENDS

_TEXT SEGMENT
EXTERN _matrixAddress: qword
EXTERN _hostImageAddress: qword

interceptor PROC
	; Game jmps to this location due to the hook set in set_intercept
	cmp byte ptr [rbx+038h], 0 				; could also check +20 or +24 for 0 if the above fails
	jne _gameOriginalCode
	mov [_matrixAddress], rbx				; intercept address of camera struct
_gameOriginalCode:
	movss dword ptr [rbx+090h], xmm0		; original statement
    movss xmm0, dword ptr [rsp+058h]        ; original statement

;>>>>>>>>>>>>>> DOESN'T WORK!
	jmp qword ptr [_hostImageAddress]+041C5BA8h	; jmp back into the original game code: 041C5B9A + 0Eh.
interceptor ENDP


_TEXT ENDS

END