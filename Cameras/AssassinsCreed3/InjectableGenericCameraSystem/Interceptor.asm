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


.code 

cameraAddressInterceptor PROC
	; Game jmps to this location due to the hook set in C function SetCameraStructInterceptorHook
	mov [_cameraStructAddress], esi						; intercept address of camera struct
originalCode:
	fld dword ptr [esi+08h]
	movsd xmm0, mmword ptr [ebp-08h]
exit:
	jmp [_cameraStructInterceptionContinue]				; jmp back into the original game code, which is the location after the original statements above.
cameraAddressInterceptor ENDP

cameraWriteInterceptor1 PROC
	; Game jmps to this location due to the hook set in C function SetFoVWriteInterceptorHooks. 
	cmp byte ptr [_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movaps xmmword ptr [esi+10h],xmm0					; original statement
	movaps xmm0,xmmword ptr [edi+20h]					; original statement
	movaps xmmword ptr [esi+20h],xmm0					; original statement
exit:
	jmp [_cameraWriteInterceptionContinue1]				; jmp back into the original game code which is the location after the original statements above.
cameraWriteInterceptor1 ENDP

;gamespeedAddressInterceptor PROC
	;mov [_gamespeedStructAddress], esi
	;cmp byte ptr [_timeStopped], 1
	;je exit
;originalCode:
	;fstp dword ptr [esi+00000104h]
;exit:
	;jmp [_gamespeedInterceptionContinue]
;gamespeedAddressInterceptor ENDP


_TEXT ENDS

END