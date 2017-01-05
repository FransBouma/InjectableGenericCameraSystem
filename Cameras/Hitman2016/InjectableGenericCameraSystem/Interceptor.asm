;////////////////////////////////////////////////////////////////////////////////////////////////////////
;// Part of Injectable Generic Camera System
;// Copyright(c) 2016, Frans Bouma
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
PUBLIC cameraWriteInterceptor1
PUBLIC cameraWriteInterceptor2
PUBLIC cameraWriteInterceptor3
PUBLIC cameraReadInterceptor1
PUBLIC gamespeedAddressInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs defined in Core.cpp, which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN _cameraStructAddress: qword
EXTERN _gamespeedStructAddress: qword
EXTERN g_cameraEnabled: byte
EXTERN g_aimFrozen: byte
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWriteInterceptionContinue1: qword
EXTERN _cameraWriteInterceptionContinue2: qword
EXTERN _cameraWriteInterceptionContinue3: qword
EXTERN _cameraReadInterceptionContinue1: qword
EXTERN _gamespeedInterceptionContinue: qword

;---------------------------------------------------------------
; For reference for fixing after game patch: Timestop usage code with RIP offset 
; hitman.exe+41C47B4 - 48 89 5C 24 50        - mov [rsp+50],rbx
; hitman.exe+41C47B9 - C6 05 88DC49FE 00     - mov byte ptr [hitman.exe+2662448],00		<<<<<<< offset.
; hitman.exe+41C47C0 - 48 83 B9 B8000000 00  - cmp qword ptr [rcx+000000B8],00
; hitman.exe+41C47C8 - 48 89 7C 24 40        - mov [rsp+40],rdi
;---------------------------------------------------------------


;---------------------------------------------------------------
.code

cameraAddressInterceptor PROC
	; Game jmps to this location due to the hook set in C function SetCameraStructInterceptorHook
	cmp byte ptr [rbx+038h], 0 							; check if this is the camera in rbx. For this game: Check with a 0-check. Could also check +20 or +24 for 0 if the above fails
	jne originalCode
	mov [_cameraStructAddress], rbx						; intercept address of camera struct
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movss dword ptr [rbx+090h], xmm0					; original statement
exit:
    movss xmm0, dword ptr [rsp+058h]					; original statement
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraAddressInterceptor ENDP

gamespeedAddressInterceptor PROC
	mov [_gamespeedStructAddress], rbx
	mov [rbx+028h],rax
	mov rcx,[rbx+018h]
	mov [rbx+020h],rcx
	add [rbx+018h],rax
	jmp qword ptr [_gamespeedInterceptionContinue]
gamespeedAddressInterceptor ENDP


;-------------------------------------------------------------------
; Camera values write interceptions. For each block of statements in the game's exe which write to the camera values, we intercept them and execute them if our
; own camera is disabled, otherwise we skip them. Each block checks whether the current struct pointer is the camera struct. If it's not the camera struct, it will
; execute the code regardless of whether our camera is enabled.

cameraWriteInterceptor1 PROC
	; Game jmps to this location due to the hook set in C function SetMatrixWriteInterceptorHooks. 
	; first check if this is really a call for the camera. Other logic will use this code too. Check rbx with our known cameraStruct address to be sure
	cmp qword ptr rbx, [_cameraStructAddress]
	jne originalCode
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movaps xmm0, xmmword ptr [rax]						; original statement
	movups xmmword ptr [rbx+080h],xmm0					; original statement
exit:
	movss xmm0, dword ptr [rsp+050h]					; original statement
	jmp qword ptr [_cameraWriteInterceptionContinue1]	; jmp back into the original game code which is the location after the original statements above.
cameraWriteInterceptor1 ENDP

cameraWriteInterceptor2 PROC
	; Game jmps to this location due to the hook set in C function SetMatrixWriteInterceptorHooks. 
	; first check if this is really a call for the camera. Other logic will use this code too. Check rbx with our known cameraStruct address to be sure
	cmp qword ptr rbx, [_cameraStructAddress]
	jne originalCode
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movss dword ptr [rbx+98h],xmm0						; original statement
	movss dword ptr [rbx+94h],xmm1						; original statement
exit:
	jmp qword ptr [_cameraWriteInterceptionContinue2]	; jmp back into the original game code which is the location after the original statements above.
cameraWriteInterceptor2 ENDP

cameraWriteInterceptor3 PROC
	; Game jmps to this location due to the hook set in C function SetMatrixWriteInterceptorHooks. 
	; first check if this is really a call for the camera. Other logic will use this code too. Check rbx with our known cameraStruct address to be sure
	cmp qword ptr rbx, [_cameraStructAddress]
	jne originalCode
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movups xmmword ptr [rbx+80h],xmm0  					; original statement
	movaps xmm1,xmmword ptr [rdi+30h]  					; original statement
	movaps xmm0,xmm1  									; original statement
	movss  dword ptr [rbx+90h],xmm1  					; original statement
	shufps xmm0,xmm1,55h  								; original statement
	shufps xmm1,xmm1,0AAh  								; original statement
	movss  dword ptr [rbx+98h],xmm1  					; original statement
	movss  dword ptr [rbx+94h],xmm0						; original statement
exit:
	jmp qword ptr [_cameraWriteInterceptionContinue3]	; jmp back into the original game code which is the location after the original statements above.
cameraWriteInterceptor3 ENDP

cameraReadInterceptor1 PROC
	cmp byte ptr [g_cameraEnabled], 1					
	jne originalCode
	cmp byte ptr [g_aimFrozen], 1
	jne originalCode
	; aim is frozen and camera is enabled. We now issue a 'ret', which means it will return to the caller of the original code as we jmp-ed to this location
	; from there. This means we won't continue in the original code. 
	ret
originalCode:
	push rbx
	sub rsp, 00000080h
	test byte ptr [rcx+000000AEh],02h
exit:
	jmp qword ptr [_cameraReadInterceptionContinue1]	; jmp back into the original game code which is the location after the original statements above.
cameraReadInterceptor1 ENDP

END