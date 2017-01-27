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
PUBLIC fovAddressInterceptor

PUBLIC cameraWriteInterceptor1
PUBLIC cameraWriteInterceptor2
PUBLIC cameraWriteInterceptor3
PUBLIC gamespeedAddressInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraStructAddress: qword
EXTERN g_fovStructAddress: qword
EXTERN g_cameraEnabled: byte

EXTERN g_gamespeedStructAddress: qword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _fovAddressInterceptorContinue: qword

EXTERN _cameraWriteInterceptionContinue1: qword
EXTERN _cameraWriteInterceptionContinue2: qword
EXTERN _cameraWriteInterceptionContinue3: qword
EXTERN _gamespeedInterceptionContinue: qword

;---------------------------------------------------------------
; Scratch pad
; FOV write below is called only when weapon is zoomed inout. There's a copy routine elsewhere but that one is called a lot of times for different code
; so not reliable. We have to determine where the xmm7 comes from below.
;DOOMx64.exe+91FCEA - F3 41 0F11 76 0C      - movss [r14+0C],xmm6
;DOOMx64.exe+91FCF0 - F3 41 0F11 7E 10      - movss [r14+10],xmm7			<< FOV write.
;DOOMx64.exe+91FCF6 - FF CB                 - dec ebx
;DOOMx64.exe+91FCF8 - 48 8D 54 24 38        - lea rdx,[rsp+38]
;; 
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


fovAddressInterceptor PROC
	; Game jmps to this location due to the hook set in C function setFoVStructInterceptorHook
	mov [g_fovStructAddress], rcx
	;mov eax,dword ptr [rcx+10h]
	;movaps xmm6,xmmword ptr [rsp]
	;mov dword ptr [rcx+18h],eax
	;movss xmm0,dword ptr [rcx+18h]
	jmp qword ptr [_fovAddressInterceptorContinue]
fovAddressInterceptor ENDP



gamespeedAddressInterceptor PROC
gamespeedAddressInterceptor ENDP

;-------------------------------------------------------------------
; Camera values write interceptions. For each block of statements in the game's exe which write to the camera values, we intercept them and execute them if our
; own camera is disabled, otherwise we skip them. Each block checks whether the current struct pointer is the camera struct. If it's not the camera struct, it will
; execute the code regardless of whether our camera is enabled.

cameraWriteInterceptor1 PROC
	; Game jmps to this location due to the hook set in C function SetMatrixWriteInterceptorHooks. 
	; The game uses 2 camera structs, both are written by this code. 
	;cmp qword ptr rcx, [g_cameraStructAddress1]
	;je testCameraEnable
	;cmp qword ptr rcx, [g_cameraStructAddress2]
	;je testCameraEnable
	;jmp originalCode
;testCameraEnable:
	;cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	;je exit												; our own camera is enabled, just skip the writes
;originalCode:
	;mov eax,dword ptr [rdx]
	;mov dword ptr [rcx+04h],eax
	;mov eax,dword ptr [rdx+04h]
	;mov dword ptr [rcx+08h],eax
	;mov eax,dword ptr [rdx+08h]
	;mov dword ptr [rcx+0Ch],eax
;exit:
	;jmp qword ptr [_cameraWriteInterceptionContinue1]	; jmp back into the original game code which is the location after the original statements above.
cameraWriteInterceptor1 ENDP

cameraWriteInterceptor2 PROC
	;; Game jmps to this location due to the hook set in C function SetMatrixWriteInterceptorHooks. 
	;; The game uses 2 camera structs, both are written by this code. 
	;cmp qword ptr rbx, [g_cameraStructAddress1]
	;je testCameraEnable
	;cmp qword ptr rbx, [g_cameraStructAddress2]
	;je testCameraEnable
	;jmp originalCode
;testCameraEnable:
	;cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	;je exit												; our own camera is enabled, just skip the writes
;originalCode:
	;movss dword ptr [rbx+5Ch],xmm0
	;movss xmm0,dword ptr [rsp+38h]
	;movss dword ptr [rbx+60h],xmm1
	;movss xmm1,dword ptr [rsp+40h]
	;movss dword ptr [rbx+64h],xmm0
	;movss xmm0,dword ptr [rsp+44h]
	;movss dword ptr [rbx+44h],xmm1
	;movss xmm1,dword ptr [rsp+48h]
	;movss dword ptr [rbx+48h],xmm0
	;movss xmm0,dword ptr [rsp+50h]
	;movss dword ptr [rbx+4Ch],xmm1
	;movss xmm1,dword ptr [rsp+54h]
	;movss dword ptr [rbx+50h],xmm0
	;movss xmm0,dword ptr [rsp+58h]
	;movss dword ptr [rbx+54h],xmm1
	;movss dword ptr [rbx+58h],xmm0
;exit:
	;jmp qword ptr [_cameraWriteInterceptionContinue2]	; jmp back into the original game code which is the location after the original statements above.
cameraWriteInterceptor2 ENDP

cameraWriteInterceptor3 PROC
	;; Game jmps to this location due to the hook set in C function SetMatrixWriteInterceptorHooks. 
	;; The game uses 2 camera structs, both are written by this code. 
	;cmp qword ptr rcx, [g_cameraStructAddress1]
	;je testCameraEnable
	;cmp qword ptr rcx, [g_cameraStructAddress2]
	;je testCameraEnable
	;jmp originalCode
;testCameraEnable:
	;cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	;je exit												; our own camera is enabled, just skip the writes
;originalCode:
	;; braindead copy routine ahead. Not sure what they are thinking at ubisoft, but this can be done way faster. 
	;mov eax, dword ptr [rdx]
	;mov dword ptr [rcx],eax
	;mov eax, dword ptr [rdx+04h]
	;mov dword ptr [rcx+04h],eax
	;mov eax, dword ptr [rdx+08h]
	;mov dword ptr [rcx+08h],eax
	;mov eax, dword ptr [rdx+0Ch]
	;mov dword ptr [rcx+0Ch],eax
	;mov eax, dword ptr [rdx+10h]
	;mov dword ptr [rcx+10h],eax
	;mov eax, dword ptr [rdx+14h]
	;mov dword ptr [rcx+14h],eax
	;mov eax, dword ptr [rdx+18h]
	;mov dword ptr [rcx+18h],eax
	;mov eax, dword ptr [rdx+1Ch]
	;mov dword ptr [rcx+1Ch],eax
	;mov eax, dword ptr [rdx+20h]
	;mov dword ptr [rcx+20h],eax
	;mov eax, dword ptr [rdx+24h]
	;mov dword ptr [rcx+24h],eax
	;mov eax, dword ptr [rdx+28h]
	;mov dword ptr [rcx+28h],eax
	;mov eax, dword ptr [rdx+2Ch]
	;mov dword ptr [rcx+2Ch],eax
	;mov eax, dword ptr [rdx+30h]
	;mov dword ptr [rcx+30h],eax
	;mov eax, dword ptr [rdx+34h]
	;mov dword ptr [rcx+34h],eax
	;mov eax, dword ptr [rdx+38h]
	;mov dword ptr [rcx+38h],eax
	;mov eax, dword ptr [rdx+3Ch]
	;mov dword ptr [rcx+3Ch],eax
	;movzx eax,byte ptr [rdx+40h]
	;mov [rcx+40h],al
	;mov eax, dword ptr [rdx+44h]
	;mov dword ptr [rcx+44h],eax
	;mov eax, dword ptr [rdx+48h]
	;mov dword ptr [rcx+48h],eax
	;mov eax, dword ptr [rdx+4Ch]
	;mov dword ptr [rcx+4Ch],eax
	;mov eax, dword ptr [rdx+50h]
	;mov dword ptr [rcx+50h],eax
	;mov eax, dword ptr [rdx+54h]
	;mov dword ptr [rcx+54h],eax
	;mov eax, dword ptr [rdx+58h]
	;mov dword ptr [rcx+58h],eax
	;mov eax, dword ptr [rdx+5Ch]
	;mov dword ptr [rcx+5Ch],eax
	;mov eax, dword ptr [rdx+60h]
	;mov dword ptr [rcx+60h],eax
	;mov eax, dword ptr [rdx+64h]
	;mov dword ptr [rcx+64h],eax
	;mov eax, dword ptr [rdx+68h]
	;mov dword ptr [rcx+68h],eax
	;mov eax, dword ptr [rdx+6Ch]
	;mov dword ptr [rcx+6Ch],eax
	;mov eax, dword ptr [rdx+70h]
	;mov dword ptr [rcx+70h],eax
;exit:
	;jmp qword ptr [_cameraWriteInterceptionContinue3]	; jmp back into the original game code which is the location after the original statements above.
cameraWriteInterceptor3 ENDP

END