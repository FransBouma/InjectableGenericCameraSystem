;////////////////////////////////////////////////////////////////////////////////////////////////////////
;// Part of Injectable Generic Camera System
;// Copyright(c) 2018, Frans Bouma
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
PUBLIC runFramesAddressInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraStructAddress: qword
EXTERN g_fovStructAddress: qword
EXTERN g_runFramesStructAddress: qword
EXTERN g_cameraEnabled: byte
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _fovAddressInterceptionContinue: qword
EXTERN _runFramesAddressInterceptionContinue: qword

; Scratch pad
;
;---------------------------------------------------------------
.code

cameraAddressInterceptor PROC
;NewColossus_x64vk.exe+FE8EC2 - F3 0F10 45 C7         - movss xmm0,[rbp-39]
;NewColossus_x64vk.exe+FE8EC7 - F3 0F10 4D CB         - movss xmm1,[rbp-35]
;NewColossus_x64vk.exe+FE8ECC - F3 0F11 87 E8000000   - movss [rdi+000000E8],xmm0    ; WRITE X. Intercept here
;NewColossus_x64vk.exe+FE8ED4 - F3 0F10 45 CF         - movss xmm0,[rbp-31]          ;
;NewColossus_x64vk.exe+FE8ED9 - F3 0F11 87 F0000000   - movss [rdi+000000F0],xmm0    ; WRITE Z
;NewColossus_x64vk.exe+FE8EE1 - 0F10 45 E7            - movups xmm0,[rbp-19]         ;
;NewColossus_x64vk.exe+FE8EE5 - F3 0F11 8F EC000000   - movss [rdi+000000EC],xmm1    ; WRITE Y
;NewColossus_x64vk.exe+FE8EED - 0F11 87 F4000000      - movups [rdi+000000F4],xmm0   ; WRITE Matrix values 0-3
;NewColossus_x64vk.exe+FE8EF4 - 0F10 45 F7            - movups xmm0,[rbp-09]         ;
;NewColossus_x64vk.exe+FE8EF8 - 0F11 87 04010000      - movups [rdi+00000104],xmm0   ; WRITE Matrix values 3-7
;NewColossus_x64vk.exe+FE8EFF - F3 0F10 45 07         - movss xmm0,[rbp+07]          ;
;NewColossus_x64vk.exe+FE8F04 - F3 0F11 87 14010000   - movss [rdi+00000114],xmm0    ; WRITE Matrix value 8
;NewColossus_x64vk.exe+FE8F0C - 48 8B CF              - mov rcx,rdi                  ; Continue here
	mov [g_cameraStructAddress], rdi		; store the camera struct in the variable
	cmp byte ptr [g_cameraEnabled], 1		; if our camera is enabled, skip writes
	je exit
originalCode:
	movss dword ptr [rdi+000000E8h],xmm0			; original code from the game, intercepted
	movss xmm0,dword ptr [rbp-31h]
	movss dword ptr [rdi+000000F0h],xmm0
	movups xmm0,xmmword ptr [rbp-19h]
	movss dword ptr [rdi+000000ECh],xmm1
	movups xmmword ptr [rdi+000000F4h],xmm0
	movups xmm0,xmmword ptr [rbp-9h]
	movups xmmword ptr [rdi+00000104h],xmm0
	movss xmm0,dword ptr [rbp+7h]
	movss dword ptr [rdi+00000114h],xmm0
exit:
	jmp qword ptr [_cameraStructInterceptionContinue] ; jmp back into the original game code, which is the location after the original statements above.
cameraAddressInterceptor ENDP

fovAddressInterceptor PROC
;NewColossus_x64vk.exe+FE8C58 - 81 25 1ED4FA01 FFFFFBFF - and [NewColossus_x64vk.exe+2F96080],FFFBFFFF
;NewColossus_x64vk.exe+FE8C62 - 48 8B 06              - mov rax,[rsi]					; Intercept here
;NewColossus_x64vk.exe+FE8C65 - 48 8D 8B C04F0000     - lea rcx,[rbx+00004FC0]			; Load FoV Address
;NewColossus_x64vk.exe+FE8C6C - 48 8D 55 67           - lea rdx,[rbp+67]
;NewColossus_x64vk.exe+FE8C70 - 48 89 45 67           - mov [rbp+67],rax	
;NewColossus_x64vk.exe+FE8C74 - E8 37C596FF           - call NewColossus_x64vk.exe+9551B0		; call FoV function; Continue here
;NewColossus_x64vk.exe+FE8C79 - 65 48 8B 04 25 58000000  - mov rax,gs:[00000058]
;NewColossus_x64vk.exe+FE8C82 - 48 8D 8F 98000000     - lea rcx,[rdi+00000098]
;NewColossus_x64vk.exe+FE8C89 - 0F57 E4               - xorps xmm4,xmm4
originalCode:
	mov rax,[rsi]			
	lea rcx,[rbx+00004FC0h]    ; Read the FoV address in rcx
	lea rdx,[rbp+67h]
	mov [rbp+67h],rax
exit:
	mov [g_fovStructAddress], rcx    ; Store the address in the variable
	jmp qword ptr [_fovAddressInterceptionContinue] ; jmp back into the original game code, which is the location after the original statements above.
fovAddressInterceptor ENDP

runFramesAddressInterceptor PROC
;NewColossus_x64vk.exe+11B5343 - 48 8B C8              - mov rcx,rax
;NewColossus_x64vk.exe+11B5346 - 48 8B 00              - mov rax,[rax]
;NewColossus_x64vk.exe+11B5349 - 48 8B 90 B8000000     - mov rdx,[rax+000000B8]
;NewColossus_x64vk.exe+11B5350 - 4C 8D 05 31195001     - lea r8,[NewColossus_x64vk.exe+26B6C88]
;NewColossus_x64vk.exe+11B5357 - 49 3B C0              - cmp rax,r8								<< INTERCEPT HERE
;NewColossus_x64vk.exe+11B535A - 75 09                 - jne NewColossus_x64vk.exe+11B5365
;NewColossus_x64vk.exe+11B535C - 48 8D 81 F0010000     - lea rax,[rcx+000001F0]
;NewColossus_x64vk.exe+11B5363 - EB 02                 - jmp NewColossus_x64vk.exe+11B5367
;NewColossus_x64vk.exe+11B5365 - FF D2                 - call rdx
;NewColossus_x64vk.exe+11B5367 - 48 8B 48 48           - mov rcx,[rax+48]
;NewColossus_x64vk.exe+11B536B - 8B 51 30              - mov edx,[rcx+30]						<< READ g_runFrames.
;NewColossus_x64vk.exe+11B536E - 85 D2                 - test edx,edx							<< CONTINUE HERE
;NewColossus_x64vk.exe+11B5370 - 7E 07                 - jle NewColossus_x64vk.exe+11B5379
;NewColossus_x64vk.exe+11B5372 - FF CA                 - dec edx
;NewColossus_x64vk.exe+11B5374 - E8 07AC0700           - call NewColossus_x64vk.exe+122FF80
originalCode:
	cmp rax,r8							
	jne c1
	lea rax,[rcx+000001F0h]
	jmp c2
c1:
	call rdx
c2:
	mov rcx,[rax+48h]
	mov edx,[rcx+30h]
exit:
	mov [g_runFramesStructAddress], rcx    ; Store the address in the variable
	jmp qword ptr [_runFramesAddressInterceptionContinue] ; jmp back into the original game code, which is the location after the original statements above.
runFramesAddressInterceptor ENDP

END