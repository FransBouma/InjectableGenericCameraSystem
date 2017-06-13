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
PUBLIC cameraWrite1Interceptor
PUBLIC cameraWrite2Interceptor
PUBLIC cameraWrite3Interceptor
PUBLIC cameraWrite4Interceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_cameraStructAddress: qword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWrite1InterceptionContinue: qword
EXTERN _cameraWrite2InterceptionContinue: qword
EXTERN _cameraWrite3InterceptionContinue: qword
EXTERN _cameraWrite4InterceptionContinue: qword

.data

;---------------------------------------------------------------
; Scratch pad
.code


cameraStructInterceptor PROC
; Struct interceptor is also a write interceptor for matrix
;mafia3.exe+96628A - F3 0F59 81 84000000   - mulss xmm0,[rcx+00000084]			<< INTERCEPT HERE
;mafia3.exe+966292 - 4C 89 65 7F           - mov [rbp+7F],r12
;mafia3.exe+966296 - F3 0F59 89 88000000   - mulss xmm1,[rcx+00000088]
;mafia3.exe+96629E - F3 0F59 99 8C000000   - mulss xmm3,[rcx+0000008C]
;mafia3.exe+9662A6 - F3 0F58 41 48         - addss xmm0,[rcx+48]				<< CONTINUE HERE
;mafia3.exe+9662AB - F3 0F58 49 4C         - addss xmm1,[rcx+4C]
;mafia3.exe+9662B0 - F3 0F58 59 50         - addss xmm3,[rcx+50]
;mafia3.exe+9662B5 - F3 0F11 41 48         - movss [rcx+48],xmm0				<< WRITE X
;mafia3.exe+9662BA - F3 0F11 49 4C         - movss [rcx+4C],xmm1				<< WRITE Y
;mafia3.exe+9662BF - F3 0F11 59 50         - movss [rcx+50],xmm3				<< WRITE Z
;mafia3.exe+9662C4 - 48 8B 81 C0010000     - mov rax,[rcx+000001C0]				
;mafia3.exe+9662CB - 48 2B 81 B8010000     - sub rax,[rcx+000001B8]
	mov [g_cameraStructAddress], rcx
	; always do the original code, we're not intercepting anything here. 
originalCode:
	mulss xmm0, dword ptr [rcx+00000084h]
	mov [rbp+7Fh],r12
	mulss xmm1, dword ptr [rcx+00000088h]
	mulss xmm3, dword ptr [rcx+0000008Ch]
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP


cameraWrite1Interceptor PROC
;mafia3.exe+963D11 - F3 0F59 C7            - mulss xmm0,xmm7				
;mafia3.exe+963D15 - 0F28 7C 24 50         - movaps xmm7,[rsp+50]
;mafia3.exe+963D1A - F3 0F59 CD            - mulss xmm1,xmm5
;mafia3.exe+963D1E - F3 0F5C E0            - subss xmm4,xmm0				
;mafia3.exe+963D22 - F3 0F11 17            - movss [rdi],xmm2				<< INTERCEPT HERE << WRITE _11
;mafia3.exe+963D26 - F3 0F5C CB            - subss xmm1,xmm3                 
;mafia3.exe+963D2A - F3 0F11 67 10         - movss [rdi+10],xmm4             << WRITE _21
;mafia3.exe+963D2F - F3 0F11 4F 20         - movss [rdi+20],xmm1             << WRITE _31
;mafia3.exe+963D34 - 48 83 C4 70           - add rsp,70 { 112 }				<< CONTINUE HERE
;mafia3.exe+963D38 - 5F                    - pop rdi
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
	jmp exit
	;cmp qword ptr rdi, [g_cameraStructAddress]
	;je exit												; destination is the matrix we're after, do skip everything
originalCode:
	movss dword ptr [rdi],xmm2	
	subss xmm1,xmm3     
	movss dword ptr [rdi+10h],xmm4 
	movss dword ptr [rdi+20h],xmm1 
exit:
	jmp qword ptr [_cameraWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite1Interceptor ENDP


cameraWrite2Interceptor PROC
;mafia3.exe+963B43 - 8B 06                 - mov eax,[rsi]					
;mafia3.exe+963B45 - 89 47 04              - mov [rdi+04],eax				<< INTERCEPT HERE << WRITE _12
;mafia3.exe+963B48 - 8B 46 04              - mov eax,[rsi+04]
;mafia3.exe+963B4B - 89 47 14              - mov [rdi+14],eax				<< WRITE _22
;mafia3.exe+963B4E - 8B 46 08              - mov eax,[rsi+08]
;mafia3.exe+963B51 - 89 47 24              - mov [rdi+24],eax				<< WRITE _32
;mafia3.exe+963B54 - F3 0F10 6B 04         - movss xmm5,[rbx+04]			<< CONTINUE HERE
;mafia3.exe+963B59 - F3 0F10 33            - movss xmm6,[rbx]
;mafia3.exe+963B5D - 0F28 DD               - movaps xmm3,xmm5
;mafia3.exe+963B60 - F3 0F10 7B 08         - movss xmm7,[rbx+08]
;mafia3.exe+963B65 - 0F28 C6               - movaps xmm0,xmm6
;mafia3.exe+963B68 - F3 44 0F10 46 04      - movss xmm8,[rsi+04]
;mafia3.exe+963B6E - 0F28 CF               - movaps xmm1,xmm7
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
	jmp exit
	;cmp qword ptr rdi, [g_cameraStructAddress]
	;je exit												; destination is the matrix we're after, do skip everything
originalCode:
	mov [rdi+04h],eax	
	mov eax,[rsi+04h]
	mov [rdi+14h],eax	
	mov eax,[rsi+08h]
	mov [rdi+24h],eax	
exit:
	jmp qword ptr [_cameraWrite2InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite2Interceptor ENDP

cameraWrite3Interceptor PROC
;mafia3.exe+963CBE - F3 0F59 F2            - mulss xmm6,xmm2
;mafia3.exe+963CC2 - F3 0F59 EA            - mulss xmm5,xmm2
;mafia3.exe+963CC6 - F3 0F59 FA            - mulss xmm7,xmm2
;mafia3.exe+963CCA - F3 0F11 77 08         - movss [rdi+08],xmm6			<< INTERCEPT HERE << WRITE _13
;mafia3.exe+963CCF - F3 0F11 6F 18         - movss [rdi+18],xmm5             << WRITE _23
;mafia3.exe+963CD4 - F3 0F11 7F 28         - movss [rdi+28],xmm7             << WRITE _33
;mafia3.exe+963CD9 - F3 0F10 5E 04         - movss xmm3,[rsi+04]			<< CONTINUE HERE
;mafia3.exe+963CDE - F3 0F10 66 08         - movss xmm4,[rsi+08]
;mafia3.exe+963CE3 - 0F28 D3               - movaps xmm2,xmm3
;mafia3.exe+963CE6 - F3 0F10 0E            - movss xmm1,[rsi]
;mafia3.exe+963CEA - 0F28 C4               - movaps xmm0,xmm4
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
	jmp exit
	cmp qword ptr rdi, [g_cameraStructAddress]
	je exit												; destination is the matrix we're after, do skip everything
originalCode:
	movss dword ptr [rdi+08h],xmm6
	movss dword ptr [rdi+18h],xmm5
	movss dword ptr [rdi+28h],xmm7
exit:
	jmp qword ptr [_cameraWrite3InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite3Interceptor ENDP

cameraWrite4Interceptor PROC
;mafia3.exe+9666AF - 89 47 50              - mov [rdi+50],eax
;mafia3.exe+9666B2 - 8B 44 24 58           - mov eax,[rsp+58]
;mafia3.exe+9666B6 - 89 47 5C              - mov [rdi+5C],eax
;mafia3.exe+9666B9 - 8B 44 24 48           - mov eax,[rsp+48]
;mafia3.exe+9666BD - 41 89 40 08           - mov [r8+08],eax
;mafia3.exe+9666C1 - F3 44 0F11 47 70      - movss [rdi+70],xmm8
;mafia3.exe+9666C7 - F3 44 0F11 4F 6C      - movss [rdi+6C],xmm9			<< INTERCEPT HERE << WRITE FOV
;mafia3.exe+9666CD - 8B 47 48              - mov eax,[rdi+48]				
;mafia3.exe+9666D0 - 89 47 24              - mov [rdi+24],eax				<<< X
;mafia3.exe+9666D3 - 8B 47 4C              - mov eax,[rdi+4C]
;mafia3.exe+9666D6 - 89 47 34              - mov [rdi+34],eax				<<< Y
;mafia3.exe+9666D9 - 8B 47 50              - mov eax,[rdi+50]
;mafia3.exe+9666DC - 89 47 44              - mov [rdi+44],eax				<<< Z
;mafia3.exe+9666DF - F3 0F10 47 54         - movss xmm0,[rdi+54]			<< CONTINUE HERE
;mafia3.exe+9666E4 - F3 0F5C 47 48         - subss xmm0,[rdi+48]
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
	jmp exit
	;cmp qword ptr rdi, [g_cameraStructAddress]
	;je exit												; destination is the matrix we're after, do skip everything
originalCode:
	movss dword ptr [rdi+6Ch],xmm9								; write fov
	mov eax,[rdi+48h]	
	mov [rdi+24h],eax
	mov eax,[rdi+4Ch]
	mov [rdi+34h],eax
	mov eax,[rdi+50h]
	mov [rdi+44h],eax
exit:
	jmp qword ptr [_cameraWrite4InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite4Interceptor ENDP

END