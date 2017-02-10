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
PUBLIC gamespeedAddressInterceptor
PUBLIC todAddressInterceptor
PUBLIC fovWriteInterceptor1
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraStructAddress1: qword
EXTERN g_cameraStructAddress2: qword
EXTERN g_gamespeedStructAddress: qword
EXTERN g_todStructAddress: qword
EXTERN g_cameraEnabled: byte
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWriteInterceptionContinue1: qword
EXTERN _cameraWriteInterceptionContinue2: qword
EXTERN _cameraWriteInterceptionContinue3: qword
EXTERN _gamespeedInterceptionContinue: qword
EXTERN _todAddressInterceptorContinue: qword
EXTERN _fovWriteInterceptorContinue: qword
EXTERN _resetCameraStructAddressOnNewAddresses: byte

;---------------------------------------------------------------
; Scratch pad
; 
; There are 2 camera objects, which are used both: one is calculated, and then copied to the other and vice versa. We need to obtain
; both pointers, otherwise it won't work. 
;
; ToD: 
;Disrupt_b64.dll+685848 - F3 0F5C F8            - subss xmm7,xmm0
;Disrupt_b64.dll+68584C - F3 0F11 BE 58070000   - movss [rsi+00000758],xmm7			<< Address is in rsi
;Disrupt_b64.dll+685854 - 48 8B CE              - mov rcx,rsi
; 7FFF2B415848 - F3 0F5C F8            - subss xmm7,xmm0

;

;---------------------------------------------------------------
.code

cameraAddressInterceptor PROC
	; Game jmps to this location due to the hook set in C function SetCameraStructInterceptorHook
	; The game uses 2 camera structs, both are written by this code. 
	; The game resets the struct addresses each time a level is loaded. So if the user goes back to the main menu and restarts, 
	; the addresses are changed. So we have to grab the addresses always:
	push rax
	add rax, 8		; add 0x8 because the rax pointer we grab is 8 bytes off
	cmp qword ptr rax, [g_cameraStructAddress1]
	je originalCode
	cmp qword ptr rax, [g_cameraStructAddress2]
	je originalCode	
	; not equal to one of the stored addresses. Likely a load has happened. Check the _resetCameraStructAddressOnNewAddresses flag to see 
	; if we need to store the new address in the first or the second struct. 
	cmp byte ptr [_resetCameraStructAddressOnNewAddresses], 1
	jne storeInSecondPointer
	; store in first pointer, reset flag
	mov [g_cameraStructAddress1], rax
	mov byte ptr [_resetCameraStructAddressOnNewAddresses], 0
	; also disable camera
	mov byte ptr [g_cameraEnabled], 0
	jmp originalCode
storeInSecondPointer:
	; store in second pointer, set flag back to 1 so next change will be picked up
	mov [g_cameraStructAddress2], rax
	mov byte ptr [_resetCameraStructAddressOnNewAddresses], 1
	; as this is code which doesn't write anything, we always execute it. 
originalCode:
	pop rax
	addss xmm8, dword ptr [rax+0Ch]	
	addss xmm7, dword ptr [rax+10h]
	addss xmm6, dword ptr [rax+14h]
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraAddressInterceptor ENDP


gamespeedAddressInterceptor PROC
	mov [g_gamespeedStructAddress], rbx
	addsd xmm1,mmword ptr [rbx+30h]
	mulsd xmm2,mmword ptr [rbx+70h]
	movapd xmm0,xmm2
	movsd mmword ptr [rbx+48h],xmm2
	jmp qword ptr [_gamespeedInterceptionContinue]
gamespeedAddressInterceptor ENDP


todAddressInterceptor PROC
	mov [g_todStructAddress], rsi
	mulss xmm0,xmm1
	subss xmm7,xmm0  
	movss dword ptr [rsi+758h],xmm7  
	jmp qword ptr [_todAddressInterceptorContinue]
todAddressInterceptor ENDP

fovWriteInterceptor1 PROC
;Disrupt_b64.dll+862220 - 44 89 41 3C           - mov [rcx+3C],r8d				<<<< INTERCEPT HERE
;Disrupt_b64.dll+862224 - F3 0F11 49 24         - movss [rcx+24],xmm1			<<<< FOV Write
;Disrupt_b64.dll+862229 - F3 0F11 49 28         - movss [rcx+28],xmm1
;Disrupt_b64.dll+86222E - C3                    - ret							<<<< CONTINUE
	mov [rcx+3Ch], r8d
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je noFovWrite												; our own camera is enabled, just skip the writes
	movss dword ptr [rcx+24h],xmm1	
noFovWrite:
	movss dword ptr [rcx+28h],xmm1
	jmp qword ptr [_fovWriteInterceptorContinue]
fovWriteInterceptor1 ENDP


;-------------------------------------------------------------------
; Camera values write interceptions. For each block of statements in the game's exe which write to the camera values, we intercept them and execute them if our
; own camera is disabled, otherwise we skip them. Each block checks whether the current struct pointer is the camera struct. If it's not the camera struct, it will
; execute the code regardless of whether our camera is enabled.

cameraWriteInterceptor1 PROC
	; Game jmps to this location due to the hook set in C function SetMatrixWriteInterceptorHooks. 
	; The game uses 2 camera structs, both are written by this code. 
	cmp qword ptr rcx, [g_cameraStructAddress1]
	je testCameraEnable
	cmp qword ptr rcx, [g_cameraStructAddress2]
	je testCameraEnable
	jmp originalCode
testCameraEnable:
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	mov eax,dword ptr [rdx]
	mov dword ptr [rcx+04h],eax
	mov eax,dword ptr [rdx+04h]
	mov dword ptr [rcx+08h],eax
	mov eax,dword ptr [rdx+08h]
	mov dword ptr [rcx+0Ch],eax
exit:
	jmp qword ptr [_cameraWriteInterceptionContinue1]	; jmp back into the original game code which is the location after the original statements above.
cameraWriteInterceptor1 ENDP

cameraWriteInterceptor2 PROC
	; Game jmps to this location due to the hook set in C function SetMatrixWriteInterceptorHooks. 
	; The game uses 2 camera structs, both are written by this code. 
	cmp qword ptr rbx, [g_cameraStructAddress1]
	je testCameraEnable
	cmp qword ptr rbx, [g_cameraStructAddress2]
	je testCameraEnable
	jmp originalCode
testCameraEnable:
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movss dword ptr [rbx+5Ch],xmm0
	movss xmm0,dword ptr [rsp+38h]
	movss dword ptr [rbx+60h],xmm1
	movss xmm1,dword ptr [rsp+40h]
	movss dword ptr [rbx+64h],xmm0
	movss xmm0,dword ptr [rsp+44h]
	movss dword ptr [rbx+44h],xmm1
	movss xmm1,dword ptr [rsp+48h]
	movss dword ptr [rbx+48h],xmm0
	movss xmm0,dword ptr [rsp+50h]
	movss dword ptr [rbx+4Ch],xmm1
	movss xmm1,dword ptr [rsp+54h]
	movss dword ptr [rbx+50h],xmm0
	movss xmm0,dword ptr [rsp+58h]
	movss dword ptr [rbx+54h],xmm1
	movss dword ptr [rbx+58h],xmm0
exit:
	jmp qword ptr [_cameraWriteInterceptionContinue2]	; jmp back into the original game code which is the location after the original statements above.
cameraWriteInterceptor2 ENDP

cameraWriteInterceptor3 PROC
	; Game jmps to this location due to the hook set in C function SetMatrixWriteInterceptorHooks. 
	; The game uses 2 camera structs, both are written by this code. 
	cmp qword ptr rcx, [g_cameraStructAddress1]
	je testCameraEnable
	cmp qword ptr rcx, [g_cameraStructAddress2]
	je testCameraEnable
	jmp originalCode
testCameraEnable:
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	; braindead copy routine ahead. Not sure what they are thinking at ubisoft, but this can be done way faster. 
	mov eax, dword ptr [rdx]
	mov dword ptr [rcx],eax
	mov eax, dword ptr [rdx+04h]
	mov dword ptr [rcx+04h],eax
	mov eax, dword ptr [rdx+08h]
	mov dword ptr [rcx+08h],eax
	mov eax, dword ptr [rdx+0Ch]
	mov dword ptr [rcx+0Ch],eax
	mov eax, dword ptr [rdx+10h]
	mov dword ptr [rcx+10h],eax
	mov eax, dword ptr [rdx+14h]
	mov dword ptr [rcx+14h],eax
	mov eax, dword ptr [rdx+18h]
	mov dword ptr [rcx+18h],eax
	mov eax, dword ptr [rdx+1Ch]
	mov dword ptr [rcx+1Ch],eax
	mov eax, dword ptr [rdx+20h]
	mov dword ptr [rcx+20h],eax
	mov eax, dword ptr [rdx+24h]
	mov dword ptr [rcx+24h],eax
	mov eax, dword ptr [rdx+28h]
	mov dword ptr [rcx+28h],eax
	mov eax, dword ptr [rdx+2Ch]
	mov dword ptr [rcx+2Ch],eax
	mov eax, dword ptr [rdx+30h]
	mov dword ptr [rcx+30h],eax
	mov eax, dword ptr [rdx+34h]
	mov dword ptr [rcx+34h],eax
	mov eax, dword ptr [rdx+38h]
	mov dword ptr [rcx+38h],eax
	mov eax, dword ptr [rdx+3Ch]
	mov dword ptr [rcx+3Ch],eax
	movzx eax,byte ptr [rdx+40h]
	mov [rcx+40h],al
	mov eax, dword ptr [rdx+44h]
	mov dword ptr [rcx+44h],eax
	mov eax, dword ptr [rdx+48h]
	mov dword ptr [rcx+48h],eax
	mov eax, dword ptr [rdx+4Ch]
	mov dword ptr [rcx+4Ch],eax
	mov eax, dword ptr [rdx+50h]
	mov dword ptr [rcx+50h],eax
	mov eax, dword ptr [rdx+54h]
	mov dword ptr [rcx+54h],eax
	mov eax, dword ptr [rdx+58h]
	mov dword ptr [rcx+58h],eax
	mov eax, dword ptr [rdx+5Ch]
	mov dword ptr [rcx+5Ch],eax
	mov eax, dword ptr [rdx+60h]
	mov dword ptr [rcx+60h],eax
	mov eax, dword ptr [rdx+64h]
	mov dword ptr [rcx+64h],eax
	mov eax, dword ptr [rdx+68h]
	mov dword ptr [rcx+68h],eax
	mov eax, dword ptr [rdx+6Ch]
	mov dword ptr [rcx+6Ch],eax
	mov eax, dword ptr [rdx+70h]
	mov dword ptr [rcx+70h],eax
exit:
	jmp qword ptr [_cameraWriteInterceptionContinue3]	; jmp back into the original game code which is the location after the original statements above.
cameraWriteInterceptor3 ENDP

END