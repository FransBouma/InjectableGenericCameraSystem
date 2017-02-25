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
PUBLIC fovWriteInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraStructAddress: qword
EXTERN g_cameraEnabled: byte
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _fovWriteInterceptorContinue: qword

;---------------------------------------------------------------
; Scratch pad
; 
;---------------------------------------------------------------
.code

cameraAddressInterceptor PROC
; The camera interceptor is also the write disable. We simply skip the complete code block if the camera is enabled. 
;BatmanAK.exe+43A111B - 89 83 74050000        - mov [rbx+00000574],eax				<<< HERE
;BatmanAK.exe+43A1121 - 8B 47 04              - mov eax,[rdi+04]
;BatmanAK.exe+43A1124 - 89 83 78050000        - mov [rbx+00000578],eax
;BatmanAK.exe+43A112A - 8B 47 08              - mov eax,[rdi+08]
;BatmanAK.exe+43A112D - 89 83 7C050000        - mov [rbx+0000057C],eax
;BatmanAK.exe+43A1133 - 8B 47 0C              - mov eax,[rdi+0C]
;BatmanAK.exe+43A1136 - 89 83 80050000        - mov [rbx+00000580],eax
;BatmanAK.exe+43A113C - 8B 47 10              - mov eax,[rdi+10]
;BatmanAK.exe+43A113F - 89 83 84050000        - mov [rbx+00000584],eax
;BatmanAK.exe+43A1145 - 8B 47 14              - mov eax,[rdi+14]
;BatmanAK.exe+43A1148 - 89 83 88050000        - mov [rbx+00000588],eax
;BatmanAK.exe+43A114E - 8B 47 18              - mov eax,[rdi+18]
;BatmanAK.exe+43A1151 - 89 83 8C050000        - mov [rbx+0000058C],eax
;BatmanAK.exe+43A1157 - 48 8B 5C 24 30        - mov rbx,[rsp+30]					<<< CONTINUE

	; Game jmps to this location due to the hook set in C function SetCameraStructInterceptorHook
	mov [g_cameraStructAddress], rbx
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	mov [rbx+00000574h],eax
	mov eax,[rdi+04h]
	mov [rbx+00000578h],eax
	mov eax,[rdi+08h]
	mov [rbx+0000057Ch],eax
	mov eax,[rdi+0Ch]
	mov [rbx+00000580h],eax
	mov eax,[rdi+10h]
	mov [rbx+00000584h],eax
	mov eax,[rdi+14h]
	mov [rbx+00000588h],eax
	mov eax,[rdi+18h]
	mov [rbx+0000058Ch],eax
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraAddressInterceptor ENDP

fovWriteInterceptor PROC
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
fovWriteInterceptor ENDP
END