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
; Externs defined in CameraManipulator.cpp, which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN _cameraStructAddress: dword
EXTERN _timestopStructAddress: dword
EXTERN _cameraEnabled: byte
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: dword
EXTERN _cameraWriteInterceptionContinue1: dword
EXTERN _timestopStructInterceptionContinue: dword

.code 
cameraAddressInterceptor PROC
;AC3SP.exe+CAF5 - D9 46 08              - fld dword ptr [esi+08]			; read FOV. Is in same struct	INTERCEPT HERE
;AC3SP.exe+CAF8 - F2 0F10 45 F8         - movsd xmm0,[ebp-08]
;AC3SP.exe+CAFD - D9 5E 04              - fstp dword ptr [esi+04]			; CONTINUE HERE
;AC3SP.exe+CB00 - F2 0F11 46 18         - movsd [esi+18],xmm0
;AC3SP.exe+CB05 - D9 46 04              - fld dword ptr [esi+04]
;AC3SP.exe+CB08 - 5E                    - pop esi
;AC3SP.exe+CB09 - 8B E5                 - mov esp,ebp
;AC3SP.exe+CB0B - 5D                    - pop ebp
;AC3SP.exe+CB0C - C3                    - ret 
	; Game jmps to this location due to the hook set in C function SetCameraStructInterceptorHook
	mov [_cameraStructAddress], esi						; intercept address of camera struct
originalCode:
	fld dword ptr [esi+08h]
	movsd xmm0, mmword ptr [ebp-08h]
exit:
	jmp [_cameraStructInterceptionContinue]				; jmp back into the original game code, which is the location after the original statements above.
cameraAddressInterceptor ENDP


cameraWriteInterceptor1 PROC
;AC3SP.exe+71EF8 - 0F28 47 10            - movaps xmm0,[edi+10]
;AC3SP.exe+71EFC - 0F29 46 10            - movaps [esi+10],xmm0			; write coords			INTERCEPT HERE
;AC3SP.exe+71F00 - 0F28 47 20            - movaps xmm0,[edi+20]
;AC3SP.exe+71F04 - 0F29 46 20            - movaps [esi+20],xmm0			; write quaternion
;AC3SP.exe+71F08 - D9 87 EC000000        - fld dword ptr [edi+000000EC]	;						CONTINUE HERE
;AC3SP.exe+71F0E - 5F                    - pop edi
;AC3SP.exe+71F0F - D9 5E 34              - fstp dword ptr [esi+34]
;AC3SP.exe+71F12 - B9 10FE5D02           - mov ecx,AC3SP.exe+21DFE10 { [025DFE14] }
	; Game jmps to this location due to the hook set in C function SetFoVWriteInterceptorHooks. 
	cmp byte ptr [_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movaps xmmword ptr [esi+10h],xmm0					; original statement
	movaps xmm0, xmmword ptr [edi+20h]					; original statement
	movaps xmmword ptr [esi+20h],xmm0					; original statement
exit:
	jmp [_cameraWriteInterceptionContinue1]				; jmp back into the original game code which is the location after the original statements above.
cameraWriteInterceptor1 ENDP


timestopStructInterceptor PROC
;AC3SP.exe+7EF86 - 85 C0                 - test eax,eax
;AC3SP.exe+7EF88 - 74 0D                 - je AC3SP.exe+7EF97
;AC3SP.exe+7EF8A - 83 B8 9C080000 00     - cmp dword ptr [eax+0000089C],00 { 0 }					<< READ Timestop. Intercept here
;AC3SP.exe+7EF91 - 7E 04                 - jle AC3SP.exe+7EF97										<< CONTINUE HERE
;AC3SP.exe+7EF93 - C6 45 FF 01           - mov byte ptr [ebp-01],01 { 1 }
;AC3SP.exe+7EF97 - F6 86 48010000 02     - test byte ptr [esi+00000148],02 { 2 }
;AC3SP.exe+7EF9E - F3 0F10 0D 5081B401   - movss xmm1,[AC3SP.exe+1748150] { [24.00] }
;AC3SP.exe+7EFA6 - 75 45                 - jne AC3SP.exe+7EFED
	mov [_timestopStructAddress], eax
originalCode:
	cmp dword ptr[eax + 0000089Ch], 00
exit:
	jmp [_timestopStructInterceptionContinue]
timestopStructInterceptor ENDP


_TEXT ENDS

END