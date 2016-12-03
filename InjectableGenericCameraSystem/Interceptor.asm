PUBLIC interceptor

_DATA SEGMENT
_DATA ENDS

_TEXT SEGMENT

EXTERN _matrixAddress: qword
EXTERN _matrixAddressInterceptionContinue: qword

interceptor PROC
	; Game jmps to this location due to the hook set in C function SetMatrixAddressInterceptorHook
	cmp byte ptr [rbx+038h], 0 							; could also check +20 or +24 for 0 if the above fails
	jne _gameOriginalCode
	mov [_matrixAddress], rbx							; intercept address of camera struct
_gameOriginalCode:
	movss dword ptr [rbx+090h], xmm0					; original statement
    movss xmm0, dword ptr [rsp+058h]					; original statement
	jmp qword ptr [_matrixAddressInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
interceptor ENDP

_TEXT ENDS
END