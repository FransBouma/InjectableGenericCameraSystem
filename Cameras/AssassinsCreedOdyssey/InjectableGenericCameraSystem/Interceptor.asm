;////////////////////////////////////////////////////////////////////////////////////////////////////////
;// Part of Injectable Generic Camera System
;// Copyright(c) 2019, Frans Bouma
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
PUBLIC resolutionScaleReadInterceptor
PUBLIC todWriteInterceptor
PUBLIC timestopReadInterceptor
PUBLIC fogReadInterceptor

;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_cameraStructAddress: qword
EXTERN g_resolutionScaleAddress: qword
EXTERN g_todStructAddress: qword
EXTERN g_timestopStructAddress: qword
EXTERN g_fogStructAddress: qword

;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWrite1InterceptionContinue: qword
EXTERN _cameraWrite2InterceptionContinue: qword
EXTERN _fovReadInterceptionContinue: qword
EXTERN _resolutionScaleReadInterceptionContinue: qword
EXTERN _todWriteInterceptionContinue: qword
EXTERN _timestopReadInterceptionContinue: qword
EXTERN _fogReadInterceptionContinue: qword

.data


.code


cameraStructInterceptor PROC
; camera address interceptor is also a write blocker. 
;ACOdyssey.exe+FC38806 - FF 90 B8020000        - call qword ptr [rax+000002B8]
;ACOdyssey.exe+FC3880C - 0F28 46 60            - movaps xmm0,[rsi+60]
;ACOdyssey.exe+FC38810 - 4C 8D 66 60           - lea r12,[rsi+60]
;ACOdyssey.exe+FC38814 - 0F28 4E 70            - movaps xmm1,[rsi+70]
;ACOdyssey.exe+FC38818 - 48 8D BE A0000000     - lea rdi,[rsi+000000A0]
;ACOdyssey.exe+FC3881F - 8B 86 80000000        - mov eax,[rsi+00000080]					
;ACOdyssey.exe+FC38825 - 66 0F7F 86 90000000   - movdqa [rsi+00000090],xmm0				<< INTERCEPT HERE  << WRITE COORDS.
;ACOdyssey.exe+FC3882D - 66 0F7F 0F            - movdqa [rdi],xmm1						<< WRITE Quaterion
;ACOdyssey.exe+FC38831 - 89 86 B0000000        - mov [rsi+000000B0],eax					<< WRITE FOV
;ACOdyssey.exe+FC38837 - E8 E4A6D4F0           - call ACOdyssey.exe+982F20				<< CONTINUE HERE
;ACOdyssey.exe+FC3883C - 48 89 C1              - mov rcx,rax
;ACOdyssey.exe+FC3883F - E8 1CA7D4F0           - call ACOdyssey.exe+982F60
;ACOdyssey.exe+FC38844 - 84 C0                 - test al,al
;ACOdyssey.exe+FC38846 - 0F84 88050000         - je ACOdyssey.exe+FC38DD4
	mov [g_cameraStructAddress], rsi
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movdqa xmmword ptr [rsi+00000090h],xmm0	
	movdqa [rdi],xmm1
	mov [rsi+000000B0h],eax		
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP


cameraWrite1Interceptor PROC
;ACOdyssey.exe+14AF060 - 48 83 EC 28           - sub rsp,28 { 40 }
;ACOdyssey.exe+14AF064 - 80 79 34 00           - cmp byte ptr [rcx+34],00 { 0 }
;ACOdyssey.exe+14AF068 - 0F84 DD000000         - je ACOdyssey.exe+14AF14B
;ACOdyssey.exe+14AF06E - 0F28 02               - movaps xmm0,[rdx]
;ACOdyssey.exe+14AF071 - 0F58 41 10            - addps xmm0,[rcx+10]
;ACOdyssey.exe+14AF075 - 0F29 74 24 10         - movaps [rsp+10],xmm6
;ACOdyssey.exe+14AF07A - 0F29 3C 24            - movaps [rsp],xmm7
;ACOdyssey.exe+14AF07E - 0F29 02               - movaps [rdx],xmm0						<< INTERCEPT HERE << WRITE COORDS
;ACOdyssey.exe+14AF081 - 0F28 71 20            - movaps xmm6,[rcx+20]
;ACOdyssey.exe+14AF085 - 41 0F28 10            - movaps xmm2,[r8]
;ACOdyssey.exe+14AF089 - 41 0F28 38            - movaps xmm7,[r8]
;ACOdyssey.exe+14AF08D - 0F28 E2               - movaps xmm4,xmm2						<< CONTINUE HERE
;ACOdyssey.exe+14AF090 - 41 0F28 28            - movaps xmm5,[r8]
;ACOdyssey.exe+14AF094 - 0FC6 FF 00            - shufps xmm7,xmm700 { 0 }
;ACOdyssey.exe+14AF098 - 66 0F6F C7            - movdqa xmm0,xmm7
;ACOdyssey.exe+14AF09C - 0FC6 E2 AA            - shufps xmm4,xmm2-56 { 170 }
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movaps xmmword ptr [rdx],xmm0	
exit:
	movaps xmm6, xmmword ptr [rcx+20h]
	movaps xmm2,[r8]
	movaps xmm7,[r8]
	jmp qword ptr [_cameraWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite1Interceptor ENDP


cameraWrite2Interceptor PROC
;ACOdyssey.exe+14AF117 - 0F59 E8               - mulps xmm5,xmm0
;ACOdyssey.exe+14AF11A - 0F58 FA               - addps xmm7,xmm2
;ACOdyssey.exe+14AF11D - 0FC6 F6 B1            - shufps xmm6,xmm6-4F { 177 }
;ACOdyssey.exe+14AF121 - 0F59 E6               - mulps xmm4,xmm6
;ACOdyssey.exe+14AF124 - 0F28 74 24 10         - movaps xmm6,[rsp+10]
;ACOdyssey.exe+14AF129 - 0F58 FD               - addps xmm7,xmm5
;ACOdyssey.exe+14AF12C - 0F58 FC               - addps xmm7,xmm4
;ACOdyssey.exe+14AF12F - 41 0F29 38            - movaps [r8],xmm7						<< INTERCEPT HERE << WRITE Quaternion
;ACOdyssey.exe+14AF133 - F3 0F10 41 30         - movss xmm0,[rcx+30]
;ACOdyssey.exe+14AF138 - F3 41 0F58 01         - addss xmm0,dword ptr [r9]
;ACOdyssey.exe+14AF13D - 0F28 3C 24            - movaps xmm7,[rsp]
;ACOdyssey.exe+14AF141 - F3 41 0F11 01         - movss [r9],xmm0						<< WRITE FOV
;ACOdyssey.exe+14AF146 - 48 83 C4 28           - add rsp,28 { 40 }						<< CONTINUE HERE
;ACOdyssey.exe+14AF14A - C3                    - ret 
	cmp byte ptr [g_cameraEnabled], 1
	jne originalCode
noWrites:
	movss xmm0, dword ptr [rcx+30h]
	addss xmm0, dword ptr [r9]
	movaps xmm7, xmmword ptr [rsp]
	jmp exit
originalCode:
	movaps xmmword ptr [r8],xmm7			
	movss xmm0, dword ptr [rcx+30h]
	addss xmm0, dword ptr [r9]
	movaps xmm7, xmmword ptr [rsp]
	movss dword ptr [r9],xmm0			
exit:
	jmp qword ptr [_cameraWrite2InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite2Interceptor ENDP


resolutionScaleReadInterceptor PROC
; The read is done with an offset from address in rsi: 
;ACOdyssey.exe+B207A3C - EB 0C                 - jmp ACOdyssey.exe+B207A4A
;ACOdyssey.exe+B207A3E - 8B 82 50020000        - mov eax,[rdx+00000250]
;ACOdyssey.exe+B207A44 - 89 86 80010000        - mov [rsi+00000180],eax
;ACOdyssey.exe+B207A4A - 8B 86 A4000000        - mov eax,[rsi+000000A4]				<< READ resolution scale. 
;ACOdyssey.exe+B207A50 - 48 89 E9              - mov rcx,rbp
;ACOdyssey.exe+B207A53 - 89 85 30070000        - mov [rbp+00000730],eax
;ACOdyssey.exe+B207A59 - E8 C2AF9FF5           - call ACOdyssey.exe+C02A20
;
; However this isn't usable as a lot of relative jumps land all over the place and there's a call with a RIP address.
;
; So we use a piece of code higher up in the function, rsi has the same value there. 
;
;ACOdyssey.exe+B207967 - 48 89 68 E8           - mov [rax-18],rbp
;ACOdyssey.exe+B20796B - 48 89 D5              - mov rbp,rdx
;ACOdyssey.exe+B20796E - 48 89 78 E0           - mov [rax-20],rdi
;ACOdyssey.exe+B207972 - 31 FF                 - xor edi,edi
;ACOdyssey.exe+B207974 - 4C 89 70 C8           - mov [rax-38],r14					
;ACOdyssey.exe+B207978 - 48 89 11              - mov [rcx],rdx
;ACOdyssey.exe+B20797B - 4C 89 78 C0           - mov [rax-40],r15
;ACOdyssey.exe+B20797F - 48 8B 82 50040000     - mov rax,[rdx+00000450]
;ACOdyssey.exe+B207986 - 48 8B 70 60           - mov rsi,[rax+60]					<< INTERCEPT HERE
;ACOdyssey.exe+B20798A - 48 8B 82 48020000     - mov rax,[rdx+00000248]
;ACOdyssey.exe+B207991 - 48 89 B4 24 B8000000  - mov [rsp+000000B8],rsi
;ACOdyssey.exe+B207999 - 4C 8B 70 08           - mov r14,[rax+08]
;ACOdyssey.exe+B20799D - 49 C1 E6 20           - shl r14,20 { 32 }					<< CONTINUE HERE
;ACOdyssey.exe+B2079A1 - 49 C1 FE 3F           - sar r14,3F { 63 }
;ACOdyssey.exe+B2079A5 - 4C 23 30              - and r14,[rax]
originalCode:
	mov rsi,[rax+60h]
	mov rax,[rdx+00000248h]
	mov [rsp+000000B8h],rsi
	mov r14,[rax+08h]
	mov [g_resolutionScaleAddress], rsi
exit:
	jmp qword ptr [_resolutionScaleReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
resolutionScaleReadInterceptor ENDP


todWriteInterceptor PROC
;ACOdyssey.exe+10434F34 - F3 0F59 C1            - mulss xmm0,xmm1
;ACOdyssey.exe+10434F38 - F3 0F58 00            - addss xmm0,dword ptr [rax]			<< INTERCEPT HERE
;ACOdyssey.exe+10434F3C - F3 0F11 00            - movss [rax],xmm0						<< WRITE Of ToD.
;ACOdyssey.exe+10434F40 - 48 8B 81 60020000     - mov rax,[rcx+00000260]
;ACOdyssey.exe+10434F47 - F3 0F10 08            - movss xmm1,[rax]						<< CONTINUE HERE
;ACOdyssey.exe+10434F4B - 0F2F CA               - comiss xmm1,xmm2
;ACOdyssey.exe+10434F4E - 72 13                 - jb ACOdyssey.exe+10434F63
;ACOdyssey.exe+10434F50 - F3 0F5C CA            - subss xmm1,xmm2
;ACOdyssey.exe+10434F54 - 0F57 C0               - xorps xmm0,xmm0
;ACOdyssey.exe+10434F57 - F3 0F5F C8            - maxss xmm1,xmm0
;ACOdyssey.exe+10434F5B - F3 0F5D CA            - minss xmm1,xmm2
;ACOdyssey.exe+10434F5F - F3 0F11 08            - movss [rax],xmm1
	addss xmm0, dword ptr [rax]	
	mov [g_todStructAddress], rax
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rax],xmm0				
exit:
	mov rax,[rcx+00000260h]
	jmp qword ptr [_todWriteInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
todWriteInterceptor ENDP


timestopReadInterceptor PROC
;ACOdyssey.exe+106B4BBA - 48 89 54 24 28        - mov [rsp+28],rdx				>>> INTERCEPT HERE
;ACOdyssey.exe+106B4BBF - 48 8B 57 18           - mov rdx,[rdi+18]
;ACOdyssey.exe+106B4BC3 - 83 E1 10              - and ecx,10 { 16 }				>>> RDX contains pWorld, needed for pause.
;ACOdyssey.exe+106B4BC6 - 4C 31 C1              - xor rcx,r8
;ACOdyssey.exe+106B4BC9 - 41 0F29 7B A8         - movaps [r11-58],xmm7
;ACOdyssey.exe+106B4BCE - 45 0F29 4B 88         - movaps [r11-78],xmm9			>>> CONTINUE HERE
;ACOdyssey.exe+106B4BD3 - 48 89 4F 78           - mov [rdi+78],rcx
;ACOdyssey.exe+106B4BD7 - 48 85 D2              - test rdx,rdx
	mov [rsp+28h],rdx		
	mov rdx,[rdi+18h]					; loads rdx with the value we need. 
	mov [g_timestopStructAddress], rdx
	and ecx,10h
	xor rcx,r8
	movaps xmmword ptr [r11-58h],xmm7
exit:
	jmp qword ptr [_timestopReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
timestopReadInterceptor ENDP


fogReadInterceptor PROC
;v1.0.7
;ACOdyssey.exe+B69961C - 48 8B 8E 18010000     - mov rcx,[rsi+00000118]
;ACOdyssey.exe+B699623 - F3 45 0F10 56 54      - movss xmm10,[r14+54]
;ACOdyssey.exe+B699629 - F3 45 0F5F 56 48      - maxss xmm10,[r14+48]
;ACOdyssey.exe+B69962F - 41 0F28 46 30         - movaps xmm0,[r14+30]
;ACOdyssey.exe+B699634 - F3 41 0F10 4E 4C      - movss xmm1,[r14+4C]			<< Haze strength (0.01 is white, 0.99 is darker). r14, not rcx!
;ACOdyssey.exe+B69963A - F3 45 0F10 4E 5C      - movss xmm9,[r14+5C]
;ACOdyssey.exe+B699640 - F3 41 0F10 7E 58      - movss xmm7,[r14+58]			<< INTERCEPT HERE
;ACOdyssey.exe+B699646 - F3 44 0F59 51 20      - mulss xmm10,[rcx+20]			<< Read FOG value. Game doesn't write the value. 
;ACOdyssey.exe+B69964C - F3 45 0F10 46 50      - movss xmm8,[r14+50]
;ACOdyssey.exe+B699652 - 0F29 44 24 70         - movaps [rsp+70],xmm0			<< CONTINUE HERE
;ACOdyssey.exe+B699657 - F3 44 0F59 15 005F75F8  - mulss xmm10,[ACOdyssey.exe+3DEF560] { [0.00] }
;ACOdyssey.exe+B699660 - F3 0F11 4C 24 7C      - movss [rsp+7C],xmm1
;ACOdyssey.exe+B699666 - E8 15F240F5           - call ACOdyssey.exe+AA8880
;ACOdyssey.exe+B69966B - 48 8B 8E 18010000     - mov rcx,[rsi+00000118]
;ACOdyssey.exe+B699672 - 0F28 F0               - movaps xmm6,xmm0

	mov [g_fogStructAddress], rcx
originalCode:
	movss xmm7, dword ptr [r14+58h]
	mulss xmm10, dword ptr [rcx+20h]
	movss xmm8, dword ptr [r14+50h]
exit:
	jmp qword ptr [_fogReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
fogReadInterceptor ENDP


END