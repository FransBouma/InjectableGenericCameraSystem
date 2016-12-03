PUBLIC cameraAddressInterceptor
PUBLIC cameraWriteInterceptor

_DATA SEGMENT
_DATA ENDS

_TEXT SEGMENT

EXTERN _cameraStructAddress: qword
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _matrixWriteInterceptionContinue: qword
EXTERN _cameraEnabled: byte

cameraAddressInterceptor PROC
	; Game jmps to this location due to the hook set in C function SetCameraStructInterceptorHook
	cmp byte ptr [rbx+038h], 0 							; check if this is the camera in rbx. For this game: Check with a 0-check. Could also check +20 or +24 for 0 if the above fails
	jne originalCode
	mov [_cameraStructAddress], rbx						; intercept address of camera struct
originalCode:
	movss dword ptr [rbx+090h], xmm0					; original statement
    movss xmm0, dword ptr [rsp+058h]					; original statement
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraAddressInterceptor ENDP


cameraWriteInterceptor PROC
	; Game jmps to this location due to the hook set in C function SetMatrixWriteInterceptorHook. 
	; first check if this is really a call for the camera. Other logic will use this code too. Check rbx with our known cameraStruct address to be sure
	cmp qword ptr rbx, [_cameraStructAddress]
	jne originalCode
	cmp byte ptr [_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movaps dword ptr [rbx+040h],xmm2					; original statement
	shufps xmm3,xmm8,-074h								; original statement
	shufps xmm1,xmm3,-034h								; original statement
	shufps xmm1,xmm1,072h								; original statement
	movaps dword ptr [rbx+050h],xmm1					; original statement
	shufps xmm3,xmm4,-027h								; original statement
	movaps dword ptr [rbx+060h],xmm3					; original statement
	movaps dword ptr [rbx+070h],xmm0					; original statement
exit:
	jmp qword ptr [_matrixWriteInterceptionContinue]	; jmp back into the original game code which is the location after the original statements above.
cameraWriteInterceptor ENDP

_TEXT ENDS
END