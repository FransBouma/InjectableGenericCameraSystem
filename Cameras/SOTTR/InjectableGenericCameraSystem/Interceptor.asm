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

.data

;---------------------------------------------------------------
; Scratch pad

.code


cameraStructInterceptor PROC
;SOTTR.exe+498F0A - 41 56                 - push r14
;SOTTR.exe+498F0C - 41 57                 - push r15
;SOTTR.exe+498F0E - 48 8D 6C 24 A8        - lea rbp,[rsp-58]
;SOTTR.exe+498F13 - 48 81 EC 58010000     - sub rsp,00000158 { 344 }
;SOTTR.exe+498F1A - 41 B1 01              - mov r9l,01 { 1 }
;SOTTR.exe+498F1D - 33 DB                 - xor ebx,ebx
;SOTTR.exe+498F1F - 89 59 28              - mov [rcx+28],ebx							<< INTERCEPT HERE
;SOTTR.exe+498F22 - 45 33 C0              - xor r8d,r8d
;SOTTR.exe+498F25 - 0F28 82 80000000      - movaps xmm0,[rdx+00000080]					<< Camera struct address in rdx.
;SOTTR.exe+498F2C - 4C 8B F9              - mov r15,rcx
;SOTTR.exe+498F2F - 41 0FB6 D1            - movzx edx,r9l								<< CONTINUE HERE
;SOTTR.exe+498F33 - 0F29 44 24 70         - movaps [rsp+70],xmm0
;SOTTR.exe+498F38 - E8 73EDFCFF           - call SOTTR.exe+467CB0
;SOTTR.exe+498F3D - 49 8B 8F E0070000     - mov rcx,[r15+000007E0]
;SOTTR.exe+498F44 - 4C 8B E8              - mov r13,rax
;SOTTR.exe+498F47 - 48 89 44 24 58        - mov [rsp+58],rax
;SOTTR.exe+498F4C - 48 85 C9              - test rcx,rcx
;SOTTR.exe+498F4F - 74 3E                 - je SOTTR.exe+498F8F
;SOTTR.exe+498F51 - 48 8B 49 08           - mov rcx,[rcx+08]
;SOTTR.exe+498F55 - 48 8D 35 A49D1003     - lea rsi,[SOTTR.exe+35A2D00] { (672) }
;SOTTR.exe+498F5C - 48 89 74 24 48        - mov [rsp+48],rsi
;SOTTR.exe+498F61 - 0FB7 91 B0000000      - movzx edx,word ptr [rcx+000000B0]
;SOTTR.exe+498F68 - 8D 4B 20              - lea ecx,[rbx+20]
;SOTTR.exe+498F6B - 0F1F 44 00 00         - nop dword ptr [rax+rax+00]
	mov [rcx+28h],ebx			
	xor r8d,r8d
	mov [g_cameraStructAddress], rdx
	movaps xmm0, xmmword ptr [rdx+00000080h]
	mov r15,rcx
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP


cameraWrite1Interceptor PROC
; Write is used by many paths / targets, only block writes for the camera address we found (if any)
;SOTTR.exe+5EE33C8 - 66 0F7F 41 60         - movdqa [rcx+60],xmm0
;SOTTR.exe+5EE33CD - 0F28 4A 70            - movaps xmm1,[rdx+70]
;SOTTR.exe+5EE33D1 - 66 0F7F 49 70         - movdqa [rcx+70],xmm1
;SOTTR.exe+5EE33D6 - 0F28 82 80000000      - movaps xmm0,[rdx+00000080]


;SOTTR.exe+5EE33DD - 66 0F7F 81 80000000   - movdqa [rcx+00000080],xmm0					<< INTERCEPT HERE << WRITE COORDS
;SOTTR.exe+5EE33E5 - 0F28 8A A0000000      - movaps xmm1,[rdx+000000A0]
;SOTTR.exe+5EE33EC - 0F29 89 A0000000      - movaps [rcx+000000A0],xmm1					<< WRITE QUATERNION
;SOTTR.exe+5EE33F3 - 8B 82 B0000000        - mov eax,[rdx+000000B0]						<< WRITE FOV
;SOTTR.exe+5EE33F9 - 89 81 B0000000        - mov [rcx+000000B0],eax
;SOTTR.exe+5EE33FF - 8B 82 B4000000        - mov eax,[rdx+000000B4]						<< CONTINUE HERE
;SOTTR.exe+5EE3405 - 89 81 B4000000        - mov [rcx+000000B4],eax
;SOTTR.exe+5EE340B - 8B 82 B8000000        - mov eax,[rdx+000000B8]
;SOTTR.exe+5EE3411 - 89 81 B8000000        - mov [rcx+000000B8],eax
;SOTTR.exe+5EE3417 - 8B 82 BC000000        - mov eax,[rdx+000000BC]
;SOTTR.exe+5EE341D - 89 81 BC000000        - mov [rcx+000000BC],eax
;SOTTR.exe+5EE3423 - 0F28 82 C0000000      - movaps xmm0,[rdx+000000C0]
;SOTTR.exe+5EE342A - 66 0F7F 81 C0000000   - movdqa [rcx+000000C0],xmm0
;SOTTR.exe+5EE3432 - 0F28 8A D0000000      - movaps xmm1,[rdx+000000D0]
	cmp byte ptr [g_cameraEnabled], 1
	jne originalCode
	cmp qword ptr rcx, [g_cameraStructAddress]
	je exit
originalCode:
	movdqa xmmword ptr [rcx+00000080h],xmm0
	movaps xmm1, xmmword ptr [rdx+000000A0h]
	movaps xmmword ptr [rcx+000000A0h],xmm1
	mov eax,[rdx+000000B0h]		
	mov [rcx+000000B0h],eax
exit:
	jmp qword ptr [_cameraWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite1Interceptor ENDP

END