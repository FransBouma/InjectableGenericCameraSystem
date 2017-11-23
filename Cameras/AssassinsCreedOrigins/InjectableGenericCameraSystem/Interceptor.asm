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
PUBLIC cameraWrite5Interceptor
PUBLIC fovReadInterceptor
PUBLIC resolutionScaleReadInterceptor
PUBLIC todWriteInterceptor
PUBLIC timestopReadInterceptor

;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_cameraStructAddress: qword
EXTERN g_cameraPhotoModeStructAddress: qword
EXTERN g_fovStructAddress: qword
EXTERN g_resolutionScaleAddress: qword
EXTERN g_todStructAddress: qword
EXTERN g_timestopStructAddress: qword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWrite1InterceptionContinue: qword
EXTERN _cameraWrite2InterceptionContinue: qword
EXTERN _cameraWrite3InterceptionContinue: qword
EXTERN _cameraWrite4InterceptionContinue: qword
EXTERN _cameraWrite5InterceptionContinue: qword
EXTERN _fovReadInterceptionContinue: qword
EXTERN _resolutionScaleReadInterceptionContinue: qword
EXTERN _todWriteInterceptionContinue: qword
EXTERN _timestopReadInterceptionContinue: qword

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
; v1.03
;ACOrigins.exe+13933C7 - 48 8B CF              - mov rcx,rdi							<< INTERCEPT HERE
;ACOrigins.exe+13933CA - 0F28 00               - movaps xmm0,[rax]
;ACOrigins.exe+13933CD - 0F29 87 E0020000      - movaps [rdi+000002E0],xmm0
;ACOrigins.exe+13933D4 - 0F29 47 60            - movaps [rdi+60],xmm0					<< Write coords
;ACOrigins.exe+13933D8 - E8 63CFFFFF           - call ACOrigins.exe+1390340				<< CONTINUE HERE
;ACOrigins.exe+13933DD - 48 8B 8F 40030000     - mov rcx,[rdi+00000340]
;ACOrigins.exe+13933E4 - 4C 8D BF 00030000     - lea r15,[rdi+00000300]
;ACOrigins.exe+13933EB - F3 0F11 74 24 40      - movss [rsp+40],xmm6
;ACOrigins.exe+13933F1 - 4C 8D 45 80           - lea r8,[rbp-80]
;ACOrigins.exe+13933F5 - 48 8D 81 B0010000     - lea rax,[rcx+000001B0]
;ACOrigins.exe+13933FC - 48 8D 91 C8000000     - lea rdx,[rcx+000000C8]
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
;ACOrigins.exe+13932D3 - 4C 8D 67 70           - lea r12,[rdi+70]
;ACOrigins.exe+13932D7 - 48 8D B7 F0020000     - lea rsi,[rdi+000002F0]
;ACOrigins.exe+13932DE - 0F28 D6               - movaps xmm2,xmm6
;ACOrigins.exe+13932E1 - 0F29 06               - movaps [rsi],xmm0
;ACOrigins.exe+13932E4 - 49 8B D4              - mov rdx,r12							<< INTERCEPT HERE
;ACOrigins.exe+13932E7 - 41 0F29 04 24         - movaps [r12],xmm0						<< WRITE quaternion
;ACOrigins.exe+13932EC - 48 8B 8F 40030000     - mov rcx,[rdi+00000340]
;ACOrigins.exe+13932F3 - 48 8B 49 50           - mov rcx,[rcx+50]						<< CONTINUE HERE
;ACOrigins.exe+13932F7 - E8 845A0300           - call ACOrigins.exe+13C8D80
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
;ACOrigins.exe+13938FD - 0F59 EB               - mulps xmm5,xmm3
;ACOrigins.exe+1393900 - 0F58 E2               - addps xmm4,xmm2
;ACOrigins.exe+1393903 - 0F58 E5               - addps xmm4,xmm5						<< INTERCEPT HERE
;ACOrigins.exe+1393906 - 0F29 26               - movaps [rsi],xmm4
;ACOrigins.exe+1393909 - 41 0F29 24 24         - movaps [r12],xmm4						<< Write quaternion.
;ACOrigins.exe+139390E - F3 41 0F10 07         - movss xmm0,[r15]
;ACOrigins.exe+1393913 - F3 0F5F 05 15BCE701   - maxss xmm0,[ACOrigins.exe+320F530]		<< CONTINUE HERE
;ACOrigins.exe+139391B - 48 8B 87 40030000     - mov rax,[rdi+00000340]
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
; v1.03
;ACOrigins.exe+1DB34F0 - 41 0F28 7B E0         - movaps xmm7,[r11-20]
;ACOrigins.exe+1DB34F5 - 0F59 C3               - mulps xmm0,xmm3
;ACOrigins.exe+1DB34F8 - 0F58 E0               - addps xmm4,xmm0
;ACOrigins.exe+1DB34FB - 0F59 E6               - mulps xmm4,xmm6
;ACOrigins.exe+1DB34FE - 41 0F28 73 F0         - movaps xmm6,[r11-10]
;ACOrigins.exe+1DB3503 - 0F29 A7 30050000      - movaps [rdi+00000530],xmm4
;ACOrigins.exe+1DB350A - 44 0F29 A7 70040000   - movaps [rdi+00000470],xmm12		<<< INTERCEPT HERE <<< coords
;ACOrigins.exe+1DB3512 - 45 0F28 63 90         - movaps xmm12,[r11-70]
;ACOrigins.exe+1DB3517 - 49 8B E3              - mov rsp,r11
;ACOrigins.exe+1DB351A - 41 5F                 - pop r15							<<< CONTINUE HERE
;ACOrigins.exe+1DB351C - 41 5E                 - pop r14
;ACOrigins.exe+1DB351E - 5F                    - pop rdi
	mov [g_cameraPhotoModeStructAddress], rdi
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movaps xmmword ptr [rdi+00000470h],xmm12
exit:
	movaps xmm12, xmmword ptr [r11-70h]
	mov rsp,r11
	jmp qword ptr [_cameraWrite3InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite3Interceptor ENDP


cameraWrite4Interceptor PROC
; photomode camera
; v1.03
;ACOrigins.exe+1DB3385 - 0F59 E0               - mulps xmm4,xmm0
;ACOrigins.exe+1DB3388 - 0F59 E5               - mulps xmm4,xmm5
;ACOrigins.exe+1DB338B - 0F28 2D EE9A4101      - movaps xmm5,[ACOrigins.exe+31CCE80]
;ACOrigins.exe+1DB3392 - 0F28 DC               - movaps xmm3,xmm4					<< INTERCEPT HERE
;ACOrigins.exe+1DB3395 - 0F29 A7 80040000      - movaps [rdi+00000480],xmm4			<< Quaternion write
;ACOrigins.exe+1DB339C - 0F59 DD               - mulps xmm3,xmm5
;ACOrigins.exe+1DB339F - 0F28 CB               - movaps xmm1,xmm3					
;ACOrigins.exe+1DB33A2 - 0F28 C3               - movaps xmm0,xmm3					<<< CONTINUE HERE
;ACOrigins.exe+1DB33A5 - 0FC6 CB AA            - shufps xmm1,xmm3,-56 { 170 }
;ACOrigins.exe+1DB33A9 - 0FC6 C3 55            - shufps xmm0,xmm3,55 { 85 }
;ACOrigins.exe+1DB33AD - F3 0F58 C1            - addss xmm0,xmm1
	movaps xmm3,xmm4			
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movaps xmmword ptr [rdi+00000480h], xmm4	
exit:
	mulps xmm3,xmm5
	movaps xmm1,xmm3
	jmp qword ptr [_cameraWrite4InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite4Interceptor ENDP


cameraWrite5Interceptor PROC
; cutscene camera coords
; v1.03
;ACOrigins.exe+13D6640 - F3 0F10 98 B00B0000   - movss xmm3,[rax+00000BB0]
;ACOrigins.exe+13D6648 - 0F14 D8               - unpcklps xmm3,xmm0
;ACOrigins.exe+13D664B - 0F14 D1               - unpcklps xmm2,xmm1
;ACOrigins.exe+13D664E - 0F14 DA               - unpcklps xmm3,xmm2
;ACOrigins.exe+13D6651 - 0F29 1F               - movaps [rdi],xmm3				<< INTERCEPT HERE << WRITE coords
;ACOrigins.exe+13D6654 - 48 8B 43 38           - mov rax,[rbx+38]
;ACOrigins.exe+13D6658 - 0F28 80 A00B0000      - movaps xmm0,[rax+00000BA0]
;ACOrigins.exe+13D665F - 0F29 45 00            - movaps [rbp+00],xmm0			
;ACOrigins.exe+13D6663 - 48 8B 43 38           - mov rax,[rbx+38]				<< CONTINUE HERE
;ACOrigins.exe+13D6667 - 8B 88 BC0B0000        - mov ecx,[rax+00000BBC]
;ACOrigins.exe+13D666D - 48 8B 44 24 70        - mov rax,[rsp+70]
;ACOrigins.exe+13D6672 - 89 08                 - mov [rax],ecx
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movaps xmmword ptr [rdi],xmm3			
exit:
	mov rax,[rbx+38h]
	movaps xmm0, xmmword ptr [rax+00000BA0h]
	movaps xmmword ptr [rbp+00h],xmm0
	jmp qword ptr [_cameraWrite5InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite5Interceptor ENDP


fovReadInterceptor PROC
;ACOrigins.exe+80717A - 0F29 85 10030000			- movaps [rbp+00000310],xmm0
;ACOrigins.exe+807181 - 0F29 8D 00030000			- movaps [rbp+00000300],xmm1
;ACOrigins.exe+807188 - E8 4358BCFF					- call ACOrigins.exe+3CC9D0
;ACOrigins.exe+80718D - F3 41 0F10 94 24 24010000	- movss xmm2,[r12+00000124]				<< INTERCEPT HERE
;ACOrigins.exe+807197 - 45 0F57 D2					- xorps xmm10,xmm10
;ACOrigins.exe+80719B - 41 0F2F D2					- comiss xmm2,xmm10
;ACOrigins.exe+80719F - F3 41 0F10 8F 64020000		- movss xmm1,[r15+00000264]				<< FOV READ
;ACOrigins.exe+8071A8 - F3 0F10 05 A45B9C02			- movss xmm0,[ACOrigins.exe+31CCD54]	<< CONTINUE HERE
;ACOrigins.exe+8071B0 - 76 41						- jna ACOrigins.exe+8071F3
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
; v1.05
;000000014072A399 | 89 8D 68 07 00 00                | mov dword ptr ss:[rbp+768],ecx   
;000000014072A39F | F3 0F 11 B5 64 02 00 00          | movss dword ptr ss:[rbp+264],xmm6
;000000014072A3A7 | 89 85 6C 07 00 00                | mov dword ptr ss:[rbp+76C],eax   
;000000014072A3AD | 41 8B 86 A8 00 00 00             | mov eax,dword ptr ds:[r14+A8]    << INTERCEPT HERE<< Read of resolution scale (float. 1.0 is 100%) Max is 4!
;000000014072A3B4 | 48 8B CD                         | mov rcx,rbp                      
;000000014072A3B7 | 89 85 30 07 00 00                | mov dword ptr ss:[rbp+730],eax   
;000000014072A3BD | E8 BE 43 11 00                   | call acorigins_dump.14083E780    << CONTINUE HERE
;000000014072A3C2 | 48 8D 8B D0 00 00 00             | lea rcx,qword ptr ds:[rbx+D0]    
;000000014072A3C9 | 48 8D 95 60 04 00 00             | lea rdx,qword ptr ss:[rbp+460]   
;000000014072A3D0 | E8 6B A6 FD FF                   | call acorigins_dump.140704A40    
	mov [g_resolutionScaleAddress], r14
originalCode:
	mov eax,[r14+000000A8h]
	mov rcx,rbp
	mov [rbp+00000730h],eax
exit:
	jmp qword ptr [_resolutionScaleReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
resolutionScaleReadInterceptor ENDP


todWriteInterceptor PROC
;ACOrigins.exe+1185730 - F3 0F5E C7            - divss xmm0,xmm7
;ACOrigins.exe+1185734 - 0F28 7C 24 30         - movaps xmm7,[rsp+30]
;ACOrigins.exe+1185739 - F3 0F59 C6            - mulss xmm0,xmm6
;ACOrigins.exe+118573D - F3 41 0F58 C0         - addss xmm0,xmm8
;ACOrigins.exe+1185742 - 44 0F28 44 24 20      - movaps xmm8,[rsp+20]
;ACOrigins.exe+1185748 - F3 0F11 00            - movss [rax],xmm0					<<< INTERCEPT HERE <<< Write of ToD.
;ACOrigins.exe+118574C - 48 8B 83 40020000     - mov rax,[rbx+00000240]
;ACOrigins.exe+1185753 - F3 0F10 08            - movss xmm1,[rax]
;ACOrigins.exe+1185757 - 0F2F CA               - comiss xmm1,xmm2					<<< CONTINUE HERE
	mov [g_todStructAddress], rax
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rax],xmm0		
exit:
	mov rax, [rbx+00000240h]
	movss xmm1, dword ptr [rax]
	jmp qword ptr [_todWriteInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
todWriteInterceptor ENDP


timestopReadInterceptor PROC
;ACOrigins.exe+11E2800 - 48 8D 8D 78010000     - lea rcx,[rbp+00000178]
;ACOrigins.exe+11E2807 - 49 8B D5              - mov rdx,r13
;ACOrigins.exe+11E280A - E8 B123FAFF           - call ACOrigins.exe+1184BC0
;ACOrigins.exe+11E280F - 48 8B 4D 78           - mov rcx,[rbp+78]					
;ACOrigins.exe+11E2813 - 44 8B 85 C4610000     - mov r8d,[rbp+000061C4]				<<< INTERCEPT HERE
;ACOrigins.exe+11E281A - 48 8B 95 BC610000     - mov rdx,[rbp+000061BC]
;ACOrigins.exe+11E2821 - 83 B9 58140000 00     - cmp dword ptr [rcx+00001458],00 	<<< READ timestop address.
;ACOrigins.exe+11E2828 - 41 0F9F C1            - setg r9l							<<< CONTINUE HERE
;ACOrigins.exe+11E282C - 41 C1 E8 11           - shr r8d,11 { 17 }
;ACOrigins.exe+11E2830 - E8 DBC8F7FF           - call ACOrigins.exe+115F110
;ACOrigins.exe+11E2835 - 48 8B 4D 78           - mov rcx,[rbp+78]
;ACOrigins.exe+11E2839 - 44 8B 85 D0610000     - mov r8d,[rbp+000061D0]
;ACOrigins.exe+11E2840 - 48 8B 95 C8610000     - mov rdx,[rbp+000061C8]
	mov [g_timestopStructAddress], rcx
originalCode:
	mov r8d,[rbp+000061C4h]
	mov rdx,[rbp+000061BCh]
	cmp dword ptr [rcx+00001458h],00 
exit:
	jmp qword ptr [_timestopReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
timestopReadInterceptor ENDP



END