;////////////////////////////////////////////////////////////////////////////////////////////////////////
;// Part of Injectable Generic Camera System
;// Copyright(c) 2017, Frans Bouma
;// All rights reserved.
;// https://github.com/FransBouma/InjectableGenericCameraSystem
;//
;// Redistribution and use in source and binary forms, with or without
;// modification, are permitted provided that the following conditions are met :
;//
;//  * Redistributions of source code must retain the above copyright notice, this
;//	  list of conditions and the following disclaimer.
;//
;//  * Redistributions in binary form must reproduce the above copyright notice,
;//    this list of conditions and the following disclaimer in the documentation
;//    and / or other materials provided with the distribution.
;//
;// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
;// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
;// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
;// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
;// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
;// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
;// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;////////////////////////////////////////////////////////////////////////////////////////////////////////
;---------------------------------------------------------------
; Game specific asm file to intercept execution flow to obtain addresses, prevent writes etc.
;---------------------------------------------------------------

;---------------------------------------------------------------
; Public definitions so the linker knows which names are present in this file
PUBLIC cameraAddressInterceptor
PUBLIC fovInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraStructAddress: qword
EXTERN g_cameraEnabled: byte
EXTERN g_fovOverrideEnabled: byte
EXTERN g_ownFoVValue: dword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _hostBaseAddress: qword
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _fovInterceptorContinue: qword

;---------------------------------------------------------------
; Scratch pad
;---------------------------------------------------------------
.code

cameraAddressInterceptor PROC
	; Game jmps to this location due to the hook set in C function SetCameraStructInterceptorHook
	; The game uses one block of code which is only used to write the new coords + camera matrix. We can simply combine the address interception and 
	; write disable in one. 
	mov [g_cameraStructAddress], rdi
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rdi+000000C0h],xmm6
	movss dword ptr [rdi+000000C4h],xmm7
	movss dword ptr [rdi+000000C8h],xmm8
	movss dword ptr [rdi+000000CCh],xmm9
	movss dword ptr [rdi+000000D8h],xmm12
	movss dword ptr [rdi+000000D0h],xmm10
	movss dword ptr [rdi+000000D4h],xmm11
	movss dword ptr [rdi+000000DCh],xmm13
	movss dword ptr [rdi+000000E0h],xmm14
	movss dword ptr [rdi+000000E4h],xmm15
	movss dword ptr [rdi+000000E8h],xmm0
	movss dword ptr [rdi+000000ECh],xmm1
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraAddressInterceptor ENDP


fovInterceptor PROC
	; Game jmps to this location due to the hook set in C function setFoVInterceptorHook
	push rax
	mov qword ptr rax, [_hostBaseAddress]
	cmp byte ptr [g_fovOverrideEnabled], 1
	jne originalCode
	movss xmm1, dword ptr [g_ownFoVValue]			; overwrite the value read from memory in the game, which is in xmm1
originalCode:
	mulss xmm1, dword ptr [rax+1DEEDD0h]
	mulss xmm1, dword ptr [rax+1DE2F08h]
	movaps xmm0, xmm1
	pop rax
	jmp qword ptr [_fovInterceptorContinue]
fovInterceptor ENDP


END