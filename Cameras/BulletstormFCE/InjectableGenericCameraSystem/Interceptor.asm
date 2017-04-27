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
PUBLIC cameraCinematicsStructInterceptor
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
EXTERN g_cinematicsCameraStructAddress: qword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWrite1InterceptionContinue: qword
EXTERN _cameraWrite2InterceptionContinue: qword
EXTERN _cameraWrite3InterceptionContinue: qword
EXTERN _cameraWrite4InterceptionContinue: qword
EXTERN _cameraCinematicsStructInterceptionContinue: qword
.data

;---------------------------------------------------------------
; Scratch pad
;---------------------------------------------------------------
.code


cameraStructInterceptor PROC
; Struct interceptor is also a write interceptor.
;StormGame-Win64-Shipping.exe+4E4EDFC - FF 90 80080000        - call qword ptr [rax+00000880]
;StormGame-Win64-Shipping.exe+4E4EE02 - 8B 08                 - mov ecx,[rax]			<<< INTERCEPT HERE
;StormGame-Win64-Shipping.exe+4E4EE04 - 89 0B                 - mov [rbx],ecx				<< WRITE X
;StormGame-Win64-Shipping.exe+4E4EE06 - 8B 48 04              - mov ecx,[rax+04]
;StormGame-Win64-Shipping.exe+4E4EE09 - 89 4B 04              - mov [rbx+04],ecx			<< WRITE Y
;StormGame-Win64-Shipping.exe+4E4EE0C - 8B 40 08              - mov eax,[rax+08]
;StormGame-Win64-Shipping.exe+4E4EE0F - 89 43 08              - mov [rbx+08],eax			<< WRITE Z
;StormGame-Win64-Shipping.exe+4E4EE12 - 48 8B 5C 24 40        - mov rbx,[rsp+40]		<<< CONTINUE HERE
;StormGame-Win64-Shipping.exe+4E4EE17 - 48 83 C4 30           - add rsp,30				
	mov [g_cameraStructAddress], rbx
	mov ecx,[rax]										; original first code, always do this
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	mov [rbx],ecx										; write x
	mov ecx,[rax+04h]
	mov [rbx+04h],ecx									; write y
	mov eax,[rax+08h]
	mov [rbx+08h],eax									; write z
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP

cameraCinematicsStructInterceptor PROC
;StormGame-Win64-Shipping.exe+56CA7DF - 0F29 B0 C8FFFFFF      - movaps [rax-00000038],xmm6			
;StormGame-Win64-Shipping.exe+56CA7E6 - 0F29 B8 B8FFFFFF      - movaps [rax-00000048],xmm7			<< INTERCEPT HERE
;StormGame-Win64-Shipping.exe+56CA7ED - 8B 06                 - mov eax,[rsi]								<<<<<<<< READ rotX here.
;StormGame-Win64-Shipping.exe+56CA7EF - 48 8D B9 80000000     - lea rdi,[rcx+00000080]
;StormGame-Win64-Shipping.exe+56CA7F6 - 0F28 FB               - movaps xmm7,xmm3					<< CONTINUE HERE
;StormGame-Win64-Shipping.exe+56CA7F9 - 89 44 24 50           - mov [rsp+50],eax
;StormGame-Win64-Shipping.exe+56CA7FD - 8B 46 04              - mov eax,[rsi+04]
;StormGame-Win64-Shipping.exe+56CA800 - 48 89 CB              - mov rbx,rcx							
;StormGame-Win64-Shipping.exe+56CA803 - F3 0F10 07            - movss xmm0,[rdi]
;StormGame-Win64-Shipping.exe+56CA807 - F3 0F10 4F 04         - movss xmm1,[rdi+04]
	push rsi
	sub rsi, 0Ch
	mov [g_cinematicsCameraStructAddress], rsi
	pop rsi
originalCode:
	movaps xmmword ptr [rax-00000048h],xmm7
	mov eax,[rsi]				
	lea rdi,[rcx+00000080h]
exit:
	jmp qword ptr [_cameraCinematicsStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraCinematicsStructInterceptor ENDP

cameraWrite1Interceptor PROC
;StormGame-Win64-Shipping.exe+EC8DBA - F3 41 0F10 47 10      - movss xmm0,[r15+10]		<< INTERCEPT HERE
;StormGame-Win64-Shipping.exe+EC8DC0 - F3 0F58 00            - addss xmm0,[rax]
;StormGame-Win64-Shipping.exe+EC8DC4 - F3 41 0F11 47 10      - movss [r15+10],xmm0			<< WRITE X
;StormGame-Win64-Shipping.exe+EC8DCA - F3 0F10 48 04         - movss xmm1,[rax+04]
;StormGame-Win64-Shipping.exe+EC8DCF - F3 41 0F58 4F 14      - addss xmm1,[r15+14]
;StormGame-Win64-Shipping.exe+EC8DD5 - F3 41 0F11 4F 14      - movss [r15+14],xmm1			<< WRITE Y
;StormGame-Win64-Shipping.exe+EC8DDB - F3 0F10 40 08         - movss xmm0,[rax+08]
;StormGame-Win64-Shipping.exe+EC8DE0 - F3 41 0F58 47 18      - addss xmm0,[r15+18]
;StormGame-Win64-Shipping.exe+EC8DE6 - F3 41 0F11 47 18      - movss [r15+18],xmm0			<< WRITE Z
;StormGame-Win64-Shipping.exe+EC8DEC - F7 86 98050000 00100000 - test [rsi+00000598],1000 <<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne exit											; our own camera is enabled, just skip the writes
noWrite:
	; perform the other statements.
	movss xmm0, dword ptr [r15+10h]
	addss xmm0, dword ptr [rax]
	movss xmm1, dword ptr [rax+04h]
	addss xmm1, dword ptr [r15+14h]
	movss xmm0, dword ptr [rax+08h]
	addss xmm0, dword ptr [r15+18h]
	jmp exit
originalCode:
	movss xmm0, dword ptr [r15+10h]
	addss xmm0, dword ptr [rax]
	movss dword ptr [r15+10h],xmm0
	movss xmm1, dword ptr [rax+04h]
	addss xmm1, dword ptr [r15+14h]
	movss dword ptr [r15+14h],xmm1
	movss xmm0, dword ptr [rax+08h]
	addss xmm0, dword ptr [r15+18h]
	movss dword ptr [r15+18h],xmm0
exit:
	jmp qword ptr [_cameraWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite1Interceptor ENDP

cameraWrite2Interceptor PROC
;StormGame-Win64-Shipping.exe+4E4839C - FF 90 78080000        - call qword ptr [rax+00000878]
;StormGame-Win64-Shipping.exe+4E483A2 - 8B 08                 - mov ecx,[rax]			<< INTERCEPT HERE
;StormGame-Win64-Shipping.exe+4E483A4 - 89 0B                 - mov [rbx],ecx					<< ROT X write
;StormGame-Win64-Shipping.exe+4E483A6 - 8B 48 04              - mov ecx,[rax+04]
;StormGame-Win64-Shipping.exe+4E483A9 - 89 4B 04              - mov [rbx+04],ecx
;StormGame-Win64-Shipping.exe+4E483AC - 8B 40 08              - mov eax,[rax+08]
;StormGame-Win64-Shipping.exe+4E483AF - 89 43 08              - mov [rbx+08],eax
;StormGame-Win64-Shipping.exe+4E483B2 - 48 8B 5C 24 40        - mov rbx,[rsp+40]
;StormGame-Win64-Shipping.exe+4E483B7 - 48 83 C4 30           - add rsp,30 { 48 }		<< CONTINUE HERE
;StormGame-Win64-Shipping.exe+4E483BB - 5F                    - pop rdi
;StormGame-Win64-Shipping.exe+4E483BC - C3                    - ret 
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	mov ecx,[rax]	
	mov [rbx],ecx	
	mov ecx,[rax+04h]
	mov [rbx+04h],ecx
	mov eax,[rax+08h]
	mov [rbx+08h],eax
exit:
	mov rbx,[rsp+40h]
	jmp qword ptr [_cameraWrite2InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite2Interceptor ENDP

cameraWrite3Interceptor PROC
;StormGame-Win64-Shipping.exe+EC928F - 8B 08                 - mov ecx,[rax]			<< INTERCEPT HERE
;StormGame-Win64-Shipping.exe+EC9291 - 89 0B                 - mov [rbx],ecx						<< ROT X write
;StormGame-Win64-Shipping.exe+EC9293 - 8B 48 04              - mov ecx,[rax+04]
;StormGame-Win64-Shipping.exe+EC9296 - 89 4B 04              - mov [rbx+04],ecx
;StormGame-Win64-Shipping.exe+EC9299 - 8B 48 08              - mov ecx,[rax+08]
;StormGame-Win64-Shipping.exe+EC929C - 89 4B 08              - mov [rbx+08],ecx
;StormGame-Win64-Shipping.exe+EC929F - 8B 08                 - mov ecx,[rax]			<<< CONTINUE HERE
;StormGame-Win64-Shipping.exe+EC92A1 - 41 89 0C 24           - mov [r12],ecx
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	mov ecx,[rax]	
	mov [rbx],ecx	
	mov ecx,[rax+04h]
	mov [rbx+04h],ecx
	mov ecx,[rax+08h]
	mov [rbx+08h],ecx
exit:
	jmp qword ptr [_cameraWrite3InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite3Interceptor ENDP

cameraWrite4Interceptor PROC
;StormGame-Win64-Shipping.exe+EC92CB - 8B 45 90              - mov eax,[rbp-70]				<< INTERCEPT HERE
;StormGame-Win64-Shipping.exe+EC92CE - 01 03                 - add [rbx],eax						<< ROT X write
;StormGame-Win64-Shipping.exe+EC92D0 - 8B 45 A8              - mov eax,[rbp-58]
;StormGame-Win64-Shipping.exe+EC92D3 - 01 43 04              - add [rbx+04],eax
;StormGame-Win64-Shipping.exe+EC92D6 - 8B 44 24 60           - mov eax,[rsp+60]
;StormGame-Win64-Shipping.exe+EC92DA - 01 43 08              - add [rbx+08],eax
;StormGame-Win64-Shipping.exe+EC92DD - 48 8D 1D 1C6D13FF     - lea rbx,[StormGame-Win64-Shipping.exe] <<<<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	mov eax,[rbp-70h]
	add [rbx],eax	
	mov eax,[rbp-58h]
	add [rbx+04h],eax
	mov eax,[rsp+60h]
	add [rbx+08h],eax
exit:
	jmp qword ptr [_cameraWrite4InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite4Interceptor ENDP

END