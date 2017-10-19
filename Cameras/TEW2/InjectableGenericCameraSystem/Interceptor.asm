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

.data

;---------------------------------------------------------------
; Scratch pad
;
; Timestop read. This is a byte (1==stop, 0==continue). 
; We simply need to read the RIP relative value using AOB scanning to get the address. 
;TEW2.exe+444830 - 83 3D 41B25702 00		- cmp dword ptr [TEW2.exe+29BFA78],00		<< Timestop read
;TEW2.exe+444837 - 75 0A					- jne TEW2.exe+444843
;TEW2.exe+444839 - C7 81 30010000 00000000	- mov [rcx+00000130],00000000 { 0 }
;TEW2.exe+444843 - 8B 92 38020000			- mov edx,[rdx+00000238]
;TEW2.exe+444849 - 03 91 30010000			- add edx,[rcx+00000130]
;TEW2.exe+44484F - 48 81 C1 28010000		- add rcx,00000128 { 296 }
;TEW2.exe+444856 - E9 05000000				- jmp TEW2.exe+444860
;

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
;
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


cameraWrite2Interceptor PROC
;TEW2.exe+6B172C - F2 0F11 46 78         - movsd [rsi+78],xmm0
;TEW2.exe+6B1731 - 0F5A C2               - cvtps2pd xmm0,xmm2
;TEW2.exe+6B1734 - F2 0F11 86 88000000   - movsd [rsi+00000088],xmm0		<< INTERCEPT HERE		// coords
;TEW2.exe+6B173C - 0F11 8E 90000000      - movups [rsi+00000090],xmm1
;TEW2.exe+6B1743 - 0F10 4C 24 50         - movups xmm1,[rsp+50]
;TEW2.exe+6B1748 - 0F11 8E A0000000      - movups [rsi+000000A0],xmm1		// matrix
;TEW2.exe+6B174F - F3 0F10 4C 24 60      - movss xmm1,[rsp+60]
;TEW2.exe+6B1755 - F3 0F11 8E B0000000   - movss [rsi+000000B0],xmm1			// matrix
;TEW2.exe+6B175D - 48 8B 9C 24 98000000  - mov rbx,[rsp+00000098]			<< CONTINUE HERE
;TEW2.exe+6B1765 - 48 81 C4 80000000     - add rsp,00000080 { 128 }
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movsd qword ptr [rsi+00000088h],xmm0   
	movups xmmword ptr [rsi+00000090h],xmm1
	movups xmm1,xmmword ptr [rsp+50h]
	movups xmmword ptr [rsi+000000a0h],xmm1
	movss xmm1,dword ptr [rsp+60h]   
	movss dword ptr [rsi+000000b0h],xmm1   
exit:
	jmp qword ptr [_cameraWrite2InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite2Interceptor ENDP

END