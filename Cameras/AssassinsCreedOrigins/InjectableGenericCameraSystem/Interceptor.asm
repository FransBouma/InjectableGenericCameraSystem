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
PUBLIC fovReadInterceptor
PUBLIC resolutionScaleReadInterceptor

;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_cameraStructAddress: qword
EXTERN g_cameraPhotoModeStructAddress: qword
EXTERN g_fovStructAddress: qword
EXTERN g_resolutionScaleAddress: qword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWrite1InterceptionContinue: qword
EXTERN _cameraWrite2InterceptionContinue: qword
EXTERN _cameraWrite3InterceptionContinue: qword
EXTERN _cameraWrite4InterceptionContinue: qword
EXTERN _fovReadInterceptionContinue: qword
EXTERN _resolutionScaleReadInterceptionContinue: qword

.data

;---------------------------------------------------------------
; Scratch pad
;
; // Generic camera copy function which can be used but gives shimering in photomode, lighting issues in normal gameplay. We'll go for the alternative. 
; // the alternative is 2 coords+quaternions for normal camera, and 1 coords+quaternion for photomode.
;ACOrigins.exe+83932F - CC                    - int 3 
;ACOrigins.exe+839330 - 0F28 01               - movaps xmm0,[rcx]						<< COPY Current matrix/coords to backup 
;ACOrigins.exe+839333 - 0F29 81 E0060000      - movaps [rcx+000006E0],xmm0
;ACOrigins.exe+83933A - 0F28 49 10            - movaps xmm1,[rcx+10]
;ACOrigins.exe+83933E - 0F29 89 F0060000      - movaps [rcx+000006F0],xmm1
;ACOrigins.exe+839345 - 0F28 41 20            - movaps xmm0,[rcx+20]
;ACOrigins.exe+839349 - 0F29 81 00070000      - movaps [rcx+00000700],xmm0
;ACOrigins.exe+839350 - 0F28 49 30            - movaps xmm1,[rcx+30]
;ACOrigins.exe+839354 - 0F29 89 10070000      - movaps [rcx+00000710],xmm1
;ACOrigins.exe+83935B - 0F28 02               - movaps xmm0,[rdx]
;ACOrigins.exe+83935E - 0F29 01               - movaps [rcx],xmm0						<< MATRIX
;ACOrigins.exe+839361 - 0F28 4A 10            - movaps xmm1,[rdx+10]
;ACOrigins.exe+839365 - 0F29 49 10            - movaps [rcx+10],xmm1
;ACOrigins.exe+839369 - 0F28 42 20            - movaps xmm0,[rdx+20]
;ACOrigins.exe+83936D - 0F29 41 20            - movaps [rcx+20],xmm0
;ACOrigins.exe+839371 - 0F28 4A 30            - movaps xmm1,[rdx+30]
;ACOrigins.exe+839375 - 0F29 49 30            - movaps [rcx+30],xmm1						<< COORDS
;ACOrigins.exe+839379 - C3                    - ret 
;

.code


cameraStructInterceptor PROC
; camera address interceptor is also a write blocker. 
;ACOrigins.exe+138A2A7 - 48 8B CF              - mov rcx,rdi							<< INTERCEPT HERE
;ACOrigins.exe+138A2AA - 0F28 00               - movaps xmm0,[rax]
;ACOrigins.exe+138A2AD - 0F29 87 E0020000      - movaps [rdi+000002E0],xmm0
;ACOrigins.exe+138A2B4 - 0F29 47 60            - movaps [rdi+60],xmm0					<< Write coords
;ACOrigins.exe+138A2B8 - E8 83CFFFFF           - call ACOrigins.exe+1387240				<< CONTINUE HERE
;ACOrigins.exe+138A2BD - 48 8B 8F 40030000     - mov rcx,[rdi+00000340]
;ACOrigins.exe+138A2C4 - 4C 8D BF 00030000     - lea r15,[rdi+00000300]
;ACOrigins.exe+138A2CB - F3 0F11 74 24 40      - movss [rsp+40],xmm6
;ACOrigins.exe+138A2D1 - 4C 8D 45 80           - lea r8,[rbp-80]
;ACOrigins.exe+138A2D5 - 48 8D 81 B0010000     - lea rax,[rcx+000001B0]
	mov [g_cameraStructAddress], rdi
	mov rcx,rdi				
	movaps xmm0, xmmword ptr [rax]
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movaps xmmword ptr [rdi+000002E0h],xmm0
	movaps xmmword ptr [rdi+60h],xmm0		
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP


cameraWrite1Interceptor PROC
;ACOrigins.exe+138A1B3 - 4C 8D 67 70           - lea r12,[rdi+70]
;ACOrigins.exe+138A1B7 - 48 8D B7 F0020000     - lea rsi,[rdi+000002F0]
;ACOrigins.exe+138A1BE - 0F28 D6               - movaps xmm2,xmm6
;ACOrigins.exe+138A1C1 - 0F29 06               - movaps [rsi],xmm0
;ACOrigins.exe+138A1C4 - 49 8B D4              - mov rdx,r12							<< INTERCEPT HERE
;ACOrigins.exe+138A1C7 - 41 0F29 04 24         - movaps [r12],xmm0						<< WRITE quaternion
;ACOrigins.exe+138A1CC - 48 8B 8F 40030000     - mov rcx,[rdi+00000340]
;ACOrigins.exe+138A1D3 - 48 8B 49 50           - mov rcx,[rcx+50]						<< CONTINUE HERE
;ACOrigins.exe+138A1D7 - E8 A48A0300           - call ACOrigins.exe+13C2C80				
	mov rdx,r12			
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movaps xmmword ptr [r12],xmm0		
exit:
	mov rcx,[rdi+00000340h]
	jmp qword ptr [_cameraWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite1Interceptor ENDP


cameraWrite2Interceptor PROC
;ACOrigins.exe+138A7D6 - 0F59 D0               - mulps xmm2,xmm0
;ACOrigins.exe+138A7D9 - 0FC6 ED B1            - shufps xmm5,xmm5,-4F { 177 }
;ACOrigins.exe+138A7DD - 0F59 EB               - mulps xmm5,xmm3
;ACOrigins.exe+138A7E0 - 0F58 E2               - addps xmm4,xmm2
;ACOrigins.exe+138A7E3 - 0F58 E5               - addps xmm4,xmm5						<< INTERCEPT HERE
;ACOrigins.exe+138A7E6 - 0F29 26               - movaps [rsi],xmm4
;ACOrigins.exe+138A7E9 - 41 0F29 24 24         - movaps [r12],xmm4						<< Write quaternion.
;ACOrigins.exe+138A7EE - F3 41 0F10 07         - movss xmm0,[r15]						
;ACOrigins.exe+138A7F3 - F3 0F5F 05 FDC8E501   - maxss xmm0,[ACOrigins.exe+31E70F8] { [0.00] }	<< CONTINUE HERE
	addps xmm4,xmm5	
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movaps xmmword ptr [rsi],xmm4
	movaps xmmword ptr [r12],xmm4	
exit:
	movss xmm0, dword ptr [r15]
	jmp qword ptr [_cameraWrite2InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite2Interceptor ENDP


cameraWrite3Interceptor PROC
; photomode camera
;ACOrigins.exe+1D9683D - 0F59 C3               - mulps xmm0,xmm3
;ACOrigins.exe+1D96840 - 0F58 E0               - addps xmm4,xmm0
;ACOrigins.exe+1D96843 - 0F59 E6               - mulps xmm4,xmm6
;ACOrigins.exe+1D96846 - 41 0F28 73 F0         - movaps xmm6,[r11-10]
;ACOrigins.exe+1D9684B - 0F29 A7 40050000      - movaps [rdi+00000540],xmm4			
;ACOrigins.exe+1D96852 - 44 0F29 8F 80040000   - movaps [rdi+00000480],xmm9			<<< INTERCEPT HERE <<< coords
;ACOrigins.exe+1D9685A - 45 0F28 4B C0         - movaps xmm9,[r11-40]
;ACOrigins.exe+1D9685F - 49 8B E3              - mov rsp,r11						
;ACOrigins.exe+1D96862 - 41 5F                 - pop r15							<<< CONTINUE HERE
	mov [g_cameraPhotoModeStructAddress], rdi
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movaps xmmword ptr [rdi+00000480h],xmm9
exit:
	movaps xmm9, xmmword ptr [r11-40h]
	mov rsp,r11
	jmp qword ptr [_cameraWrite3InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite3Interceptor ENDP


cameraWrite4Interceptor PROC
; photomode camera
;ACOrigins.exe+1D966DD - 0F28 35 BCA24101      - movaps xmm6,[ACOrigins.exe+31B09A0]
;ACOrigins.exe+1D966E4 - 0F28 DC               - movaps xmm3,xmm4					<< INTERCEPT HERE
;ACOrigins.exe+1D966E7 - 0F29 A7 90040000      - movaps [rdi+00000490],xmm4			<<< Quaternion write
;ACOrigins.exe+1D966EE - 0F59 DD               - mulps xmm3,xmm5
;ACOrigins.exe+1D966F1 - 0F28 CB               - movaps xmm1,xmm3
;ACOrigins.exe+1D966F4 - 0F28 C3               - movaps xmm0,xmm3					<<< CONTINUE HERE
;ACOrigins.exe+1D966F7 - 0FC6 CB AA            - shufps xmm1,xmm3,-56 { 170 }
;ACOrigins.exe+1D966FB - 0FC6 C3 55            - shufps xmm0,xmm3,55 { 85 }
;ACOrigins.exe+1D966FF - F3 0F58 C1            - addss xmm0,xmm1
;ACOrigins.exe+1D96703 - 0F28 CC               - movaps xmm1,xmm4
;ACOrigins.exe+1D96706 - 0FC6 CC D2            - shufps xmm1,xmm4,-2E { 210 }
	movaps xmm3,xmm4			
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movaps xmmword ptr [rdi+00000490h], xmm4	
exit:
	mulps xmm3,xmm5
	movaps xmm1,xmm3
	jmp qword ptr [_cameraWrite4InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite4Interceptor ENDP


fovReadInterceptor PROC
;ACOrigins.exe+80336A - 0F29 85 10030000			- movaps [rbp+00000310],xmm0
;ACOrigins.exe+803371 - 0F29 8D 00030000			- movaps [rbp+00000300],xmm1
;ACOrigins.exe+803378 - E8 3359BCFF					- call ACOrigins.exe+3C8CB0
;ACOrigins.exe+80337D - F3 41 0F10 94 24 24010000	- movss xmm2,[r12+00000124]				<< INTERCEPT HERE
;ACOrigins.exe+803387 - 45 0F57 D2					- xorps xmm10,xmm10
;ACOrigins.exe+80338B - 41 0F2F D2					- comiss xmm2,xmm10
;ACOrigins.exe+80338F - F3 41 0F10 8F 64020000		- movss xmm1,[r15+00000264]				<< FOV READ
;ACOrigins.exe+803398 - F3 0F10 05 94119A02			- movss xmm0,[ACOrigins.exe+31A4534]	<< CONTINUE HERE
;ACOrigins.exe+8033A0 - 76 41						- jna ACOrigins.exe+8033E3
	mov [g_fovStructAddress], r15
originalCode:
	movss xmm2, dword ptr [r12+00000124h]
	xorps xmm10,xmm10
	comiss xmm2,xmm10
	movss xmm1, dword ptr [r15+00000264h]
exit:
	jmp qword ptr [_fovReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
fovReadInterceptor ENDP


resolutionScaleReadInterceptor PROC
;ACOrigins.exe+72A6C5 - 41 8B 86 A8000000     - mov eax,[r14+000000A8]					<< INTERCEPT HERE<< Read of resolution scale (float. 1.0 is 100%) Max is 4!
;ACOrigins.exe+72A6CC - 49 8B CF              - mov rcx,r15
;ACOrigins.exe+72A6CF - 41 89 87 20070000     - mov [r15+00000720],eax
;ACOrigins.exe+72A6D6 - E8 454B1100           - call ACOrigins.exe+83F220				<< CONTINUE HERE
;ACOrigins.exe+72A6DB - 48 8D 8B D0000000     - lea rcx,[rbx+000000D0]
;ACOrigins.exe+72A6E2 - 49 8D 97 50040000     - lea rdx,[r15+00000450]
;ACOrigins.exe+72A6E9 - E8 F2ABFDFF           - call ACOrigins.exe+7052E0
	mov [g_resolutionScaleAddress], r14
originalCode:
	mov eax,[r14+000000A8h]
	mov rcx,r15
	mov [r15+00000720h],eax
exit:
	jmp qword ptr [_resolutionScaleReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
resolutionScaleReadInterceptor ENDP


END