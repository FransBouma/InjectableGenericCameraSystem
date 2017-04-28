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
PUBLIC cameraStructInterceptor
PUBLIC fovWriteInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_cameraStructAddress: qword
EXTERN g_ownCoordinates: qword
EXTERN g_ownAngles: qword
EXTERN g_fovValue: dword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _fovWriteInterceptionContinue : qword
.data

;---------------------------------------------------------------
; Scratch pad
;---------------------------------------------------------------
.code


cameraStructInterceptor PROC
; Struct interceptor is also a write interceptor.
; From 1.1.2, 28/04/2017
;StormGame-Win64-Shipping.exe+3C0E1F - CC                    - int 3 
;StormGame-Win64-Shipping.exe+3C0E20 - 8B 81 1C040000        - mov eax,[rcx+0000041C]			<< INTERCEPT HERE
;StormGame-Win64-Shipping.exe+3C0E26 - 89 02                 - mov [rdx],eax					<< WRITE X
;StormGame-Win64-Shipping.exe+3C0E28 - 8B 81 20040000        - mov eax,[rcx+00000420]
;StormGame-Win64-Shipping.exe+3C0E2E - 89 42 04              - mov [rdx+04],eax
;StormGame-Win64-Shipping.exe+3C0E31 - 8B 81 24040000        - mov eax,[rcx+00000424]
;StormGame-Win64-Shipping.exe+3C0E37 - 89 42 08              - mov [rdx+08],eax
;StormGame-Win64-Shipping.exe+3C0E3A - 8B 81 28040000        - mov eax,[rcx+00000428]			
;StormGame-Win64-Shipping.exe+3C0E40 - 41 89 00              - mov [r8],eax						<< WRITE ROT X
;StormGame-Win64-Shipping.exe+3C0E43 - 8B 81 2C040000        - mov eax,[rcx+0000042C]
;StormGame-Win64-Shipping.exe+3C0E49 - 41 89 40 04           - mov [r8+04],eax
;StormGame-Win64-Shipping.exe+3C0E4D - 8B 81 30040000        - mov eax,[rcx+00000430]
;StormGame-Win64-Shipping.exe+3C0E53 - 41 89 40 08           - mov [r8+08],eax
;StormGame-Win64-Shipping.exe+3C0E57 - C3                    - ret								<< CONTINUE HERE
;StormGame-Win64-Shipping.exe+3C0E58 - CC                    - int 3 

; This function intercepts the camera address and also blocks the writes by reading from our own two structs. 
	mov [g_cameraStructAddress], rcx
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
	; it can be the structs aren't created yet. We check on that and skip to the original code if they're not here yet. 
	cmp qword ptr [g_ownCoordinates], 0
	je originalCode
	cmp qword ptr [g_ownAngles], 0
	je originalCode
readFromOwnStructs:
	push rcx
	mov rcx, [g_ownCoordinates]
	mov eax, [rcx]
	mov [rdx],eax			
	mov eax, [rcx+4h]
	mov [rdx+04h],eax
	mov eax,[rcx+8h]
	mov [rdx+08h],eax
	mov rcx, [g_ownAngles]
	mov eax,[rcx]
	mov [r8],eax				
	mov eax,[rcx+4h]
	mov [r8+04h],eax
	mov eax,[rcx+8h]
	mov [r8+08h],eax
	pop rcx
	jmp exit
originalCode:
	mov eax, [rcx+0000041Ch]
	mov [rdx],eax			
	mov eax, [rcx+00000420h]
	mov [rdx+04h],eax
	mov eax,[rcx+00000424h]
	mov [rdx+08h],eax
	mov eax,[rcx+00000428h]
	mov [r8],eax				
	mov eax,[rcx+0000042Ch]
	mov [r8+04h],eax
	mov eax,[rcx+00000430h]
	mov [r8+08h],eax
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP


fovWriteInterceptor PROC
; This one is nasty. To be able to write the fov we have to inject it into the xmm0 register. This is needed because
; the fov is overwritten by a memmove so we can't intercept it. This way we can inject our own fov value. 
;StormGame-Win64-Shipping.exe+6D70873 - 4C 8B 44 24 50        - mov r8,[rsp+50]
;StormGame-Win64-Shipping.exe+6D70878 - F3 0F10 4C 24 48      - movss xmm1,[rsp+48]
;StormGame-Win64-Shipping.exe+6D7087E - 48 89 F1              - mov rcx,rsi
;StormGame-Win64-Shipping.exe+6D70881 - E8 FA220CFA           - call StormGame-Win64-Shipping.exe+E32B80
; StormGame-Win64-Shipping.exe+6D70886 - 48 8B 5C 24 40        - mov rbx,[rsp+40]					<< INTERCEPT HERE
;StormGame-Win64-Shipping.ABSPlayerCamera::execAdjustFOVForViewport+AB - F3 0F11 07            - movss [rdi],xmm0					<< FOV WRITE. 
;StormGame-Win64-Shipping.ABSPlayerCamera::execAdjustFOVForViewport+AF - 48 83 C4 20           - add rsp,20
;StormGame-Win64-Shipping.ABSPlayerCamera::execAdjustFOVForViewport+B3 - 5F                    - pop rdi
;StormGame-Win64-Shipping.exe+6D70894                                  - 5E                    - pop rsi
;StormGame-Win64-Shipping.exe+6D70895                                  - 5D                    - pop rbp
;StormGame-Win64-Shipping.exe+6D70896                                  - C3                    - ret								<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
	; inject our fov value into the xmm0 register.
	push rcx
	mov ecx, [g_fovValue]
	movd xmm0, ecx
	pop rcx
originalCode:
	mov rbx,[rsp+40h]
	movss dword ptr [rdi],xmm0
	add rsp,20h
	pop rdi
	pop rsi
	pop rbp
exit:
	jmp qword ptr [_fovWriteInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
fovWriteInterceptor ENDP

END