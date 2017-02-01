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
PUBLIC cameraWriteInterceptor1
PUBLIC cameraWriteInterceptor2
PUBLIC cameraWriteInterceptor3
PUBLIC cameraReadInterceptor1
PUBLIC gamespeedAddressInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraStructAddress: qword
EXTERN g_gamespeedStructAddress: qword
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
.code

cameraAddressInterceptor PROC
;hitman.exe+44F6F94 - F3 0F10 44 24 50      - movss xmm0,[rsp+50]
;hitman.exe+44F6F9A - F3 0F11 83 90000000   - movss [rbx+00000090],xmm0		<<<<<<<< HERE
;hitman.exe+44F6FA2 - F3 0F10 44 24 58      - movss xmm0,[rsp+58]
;hitman.exe+44F6FA8 - F3 0F11 83 98000000   - movss [rbx+00000098],xmm0	    <<<<<<<< CONTINUE
;hitman.exe+44F6FB0 - F3 0F11 8B 94000000   - movss [rbx+00000094],xmm1

	; Game jmps to this location due to the hook set in C function SetCameraStructInterceptorHook
	cmp byte ptr [rbx+038h], 0 							; check if this is the camera in rbx. For this game: Check with a 0-check. Could also check +20 or +24 for 0 if the above fails
	jne originalCode
	mov [g_cameraStructAddress], rbx						; intercept address of camera struct
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movss dword ptr [rbx+090h], xmm0					; original statement
exit:
    movss xmm0, dword ptr [rsp+058h]					; original statement
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraAddressInterceptor ENDP

gamespeedAddressInterceptor PROC
;hitman.exe+44FA00A - 48 C1 F8 14           - sar rax,14 { 20 }				
;hitman.exe+44FA00E - 48 89 43 28           - mov [rbx+28],rax				<<< HERE
;hitman.exe+44FA012 - 48 8B 4B 18           - mov rcx,[rbx+18]
;hitman.exe+44FA016 - 48 89 4B 20           - mov [rbx+20],rcx
;hitman.exe+44FA01A - 48 01 43 18           - add [rbx+18],rax
;hitman.exe+44FA01E - EB 48                 - jmp hitman.exe+44FA068		<<< CONTINUE
	mov [g_gamespeedStructAddress], rbx
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
;hitman.exe+44F6F84 - F3 0F10 4C 24 54      - movss xmm1,[rsp+54]
;hitman.exe+44F6F8A - 0F28 00               - movaps xmm0,[rax]					<<<<< HERE
;hitman.exe+44F6F8D - 0F11 83 80000000      - movups [rbx+00000080],xmm0
;hitman.exe+44F6F94 - F3 0F10 44 24 50      - movss xmm0,[rsp+50]
;hitman.exe+44F6F9A - F3 0F11 83 90000000   - movss [rbx+00000090],xmm0			<<<<< CONTINUE

	; Game jmps to this location due to the hook set in C function SetMatrixWriteInterceptorHooks. 
	; first check if this is really a call for the camera. Other logic will use this code too. Check rbx with our known cameraStruct address to be sure
	cmp qword ptr rbx, [g_cameraStructAddress]
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
;hitman.exe+44F6FA2 - F3 0F10 44 24 58      - movss xmm0,[rsp+58]
;hitman.exe+44F6FA8 - F3 0F11 83 98000000   - movss [rbx+00000098],xmm0			<<<< HERE
;hitman.exe+44F6FB0 - F3 0F11 8B 94000000   - movss [rbx+00000094],xmm1
;hitman.exe+44F6FB8 - EB 3C                 - jmp hitman.exe+44F6FF6			<<<< CONTINUE

	; Game jmps to this location due to the hook set in C function SetMatrixWriteInterceptorHooks. 
	; first check if this is really a call for the camera. Other logic will use this code too. Check rbx with our known cameraStruct address to be sure
	cmp qword ptr rbx, [g_cameraStructAddress]
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
;hitman.exe+44F6FC5 - 0F28 00               - movaps xmm0,[rax]
;hitman.exe+44F6FC8 - 0F11 83 80000000      - movups [rbx+00000080],xmm0		<<<< HERE
;hitman.exe+44F6FCF - 0F28 4F 30            - movaps xmm1,[rdi+30]
;hitman.exe+44F6FD3 - 0F28 C1               - movaps xmm0,xmm1
;hitman.exe+44F6FD6 - F3 0F11 8B 90000000   - movss [rbx+00000090],xmm1
;hitman.exe+44F6FDE - 0FC6 C1 55            - shufps xmm0,xmm1,55 { 85 }
;hitman.exe+44F6FE2 - 0FC6 C9 AA            - shufps xmm1,xmm1,-56 { 170 }
;hitman.exe+44F6FE6 - F3 0F11 8B 98000000   - movss [rbx+00000098],xmm1
;hitman.exe+44F6FEE - F3 0F11 83 94000000   - movss [rbx+00000094],xmm0
;hitman.exe+44F6FF6 - 48 8B 03              - mov rax,[rbx]						<<<< CONTINUE
;
	; Game jmps to this location due to the hook set in C function SetMatrixWriteInterceptorHooks. 
	; first check if this is really a call for the camera. Other logic will use this code too. Check rbx with our known cameraStruct address to be sure
	cmp qword ptr rbx, [g_cameraStructAddress]
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
;hitman.exe+44FA590 - 53                    - push rbx								<<< HERE
;hitman.exe+44FA591 - 48 81 EC 80000000     - sub rsp,00000080 { 128 }
;hitman.exe+44FA598 - F6 81 AE000000 02     - test byte ptr [rcx+000000AE],02 { 2 }
;hitman.exe+44FA59F - 48 89 CB              - mov rbx,rcx							<<< CONTINUE
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