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

.data

;---------------------------------------------------------------
; Scratch pad
;
.code


cameraStructInterceptor PROC
; camera address interceptor is also a write blocker. 
;client.dll+142C4A - FF 90 A0050000        - call qword ptr [rax+000005A0]
;client.dll+142C50 - 48 8D 55 B7           - lea rdx,[rbp-49]
;client.dll+142C54 - 8B 08                 - mov ecx,[rax]
;client.dll+142C56 - 89 0E                 - mov [rsi],ecx						<< INTERCEPT HERE << WRITE X
;client.dll+142C58 - 8B 48 04              - mov ecx,[rax+04]
;client.dll+142C5B - 89 4E 04              - mov [rsi+04],ecx					<< WRITE Y
;client.dll+142C5E - 8B 40 08              - mov eax,[rax+08]
;client.dll+142C61 - 48 8B CB              - mov rcx,rbx
;client.dll+142C64 - 89 46 08              - mov [rsi+08],eax					<< WRITE Z
;client.dll+142C67 - 48 8B 03              - mov rax,[rbx]						<< CONTINUE HERE
;client.dll+142C6A - FF 90 B0050000        - call qword ptr [rax+000005B0]
;client.dll+142C70 - 8B 08                 - mov ecx,[rax]
;client.dll+142C72 - 89 0F                 - mov [rdi],ecx
;client.dll+142C74 - 8B 48 04              - mov ecx,[rax+04]
;client.dll+142C77 - 89 4F 04              - mov [rdi+04],ecx
;client.dll+142C7A - 8B 40 08              - mov eax,[rax+08]
;client.dll+142C7D - 89 47 08              - mov [rdi+08],eax
;client.dll+142C80 - E8 8BAA3A00           - call client.dll+4ED710
;client.dll+142C85 - 33 D2                 - xor edx,edx
;client.dll+142C87 - 48 8B CB              - mov rcx,rbx

	mov [g_cameraStructAddress], rsi
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	mov [rsi],ecx	
	mov ecx,[rax+04h]
	mov [rsi+04h],ecx
	mov eax,[rax+08h]
	mov [rsi+08h],eax
exit:
	mov rcx,rbx
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP


cameraWrite1Interceptor PROC
;client.dll+10E093 - 48 8B D5              - mov rdx,rbp
;client.dll+10E096 - E8 45F35100           - call client.dll+62D3E0
;client.dll+10E09B - F3 0F10 44 24 2C      - movss xmm0,[rsp+2C]
;client.dll+10E0A1 - F3 0F10 4C 24 3C      - movss xmm1,[rsp+3C]
;client.dll+10E0A7 - F3 0F11 07            - movss [rdi],xmm0						<< INTERCEPT HERE  << WRITE X
;client.dll+10E0AB - F3 0F10 44 24 4C      - movss xmm0,[rsp+4C]
;client.dll+10E0B1 - F3 0F11 4F 04         - movss [rdi+04],xmm1					<< WRITE Y
;client.dll+10E0B6 - F3 0F11 47 08         - movss [rdi+08],xmm0					<< WRITE Z
;client.dll+10E0BB - 4C 8D 9C 24 90010000  - lea r11,[rsp+00000190]					<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rdi],xmm0	
	movss xmm0, dword ptr [rsp+4Ch]
	movss dword ptr [rdi+04h],xmm1
	movss dword ptr [rdi+08h],xmm0
exit:
	jmp qword ptr [_cameraWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite1Interceptor ENDP


cameraWrite2Interceptor PROC
;client.dll+4E3E89 - F3 0F10 44 24 4C      - movss xmm0,[rsp+4C]
;client.dll+4E3E8F - F3 0F10 4C 24 5C      - movss xmm1,[rsp+5C]
;client.dll+4E3E95 - F3 0F11 07            - movss [rdi],xmm0						<< INTERCEPT HERE << WRITE X
;client.dll+4E3E99 - F3 0F10 44 24 6C      - movss xmm0,[rsp+6C]
;client.dll+4E3E9F - F3 0F11 4F 04         - movss [rdi+04],xmm1					<< WRITE Y
;client.dll+4E3EA4 - F3 0F11 47 08         - movss [rdi+08],xmm0					<< WRITE Z
;client.dll+4E3EA9 - 4C 8D 9C 24 60010000  - lea r11,[rsp+00000160]					<< CONTINUE HERE
;client.dll+4E3EB1 - 49 8B 5B 10           - mov rbx,[r11+10]
;client.dll+4E3EB5 - 49 8B 73 18           - mov rsi,[r11+18]
;client.dll+4E3EB9 - 49 8B E3              - mov rsp,r11
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rdi],xmm0	
	movss xmm0, dword ptr [rsp+6Ch]
	movss dword ptr [rdi+04h],xmm1
	movss dword ptr [rdi+08h],xmm0
exit:
	jmp qword ptr [_cameraWrite2InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite2Interceptor ENDP


cameraWrite3Interceptor PROC
;client.dll+142C6A - FF 90 B0050000        - call qword ptr [rax+000005B0]
;client.dll+142C70 - 8B 08                 - mov ecx,[rax]					<< INTERCEPT HERE
;client.dll+142C72 - 89 0F                 - mov [rdi],ecx					<< WRITE X rot value  (x is right)
;client.dll+142C74 - 8B 48 04              - mov ecx,[rax+04]
;client.dll+142C77 - 89 4F 04              - mov [rdi+04],ecx				<< WRITE Z rot value  (z is up)
;client.dll+142C7A - 8B 40 08              - mov eax,[rax+08]
;client.dll+142C7D - 89 47 08              - mov [rdi+08],eax				<< WRITE Y rot value  (y is into the screen)
;client.dll+142C80 - E8 8BAA3A00           - call client.dll+4ED710			<< CONTINUE HERE
;client.dll+142C85 - 33 D2                 - xor edx,edx
;client.dll+142C87 - 48 8B CB              - mov rcx,rbx
;client.dll+142C8A - E8 41E01700           - call client.dll+2C0CD0
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	mov ecx,[rax]	
	mov [rdi],ecx	
	mov ecx,[rax+04h]
	mov [rdi+04h],ecx
	mov eax,[rax+08h]
	mov [rdi+08h],eax
exit:
	jmp qword ptr [_cameraWrite3InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite3Interceptor ENDP

END