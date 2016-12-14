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

.model flat,C
.stack 4096

;---------------------------------------------------------------
; Public definitions so the linker knows which names are present in this file
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs defined in Core.cpp, which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN _cameraStructAddress: dword
EXTERN _gamespeedStructAddress: dword
EXTERN _cameraEnabled: byte
EXTERN _timeStopped: byte
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: dword
EXTERN _cameraWriteInterceptionContinue1: dword
EXTERN _cameraWriteInterceptionContinue2: dword
EXTERN _gamespeedInterceptionContinue: qword


.code 
cameraAddressInterceptor PROC
	; Game jmps to this location due to the hook set in C function SetCameraStructInterceptorHook
	mov [_cameraStructAddress], esi						; intercept address of camera struct
	cmp byte ptr [_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movaps [esi+00000090h],xmm0							; original statement
exit:
	jmp [_cameraStructInterceptionContinue]				; jmp back into the original game code, which is the location after the original statements above.
cameraAddressInterceptor ENDP


gamespeedAddressInterceptor PROC
	;mov [_gamespeedStructAddress], rbx
	;mov [rbx+028h],rax
	;mov rcx,[rbx+018h]
	;mov [rbx+020h],rcx
	;add [rbx+018h],rax
	;jmp qword ptr [_gamespeedInterceptionContinue]
gamespeedAddressInterceptor ENDP

;-------------------------------------------------------------------
; Camera values write interceptions. For each block of statements in the game's exe which write to the camera values, we intercept them and execute them if our
; own camera is disabled, otherwise we skip them. Each block checks whether the current struct pointer is the camera struct. If it's not the camera struct, it will
; execute the code regardless of whether our camera is enabled.

cameraWriteInterceptor1 PROC
	; Game jmps to this location due to the hook set in C function SetMatrixWriteInterceptorHooks. 
	cmp byte ptr [_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je intercepted										; our own camera is enabled, just skip the writes
originalCode:
	movaps [eax],xmm0									; original statement
	movaps xmm0,[edx]									; original statement
	mov edx,[ebp+14h]									; original statement
	movaps [eax+10h],xmm0								; original statement
	movaps xmm0,[ecx]									; original statement
	movaps [eax+20h],xmm0								; original statement
	movaps xmm0,[edx]									; original statement
	fst dword ptr [eax+0Ch]								; original statement
	fst dword ptr [eax+01Ch]							; original statement
	movaps [eax+30h],xmm0								; original statement
	jmp exit
intercepted:											; the original statements without the writes. 
	movaps xmm0,[edx]									; original statement
	mov edx,[ebp+14h]									; original statement
	movaps xmm0,[ecx]									; original statement
	movaps xmm0,[edx]									; original statement
	fst dword ptr [eax+0Ch]								; original statement
	fst dword ptr [eax+01Ch]							; original statement
exit:
	jmp [_cameraWriteInterceptionContinue1]				; jmp back into the original game code which is the location after the original statements above.
cameraWriteInterceptor1 ENDP

cameraWriteInterceptor2 PROC
	; Game jmps to this location due to the hook set in C function SetMatrixWriteInterceptorHooks. 
	cmp dword ptr esi, [_cameraStructAddress]
	jne originalCode
	; The first write is in the camera address interception, these are the rest of the writes.
	cmp byte ptr [_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je intercepted												; our own camera is enabled, just skip the writes
originalCode:
	mov [esi-5Ch],ecx
	mov edx,[edi+08h]
	mov [esi-58h],edx
	mov eax,[edi+0Ch]
	mov [esi-54h],eax
	mov ecx,[esi+000003C0h]
	mov [esi-50h],ecx
	mov edx,[esi+000003C4h]
	mov [esi-4Ch],edx
	mov eax,[esi+000003C8h]
	mov [esi-48h],eax
	mov ecx,[esi+000003CCh]
	mov edi,[esp+1Ch]
	mov [esi-44h],ecx
	mov edx,[esi+000003D0h]
	mov [esi-40h],edx
	mov eax,[esi+000003D4h]
	mov [esi-3Ch],eax
	mov ecx,[esi+000003D8h]
	mov [esi-38h],ecx
	mov edx,[esi+000003DCh]
	mov [esi-34h],edx
	mov eax,[esi+000003E0h]
	mov [esi-30h],eax
	mov ecx,[esi+000003E4h]
	mov [esi-2Ch],ecx
	mov edx,[esi+000003E8h]
	mov [esi-28h],edx
	mov eax,[esi+000003ECh]
	push 00h
	push ebx
	mov ecx,edi
	mov [esi-24h],eax
	jmp exit
intercepted:
	mov edx,[edi+08h]
	mov eax,[edi+0Ch]
	mov ecx,[esi+000003C0h]
	mov edx,[esi+000003C4h]
	mov eax,[esi+000003C8h]
	mov ecx,[esi+000003CCh]
	mov edi,[esp+1Ch]
	mov edx,[esi+000003D0h]
	mov eax,[esi+000003D4h]
	mov ecx,[esi+000003D8h]
	mov edx,[esi+000003DCh]
	mov eax,[esi+000003E0h]
	mov ecx,[esi+000003E4h]
	mov edx,[esi+000003E8h]
	mov eax,[esi+000003ECh]
	push 00h
	push ebx
	mov ecx,edi
exit:
	jmp [_cameraWriteInterceptionContinue2]	; jmp back into the original game code which is the location after the original statements above.
cameraWriteInterceptor2 ENDP


_TEXT ENDS

END