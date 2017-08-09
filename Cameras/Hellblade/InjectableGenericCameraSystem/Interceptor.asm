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
PUBLIC fovReadInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_cameraStructAddress: qword
EXTERN g_fovStructAddress: qword			; although FoV is in the main camera struct, we're re-reading the location every frame as 
											; the photomode uses the original camera's fov value, but a different camera's coords/angles.
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _fovReadInterceptionContinue: qword

.data
;---------------------------------------------------------------
; Scratch pad
.code


cameraStructInterceptor PROC
;// Used in-game and in photomode
;// UE uses angles for rotation. In UE3 it uses packed 16 bit ints, in UE4 it uses floats, in degrees (0-360.0)
;Relocated code. Simply skip the writes if the camera is enabled.
;HellbladeGame-Win64-Shipping.exe+1940B55 - 48 8D 55 90           - lea rdx,[rbp-70]					// 0x7FF6947B0B55
;HellbladeGame-Win64-Shipping.exe+1940B59 - 8B 88 F8080000        - mov ecx,[rax+000008F8]
;HellbladeGame-Win64-Shipping.exe+1940B5F - 8B 44 24 58           - mov eax,[rsp+58]
;HellbladeGame-Win64-Shipping.exe+1940B63 - 89 8F 70040000        - mov [rdi+00000470],ecx
;HellbladeGame-Win64-Shipping.exe+1940B69 - 48 8D 8F C0040000     - lea rcx,[rdi+000004C0]
;HellbladeGame-Win64-Shipping.exe+1940B70 - 83 A7 AC040000 FC     - and dword ptr [rdi+000004AC],-04
;HellbladeGame-Win64-Shipping.exe+1940B77 - F2 0F11 87 80040000   - movsd [rdi+00000480],xmm0		<< INTERCEPT HERE. Write X & Y
;HellbladeGame-Win64-Shipping.exe+1940B7F - F2 0F10 44 24 5C      - movsd xmm0,[rsp+5C]
;HellbladeGame-Win64-Shipping.exe+1940B85 - F2 0F11 87 8C040000   - movsd [rdi+0000048C],xmm0			// Write Rotation around X/ around Z
;HellbladeGame-Win64-Shipping.exe+1940B8D - 0F10 44 24 68         - movups xmm0,[rsp+68]
;HellbladeGame-Win64-Shipping.exe+1940B92 - 89 87 88040000        - mov [rdi+00000488],eax				// Write Z (UP)
;HellbladeGame-Win64-Shipping.exe+1940B98 - 8B 44 24 64           - mov eax,[rsp+64]
;HellbladeGame-Win64-Shipping.exe+1940B9C - 89 87 94040000        - mov [rdi+00000494],eax				// Write Rotation around Y (into the screen)
;HellbladeGame-Win64-Shipping.exe+1940BA2 - 8B 44 24 7C           - mov eax,[rsp+7C]
;HellbladeGame-Win64-Shipping.exe+1940BA6 - 0F11 87 98040000      - movups [rdi+00000498],xmm0			// Write FOV
;HellbladeGame-Win64-Shipping.exe+1940BAD - 83 E0 03              - and eax,03 { 3 }					<< CONTINUE HERE
;HellbladeGame-Win64-Shipping.exe+1940BB0 - F3 0F10 44 24 78      - movss xmm0,[rsp+78]
;HellbladeGame-Win64-Shipping.exe+1940BB6 - 09 87 AC040000        - or [rdi+000004AC],eax
	mov [g_cameraStructAddress], rdi
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movsd qword ptr [rdi+480h],xmm0
	movsd xmm0,qword ptr [rsp+5Ch]
	movsd qword ptr [rdi+48Ch],xmm0
	movups xmm0,xmmword ptr [rsp+68h]
	mov dword ptr [rdi+488h],eax
	mov eax,dword ptr [rsp+64h]
	mov dword ptr [rdi+494h],eax
	mov eax,dword ptr [rsp+7Ch]
	movups xmmword ptr [rdi+498h],xmm0
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP


fovReadInterceptor PROC
;HellbladeGame-Win64-Shipping.exe+1944DD0 - F3 0F10 81 14040000   - movss xmm0,[rcx+00000414]			<< INTERCEPT HERE
;HellbladeGame-Win64-Shipping.exe+1944DD8 - 0F57 C9               - xorps xmm1,xmm1
;HellbladeGame-Win64-Shipping.exe+1944DDB - 0F2F C1               - comiss xmm0,xmm1
;HellbladeGame-Win64-Shipping.exe+1944DDE - 77 08                 - ja HellbladeGame-Win64-Shipping.exe+1944DE8
;HellbladeGame-Win64-Shipping.exe+1944DE0 - F3 0F10 81 98040000   - movss xmm0,[rcx+00000498]         // FOV READ
;HellbladeGame-Win64-Shipping.exe+1944DE8 - C3                    - ret									<< CONTINUE HERE
;
; the game uses the fov in the camera struct, but in the photomode the fov is at offset 414, not 498
	mov [g_fovStructAddress], rcx
originalCode:
	movss xmm0,dword ptr [rcx+414h]
	xorps xmm1,xmm1                             
	comiss xmm0,xmm1                            
	ja exit
	movss xmm0,dword ptr [rcx+498h]
exit:
	jmp qword ptr [_fovReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
fovReadInterceptor ENDP

END