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
PUBLIC timestopReadInterceptor

;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_cameraStructAddress: qword
EXTERN g_timestopStructAddress: qword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWrite1InterceptionContinue: qword
EXTERN _timestopInterceptionContinue: qword

.data

;---------------------------------------------------------------
; Scratch pad

.code


cameraStructInterceptor PROC
; camera address interceptor is also a write blocker. If the camera is enabled, we simply issue a 'ret' as the whole routine is simply copying the 
; data from stack to the memory location.

;TEW2.exe+6B5410 - F3 0F10 0A            - movss xmm1,[rdx]				<< INTERCEPT HERE
;TEW2.exe+6B5414 - F3 41 0F10 00         - movss xmm0,[r8]
;TEW2.exe+6B5419 - 0F5A C0               - cvtps2pd xmm0,xmm0
;TEW2.exe+6B541C - 0F5A C9               - cvtps2pd xmm1,xmm1			
;TEW2.exe+6B541F - F2 0F58 C8            - addsd xmm1,xmm0				<< CONTINUE HERE
;TEW2.exe+6B5423 - F2 0F11 89 88000000   - movsd [rcx+00000088],xmm1			// X
;TEW2.exe+6B542B - F3 0F10 52 04         - movss xmm2,[rdx+04]
;TEW2.exe+6B5430 - F3 41 0F10 40 04      - movss xmm0,[r8+04]
;TEW2.exe+6B5436 - 0F5A C0               - cvtps2pd xmm0,xmm0
;TEW2.exe+6B5439 - 0F5A D2               - cvtps2pd xmm2,xmm2
;TEW2.exe+6B543C - F2 0F58 D0            - addsd xmm2,xmm0
;TEW2.exe+6B5440 - F2 0F11 91 90000000   - movsd [rcx+00000090],xmm2			// Y
;TEW2.exe+6B5448 - F3 0F10 4A 08         - movss xmm1,[rdx+08]
;TEW2.exe+6B544D - F3 41 0F10 40 08      - movss xmm0,[r8+08]
;TEW2.exe+6B5453 - 0F5A C9               - cvtps2pd xmm1,xmm1
;TEW2.exe+6B5456 - 0F5A C0               - cvtps2pd xmm0,xmm0
;TEW2.exe+6B5459 - F2 0F58 C8            - addsd xmm1,xmm0
;TEW2.exe+6B545D - F2 0F11 89 98000000   - movsd [rcx+00000098],xmm1			// Z
;TEW2.exe+6B5465 - 41 8B 01              - mov eax,[r9]
;TEW2.exe+6B5468 - 89 81 A0000000        - mov [rcx+000000A0],eax			// Matrix
;TEW2.exe+6B546E - 41 8B 41 04           - mov eax,[r9+04]
;TEW2.exe+6B5472 - 89 81 A4000000        - mov [rcx+000000A4],eax
;TEW2.exe+6B5478 - 41 8B 41 08           - mov eax,[r9+08]
;TEW2.exe+6B547C - 89 81 A8000000        - mov [rcx+000000A8],eax
;TEW2.exe+6B5482 - 41 8B 41 0C           - mov eax,[r9+0C]
;TEW2.exe+6B5486 - 89 81 AC000000        - mov [rcx+000000AC],eax
;TEW2.exe+6B548C - 41 8B 41 10           - mov eax,[r9+10]
;TEW2.exe+6B5490 - 89 81 B0000000        - mov [rcx+000000B0],eax
;TEW2.exe+6B5496 - 41 8B 41 14           - mov eax,[r9+14]
;TEW2.exe+6B549A - 89 81 B4000000        - mov [rcx+000000B4],eax
;TEW2.exe+6B54A0 - 41 8B 41 18           - mov eax,[r9+18]
;TEW2.exe+6B54A4 - 89 81 B8000000        - mov [rcx+000000B8],eax
;TEW2.exe+6B54AA - 41 8B 41 1C           - mov eax,[r9+1C]
;TEW2.exe+6B54AE - 89 81 BC000000        - mov [rcx+000000BC],eax
;TEW2.exe+6B54B4 - 41 8B 41 20           - mov eax,[r9+20]
;TEW2.exe+6B54B8 - 89 81 C0000000        - mov [rcx+000000C0],eax
;TEW2.exe+6B54BE - C3                    - ret 
	mov [g_cameraStructAddress], rcx
	cmp byte ptr [g_cameraEnabled], 1
	jne originalCode
	; issue a 'ret' as the routine shouldn't be executed
	ret
originalCode:
	movss xmm1,dword ptr [rdx]   
	movss xmm0,dword ptr [r8]    
	cvtps2pd xmm0,xmm0              
	cvtps2pd xmm1,xmm1              
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP

cameraWrite1Interceptor PROC
; If the camera is enabled, we simply issue a 'ret' as the whole routine is simply copying the data from stack to the memory location.
;TEW2.exe+6B5560 - F3 0F10 02            - movss xmm0,[rdx]					<< INTERCEPT HERE
;TEW2.exe+6B5564 - 0F5A C0               - cvtps2pd xmm0,xmm0				
;TEW2.exe+6B5567 - F2 0F11 81 88000000   - movsd [rcx+00000088],xmm0			// write X (double!)
;TEW2.exe+6B556F - F3 0F10 4A 04         - movss xmm1,[rdx+04]				<< CONTINUE HERE
;TEW2.exe+6B5574 - 0F5A C9               - cvtps2pd xmm1,xmm1
;TEW2.exe+6B5577 - F2 0F11 89 90000000   - movsd [rcx+00000090],xmm1			// write Y
;TEW2.exe+6B557F - F3 0F10 42 08         - movss xmm0,[rdx+08]
;TEW2.exe+6B5584 - 0F5A C0               - cvtps2pd xmm0,xmm0
;TEW2.exe+6B5587 - F2 0F11 81 98000000   - movsd [rcx+00000098],xmm0			// write Z
;TEW2.exe+6B558F - 41 8B 00              - mov eax,[r8]
;TEW2.exe+6B5592 - 89 81 A0000000        - mov [rcx+000000A0],eax				// matrix
;TEW2.exe+6B5598 - 41 8B 40 04           - mov eax,[r8+04]
;TEW2.exe+6B559C - 89 81 A4000000        - mov [rcx+000000A4],eax
;TEW2.exe+6B55A2 - 41 8B 40 08           - mov eax,[r8+08]
;TEW2.exe+6B55A6 - 89 81 A8000000        - mov [rcx+000000A8],eax
;TEW2.exe+6B55AC - 41 8B 40 0C           - mov eax,[r8+0C]
;TEW2.exe+6B55B0 - 89 81 AC000000        - mov [rcx+000000AC],eax
;TEW2.exe+6B55B6 - 41 8B 40 10           - mov eax,[r8+10]
;TEW2.exe+6B55BA - 89 81 B0000000        - mov [rcx+000000B0],eax
;TEW2.exe+6B55C0 - 41 8B 40 14           - mov eax,[r8+14]
;TEW2.exe+6B55C4 - 89 81 B4000000        - mov [rcx+000000B4],eax
;TEW2.exe+6B55CA - 41 8B 40 18           - mov eax,[r8+18]
;TEW2.exe+6B55CE - 89 81 B8000000        - mov [rcx+000000B8],eax
;TEW2.exe+6B55D4 - 41 8B 40 1C           - mov eax,[r8+1C]
;TEW2.exe+6B55D8 - 89 81 BC000000        - mov [rcx+000000BC],eax
;TEW2.exe+6B55DE - 41 8B 40 20           - mov eax,[r8+20]
;TEW2.exe+6B55E2 - 89 81 C0000000        - mov [rcx+000000C0],eax
;TEW2.exe+6B55E8 - C3                    - ret
	cmp byte ptr [g_cameraEnabled], 1
	jne originalCode
	; issue a 'ret' as the routine shouldn't be executed
	ret
originalCode:
	movss xmm0,dword ptr [rdx]
	cvtps2pd xmm0,xmm0      
	movsd qword ptr [rcx+88h],xmm0
exit:
	jmp qword ptr [_cameraWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite1Interceptor ENDP



timestopReadInterceptor PROC
;1433BA21C - 48 BA 2B151D5CE5A60000 - mov rdx,0000A6E55C1D152B				<< INTERCEPT HERE
;1433BA226 - 49 8B 1E              - mov rbx,[r14]
;1433BA229 - 4C 8B 08              - mov r9,[rax]
;1433BA22C - 45 31 C0              - xor r8d,r8d
;1433BA22F - 48 89 C1              - mov rcx,rax
;1433BA232 - 41 FF 51 18           - call qword ptr [r9+18]					<< CONTINUE HERE
;
; timestop struct pointer is in rdi. The actual read is further down but we can't intercept it there so we do it higher up in the code path
;
; actual read:
;1433BA301 - 8B 47 38              - mov eax,[rdi+38]					// timestop read.
;1433BA304 - FF C8                 - dec eax
;1433BA306 - 83 F8 3F              - cmp eax,3F { 63 }
;1433BA309 - 77 2C                 - ja 1433BA337
;1433BA30B - 48 B9 0180008000000080 - mov rcx,8000000080008001 { -2147450879 }
;1433BA315 - 48 0FA3 C1            - bt rcx,rax
;1433BA319 - 73 1C                 - jae 1433BA337
;1433BA31B - 80 3D EEFB7EFF 00     - cmp byte ptr [142BA9F10],00 { [00000000] }
;1433BA322 - 75 13                 - jne 1433BA337
;1433BA324 - 48 8B 4F 40           - mov rcx,[rdi+40]
;1433BA328 - 8B 01                 - mov eax,[rcx]
;1433BA32A - C1 E8 1F              - shr eax,1F { 31 }
	mov [g_timestopStructAddress], rdi
originalCode:
	mov rdx,0000A6E55C1D152Bh
	mov rbx,[r14]
	mov r9,[rax]
	xor r8d,r8d
	mov rcx,rax
exit:
	jmp qword ptr [_timestopInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
timestopReadInterceptor ENDP


END