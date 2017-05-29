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
PUBLIC cameraWrite6Interceptor
PUBLIC cameraWrite7Interceptor
PUBLIC cameraWrite8Interceptor
PUBLIC cameraWrite9Interceptor
PUBLIC fovWriteInterceptor
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
EXTERN _cameraWrite2InterceptionContinue: qword
EXTERN _cameraWrite3InterceptionContinue: qword
EXTERN _cameraWrite4InterceptionContinue: qword
EXTERN _cameraWrite5InterceptionContinue: qword
EXTERN _cameraWrite6InterceptionContinue: qword
EXTERN _cameraWrite7InterceptionContinue: qword
EXTERN _cameraWrite8InterceptionContinue: qword
EXTERN _cameraWrite9InterceptionContinue: qword
EXTERN _fovWriteInterceptionContinue: qword
EXTERN _timestopInterceptionContinue: qword
.data

;---------------------------------------------------------------
; Scratch pad
.code


cameraStructInterceptor PROC
; Struct interceptor is also a write interceptor for coords/quaternion
;Ryse.exe+1531AE4 - 89 06                 - mov [rsi],eax				<< INTERCEPT HERE << WRITE X
;Ryse.exe+1531AE6 - 8B 44 24 54           - mov eax,[rsp+54]
;Ryse.exe+1531AEA - F3 0F59 F0            - mulss xmm6,xmm0
;Ryse.exe+1531AEE - F3 0F59 F8            - mulss xmm7,xmm0
;Ryse.exe+1531AF2 - F3 44 0F10 45 D8      - movss xmm8,[rbp-28]
;Ryse.exe+1531AF8 - F3 0F11 75 D0         - movss [rbp-30],xmm6
;Ryse.exe+1531AFD - F3 0F11 7D D4         - movss [rbp-2C],xmm7
;Ryse.exe+1531B02 - 89 46 04              - mov [rsi+04],eax			<< WRITE Y
;Ryse.exe+1531B05 - 8B 44 24 58           - mov eax,[rsp+58]
;Ryse.exe+1531B09 - F3 41 0F59 F3         - mulss xmm6,xmm11
;Ryse.exe+1531B0E - F3 44 0F59 C0         - mulss xmm8,xmm0
;Ryse.exe+1531B13 - 0F28 C6               - movaps xmm0,xmm6
;Ryse.exe+1531B16 - 89 46 08              - mov [rsi+08],eax			<< WRITE Z
;Ryse.exe+1531B19 - F3 44 0F11 45 D8      - movss [rbp-28],xmm8			<< CONTINUE HERE
	mov [g_cameraStructAddress], rsi
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
noWrites:
	mov eax,[rsp+54h]
	mulss xmm6,xmm0
	mulss xmm7,xmm0
	movss xmm8, dword ptr [rbp-28h]
	movss dword ptr [rbp-30h], xmm6
	movss dword ptr [rbp-2Ch], xmm7
	mov eax, [rsp+58h]
	mulss xmm6,xmm11
	mulss xmm8,xmm0
	movaps xmm0,xmm6
	jmp exit
originalCode:
	mov [rsi],eax		
	mov eax,[rsp+54h]
	mulss xmm6,xmm0
	mulss xmm7,xmm0
	movss xmm8, dword ptr [rbp-28h]
	movss dword ptr [rbp-30h], xmm6
	movss dword ptr [rbp-2Ch], xmm7
	mov [rsi+04h],eax	
	mov eax, [rsp+58h]
	mulss xmm6,xmm11
	mulss xmm8,xmm0
	movaps xmm0,xmm6
	mov [rsi+08h],eax	
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP


cameraWrite1Interceptor PROC
;Ryse.exe+346C07 - 89 06                 - mov [rsi],eax				<< INTERCEPT HERE	<< WRITE X
;Ryse.exe+346C09 - F3 44 0F11 5C 24 44   - movss [rsp+44],xmm11
;Ryse.exe+346C10 - F3 44 0F11 44 24 48   - movss [rsp+48],xmm8
;Ryse.exe+346C17 - 8B 44 24 44           - mov eax,[rsp+44]
;Ryse.exe+346C1B - 89 46 04              - mov [rsi+04],eax
;Ryse.exe+346C1E - 8B 44 24 48           - mov eax,[rsp+48]
;Ryse.exe+346C22 - 89 46 08              - mov [rsi+08],eax
;Ryse.exe+346C25 - 4D 85 E4              - test r12,r12					<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
noWrites:
	movss dword ptr [rsp+44h],xmm11
	movss dword ptr [rsp+48h],xmm8
	mov eax,[rsp+44h]
	mov eax,[rsp+48h]
	jmp exit
originalCode:
	mov [rsi],eax		
	movss dword ptr [rsp+44h],xmm11
	movss dword ptr [rsp+48h],xmm8
	mov eax,[rsp+44h]
	mov [rsi+04h],eax
	mov eax,[rsp+48h]
	mov [rsi+08h],eax
exit:
	jmp qword ptr [_cameraWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite1Interceptor ENDP


cameraWrite2Interceptor PROC
;Ryse.exe+347D20 - 89 03                 - mov [rbx],eax				<<INTERCEPT HERE	<< WRITE X
;Ryse.exe+347D22 - 8B 45 AB              - mov eax,[rbp-55]
;Ryse.exe+347D25 - 89 43 04              - mov [rbx+04],eax
;Ryse.exe+347D28 - F3 0F11 45 AF         - movss [rbp-51],xmm0
;Ryse.exe+347D2D - 8B 45 AF              - mov eax,[rbp-51]
;Ryse.exe+347D30 - 89 43 08              - mov [rbx+08],eax
;Ryse.exe+347D33 - 8B 83 94000000        - mov eax,[rbx+00000094]		<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
noWrites:
	mov eax,[rbp-55h]
	movss dword ptr [rbp-51h],xmm0
	mov eax,[rbp-51h]
	jmp exit
originalCode:
	mov [rbx],eax		
	mov eax,[rbp-55h]
	mov [rbx+04h],eax
	movss dword ptr [rbp-51h],xmm0
	mov eax,[rbp-51h]
	mov [rbx+08h],eax
exit:
	jmp qword ptr [_cameraWrite2InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite2Interceptor ENDP

cameraWrite3Interceptor PROC
;Ryse.exe+342EA2 - 89 01                 - mov [rcx],eax				<< INTERCEPT HERE << WRITE X
;Ryse.exe+342EA4 - 8B 42 04              - mov eax,[rdx+04]
;Ryse.exe+342EA7 - 89 41 04              - mov [rcx+04],eax
;Ryse.exe+342EAA - 8B 42 08              - mov eax,[rdx+08]
;Ryse.exe+342EAD - 89 41 08              - mov [rcx+08],eax
;Ryse.exe+342EB0 - 8B 42 0C              - mov eax,[rdx+0C]
;Ryse.exe+342EB3 - 89 41 0C              - mov [rcx+0C],eax				<< qX
;Ryse.exe+342EB6 - 8B 42 10              - mov eax,[rdx+10]
;Ryse.exe+342EB9 - 89 41 10              - mov [rcx+10],eax
;Ryse.exe+342EBC - 8B 42 14              - mov eax,[rdx+14]
;Ryse.exe+342EBF - 89 41 14              - mov [rcx+14],eax
;Ryse.exe+342EC2 - 8B 42 18              - mov eax,[rdx+18]
;Ryse.exe+342EC5 - 89 41 18              - mov [rcx+18],eax
;Ryse.exe+342EC8 - 8B 42 1C              - mov eax,[rdx+1C]
;Ryse.exe+342ECB - 89 41 1C              - mov [rcx+1C],eax
;Ryse.exe+342ECE - 8B 42 20              - mov eax,[rdx+20]
;Ryse.exe+342ED1 - 89 41 20              - mov [rcx+20],eax
;Ryse.exe+342ED4 - 8B 42 24              - mov eax,[rdx+24]
;Ryse.exe+342ED7 - 89 41 24              - mov [rcx+24],eax
;Ryse.exe+342EDA - 8B 42 28              - mov eax,[rdx+28]
;Ryse.exe+342EDD - 89 41 28              - mov [rcx+28],eax
;Ryse.exe+342EE0 - 8B 42 2C              - mov eax,[rdx+2C]
;Ryse.exe+342EE3 - 89 41 2C              - mov [rcx+2C],eax
;Ryse.exe+342EE6 - 8B 42 30              - mov eax,[rdx+30]
;Ryse.exe+342EE9 - 89 41 30              - mov [rcx+30],eax				<< FOV
;Ryse.exe+342EEC - 0FB6 42 34            - movzx eax,byte ptr [rdx+34]  << CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1						; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode										; our own camera is enabled, just skip the writes
noWrites:
	mov eax,[rdx+1Ch]
	mov [rcx+1Ch],eax
	mov eax,[rdx+20h]
	mov [rcx+20h],eax
	mov eax,[rdx+24h]
	mov [rcx+24h],eax
	mov eax,[rdx+28h]
	mov [rcx+28h],eax
	mov eax,[rdx+2Ch]
	mov [rcx+2Ch],eax
	jmp exit
originalCode:
	mov [rcx],eax						;X
	mov eax,[rdx+04h]
	mov [rcx+04h],eax					;Y
	mov eax,[rdx+08h]
	mov [rcx+08h],eax					;Z
	mov eax,[rdx+0Ch]
	mov [rcx+0Ch],eax					;Qx
	mov eax,[rdx+10h]
	mov [rcx+10h],eax					;Qy
	mov eax,[rdx+14h]
	mov [rcx+14h],eax					;Qz
	mov eax,[rdx+18h]
	mov [rcx+18h],eax					;Qw
	mov eax,[rdx+1Ch]
	mov [rcx+1Ch],eax
	mov eax,[rdx+20h]
	mov [rcx+20h],eax
	mov eax,[rdx+24h]
	mov [rcx+24h],eax
	mov eax,[rdx+28h]
	mov [rcx+28h],eax
	mov eax,[rdx+2Ch]
	mov [rcx+2Ch],eax
	mov eax,[rdx+30h]
	mov [rcx+30h],eax					; FOV
exit:
	jmp qword ptr [_cameraWrite3InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite3Interceptor ENDP


cameraWrite4Interceptor PROC
;Ryse.exe+1531C28 - F3 44 0F11 46 18      - movss [rsi+18],xmm8						<< INTERCEPT HERE // qW
;Ryse.exe+1531C2E - 0F28 CF               - movaps xmm1,xmm7
;Ryse.exe+1531C31 - 0F28 D5               - movaps xmm2,xmm5
;Ryse.exe+1531C34 - F3 0F59 C5            - mulss xmm0,xmm5
;Ryse.exe+1531C38 - F3 0F59 E6            - mulss xmm4,xmm6
;Ryse.exe+1531C3C - F3 0F5C E0            - subss xmm4,xmm0
;Ryse.exe+1531C40 - 41 0F28 C4            - movaps xmm0,xmm12
;Ryse.exe+1531C44 - F3 41 0F59 CB         - mulss xmm1,xmm11
;Ryse.exe+1531C49 - F3 41 0F59 D3         - mulss xmm2,xmm11
;Ryse.exe+1531C4E - F3 0F58 E1            - addss xmm4,xmm1
;Ryse.exe+1531C52 - 41 0F28 C9            - movaps xmm1,xmm9
;Ryse.exe+1531C56 - F3 0F59 C3            - mulss xmm0,xmm3
;Ryse.exe+1531C5A - F3 44 0F59 CB         - mulss xmm9,xmm3
;Ryse.exe+1531C5F - F3 0F59 CF            - mulss xmm1,xmm7
;Ryse.exe+1531C63 - F3 0F58 E0            - addss xmm4,xmm0
;Ryse.exe+1531C67 - 41 0F28 C5            - movaps xmm0,xmm13
;Ryse.exe+1531C6B - F3 0F11 66 0C         - movss [rsi+0C],xmm4						// qX
;Ryse.exe+1531C70 - F3 0F59 C3            - mulss xmm0,xmm3
;Ryse.exe+1531C74 - F3 44 0F59 EF         - mulss xmm13,xmm7
;Ryse.exe+1531C79 - F3 0F5C D0            - subss xmm2,xmm0
;Ryse.exe+1531C7D - 41 0F28 C4            - movaps xmm0,xmm12
;Ryse.exe+1531C81 - F3 0F59 C6            - mulss xmm0,xmm6
;Ryse.exe+1531C85 - F3 44 0F59 E5         - mulss xmm12,xmm5
;Ryse.exe+1531C8A - F3 41 0F59 F3         - mulss xmm6,xmm11
;Ryse.exe+1531C8F - F3 0F58 D1            - addss xmm2,xmm1
;Ryse.exe+1531C93 - F3 0F58 D0            - addss xmm2,xmm0
;Ryse.exe+1531C97 - F3 0F10 85 00040000   - movss xmm0,[rbp+00000400]
;Ryse.exe+1531C9F - F3 44 0F5C CE         - subss xmm9,xmm6
;Ryse.exe+1531CA4 - F3 0F11 56 10         - movss [rsi+10],xmm2						// qY
;Ryse.exe+1531CA9 - F3 45 0F58 CD         - addss xmm9,xmm13
;Ryse.exe+1531CAE - 44 0F28 AC 24 30040000  - movaps xmm13,[rsp+00000430]
;Ryse.exe+1531CB7 - F3 45 0F58 CC         - addss xmm9,xmm12
;Ryse.exe+1531CBC - 44 0F28 A4 24 40040000  - movaps xmm12,[rsp+00000440]
;Ryse.exe+1531CC5 - F3 44 0F11 4E 14      - movss [rsi+14],xmm9						// qZ
;Ryse.exe+1531CCB - F3 0F11 46 30         - movss [rsi+30],xmm0						// FOV
;Ryse.exe+1531CD0 - F3 0F10 45 94         - movss xmm0,[rbp-6C]						<< CONTINUE HERE
;
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
noWrites:
	movaps xmm1,xmm7
	movaps xmm2,xmm5
	mulss xmm0,xmm5
	mulss xmm4,xmm6
	subss xmm4,xmm0
	movaps xmm0,xmm12
	mulss xmm1,xmm11
	mulss xmm2,xmm11
	addss xmm4,xmm1
	movaps xmm1,xmm9
	mulss xmm0,xmm3
	mulss xmm9,xmm3
	mulss xmm1,xmm7
	addss xmm4,xmm0
	movaps xmm0,xmm13
	mulss xmm0,xmm3
	mulss xmm13,xmm7
	subss xmm2,xmm0
	movaps xmm0,xmm12
	mulss xmm0,xmm6
	mulss xmm12,xmm5
	mulss xmm6,xmm11
	addss xmm2,xmm1
	addss xmm2,xmm0
	movss xmm0, dword ptr [rbp+00000400h]
	subss xmm9,xmm6
	addss xmm9,xmm13
	movaps xmm13, [rsp+00000430h]
	addss xmm9,xmm12
	movaps xmm12, [rsp+00000440h]
	jmp exit
originalCode:
	movss dword ptr [rsi+18h],xmm8				
	movaps xmm1,xmm7
	movaps xmm2,xmm5
	mulss xmm0,xmm5
	mulss xmm4,xmm6
	subss xmm4,xmm0
	movaps xmm0,xmm12
	mulss xmm1,xmm11
	mulss xmm2,xmm11
	addss xmm4,xmm1
	movaps xmm1,xmm9
	mulss xmm0,xmm3
	mulss xmm9,xmm3
	mulss xmm1,xmm7
	addss xmm4,xmm0
	movaps xmm0,xmm13
	movss dword ptr [rsi+0Ch],xmm4				
	mulss xmm0,xmm3
	mulss xmm13,xmm7
	subss xmm2,xmm0
	movaps xmm0,xmm12
	mulss xmm0,xmm6
	mulss xmm12,xmm5
	mulss xmm6,xmm11
	addss xmm2,xmm1
	addss xmm2,xmm0
	movss xmm0, dword ptr [rbp+00000400h]
	subss xmm9,xmm6
	movss dword ptr [rsi+10h],xmm2				
	addss xmm9,xmm13
	movaps xmm13, [rsp+00000430h]
	addss xmm9,xmm12
	movaps xmm12, [rsp+00000440h]
	movss dword ptr [rsi+14h],xmm9
	movss dword ptr [rsi+30h],xmm0	
exit:
	jmp qword ptr [_cameraWrite4InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite4Interceptor ENDP


cameraWrite5Interceptor PROC
;Ryse.exe+346A6A - 89 07                 - mov [rdi],eax						<< INTERCEPT HERE	// qX
;Ryse.exe+346A6C - F3 0F11 4C 24 44      - movss [rsp+44],xmm1
;Ryse.exe+346A72 - F3 41 0F58 D1         - addss xmm2,xmm9
;Ryse.exe+346A77 - 8B 44 24 44           - mov eax,[rsp+44]
;Ryse.exe+346A7B - 89 47 04              - mov [rdi+04],eax
;Ryse.exe+346A7E - F3 0F11 54 24 48      - movss [rsp+48],xmm2
;Ryse.exe+346A84 - 8B 44 24 48           - mov eax,[rsp+48]
;Ryse.exe+346A88 - 89 47 08              - mov [rdi+08],eax
;Ryse.exe+346A8B - F3 0F10 17            - movss xmm2,[rdi]						<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
noWrites:
	movss dword ptr [rsp+44h],xmm1
	addss xmm2,xmm9
	mov eax,[rsp+44h]
	movss dword ptr [rsp+48h],xmm2
	mov eax,[rsp+48h]
	jmp exit
originalCode:
	mov [rdi],eax		
	movss dword ptr [rsp+44h],xmm1
	addss xmm2,xmm9
	mov eax,[rsp+44h]
	mov [rdi+04h],eax
	movss dword ptr [rsp+48h],xmm2
	mov eax,[rsp+48h]
	mov [rdi+08h],eax
exit:
	jmp qword ptr [_cameraWrite5InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite5Interceptor ENDP

cameraWrite6Interceptor PROC
;Ryse.exe+346AED - F3 44 0F11 57 0C      - movss [rdi+0C],xmm10					<< INTERCEPT HERE
;Ryse.exe+346AF3 - F3 0F11 17            - movss [rdi],xmm2						// qX
;Ryse.exe+346AF7 - F3 0F11 5F 04         - movss [rdi+04],xmm3	
;Ryse.exe+346AFC - F3 0F59 E1            - mulss xmm4,xmm1
;Ryse.exe+346B00 - F3 0F11 67 08         - movss [rdi+08],xmm4	
;Ryse.exe+346B05 - EB 12                 - jmp Ryse.exe+346B19					<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
noWrites:
	mulss xmm4,xmm1
	jmp exit
originalCode:
	movss dword ptr [rdi+0Ch],xmm10
	movss dword ptr [rdi],xmm2		
	movss dword ptr [rdi+04h],xmm3	
	movss dword ptr [rdi+08h],xmm4	
exit:
	jmp qword ptr [_cameraWrite6InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite6Interceptor ENDP

cameraWrite7Interceptor PROC
;Ryse.exe+347EA0 - 89 43 0C              - mov [rbx+0C],eax						<< INTERCEPT HERE// qX
;Ryse.exe+347EA3 - 44 0F28 8C 24 90000000  - movaps xmm9,[rsp+00000090]
;Ryse.exe+347EAC - 44 0F28 84 24 A0000000  - movaps xmm8,[rsp+000000A0]
;Ryse.exe+347EB5 - F3 0F11 4D AB         - movss [rbp-55],xmm1
;Ryse.exe+347EBA - F3 0F11 55 AF         - movss [rbp-51],xmm2
;Ryse.exe+347EBF - 8B 45 AB              - mov eax,[rbp-55]
;Ryse.exe+347EC2 - 89 43 10              - mov [rbx+10],eax
;Ryse.exe+347EC5 - 8B 45 AF              - mov eax,[rbp-51]
;Ryse.exe+347EC8 - 89 43 14              - mov [rbx+14],eax
;Ryse.exe+347ECB - 0FB6 43 34            - movzx eax,byte ptr [rbx+34]			<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
noWrites:
	movaps xmm9, [rsp+00000090h]
	movaps xmm8, [rsp+000000A0h]
	movss dword ptr [rbp-55h],xmm1
	movss dword ptr [rbp-51h],xmm2
	mov eax,[rbp-55h]
	mov eax,[rbp-51h]
	jmp exit
originalCode:
	mov [rbx+0Ch],eax				
	movaps xmm9, [rsp+00000090h]
	movaps xmm8, [rsp+000000A0h]
	movss dword ptr [rbp-55h],xmm1
	movss dword ptr [rbp-51h],xmm2
	mov eax,[rbp-55h]
	mov [rbx+10h],eax
	mov eax,[rbp-51h]
	mov [rbx+14h],eax
exit:
	jmp qword ptr [_cameraWrite7InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite7Interceptor ENDP

cameraWrite8Interceptor PROC
;Ryse.exe+346989 - F3 44 0F5C D2         - subss xmm10,xmm2					<< INTERCEPT HERE
;Ryse.exe+34698E - F3 44 0F11 57 0C      - movss [rdi+0C],xmm10					// qW
;Ryse.exe+346994 - 8B 43 54              - mov eax,[rbx+54]
;Ryse.exe+346997 - 89 44 24 40           - mov [rsp+40],eax					<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
noWrites:
	subss xmm10,xmm2		
	mov eax,[rbx+54h]
	jmp exit
originalCode:
	subss xmm10,xmm2		
	movss dword ptr [rdi+0Ch],xmm10
	mov eax,[rbx+54h]
exit:
	jmp qword ptr [_cameraWrite8InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite8Interceptor ENDP

cameraWrite9Interceptor PROC
;Ryse.exe+347D98 - F3 0F5C DA            - subss xmm3,xmm2					<< INTERCEPT HERE
;Ryse.exe+347D9C - F3 0F11 5B 18         - movss [rbx+18],xmm3					// qW
;Ryse.exe+347DA1 - 8B 83 94000000        - mov eax,[rbx+00000094]
;Ryse.exe+347DA7 - 89 45 A7              - mov [rbp-59],eax					<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
noWrites:
	subss xmm3,xmm2			
	mov eax,[rbx+00000094h]
	jmp exit
originalCode:
	subss xmm3,xmm2			
	movss dword ptr [rbx+18h],xmm3		
	mov eax,[rbx+00000094h]
exit:
	jmp qword ptr [_cameraWrite9InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite9Interceptor ENDP

timestopInterceptor PROC
;Ryse.exe+EFBD8A - 44 0F28 54 24 50      - movaps xmm10,[rsp+50]					<< INTERCEPT HERE
;Ryse.exe+EFBD90 - 44 0F28 4C 24 60      - movaps xmm9,[rsp+60]
;Ryse.exe+EFBD96 - 80 BF 9C020000 00     - cmp byte ptr [rdi+0000029C],00 { 0 }				<< READ TIMESTOP
;Ryse.exe+EFBD9D - F2 0F10 15 2B1B0E01   - movsd xmm2,[Ryse.exe+1FDD8D0] { [1.00] }	<< CONTINUE HERE
	mov [g_timestopStructAddress], rdi
originalCode:
	movaps xmm10,[rsp+50h]		
	movaps xmm9,[rsp+60h]
	cmp byte ptr [rdi+0000029Ch],00
exit:
	jmp qword ptr [_timestopInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
timestopInterceptor ENDP

fovWriteInterceptor PROC
;Ryse.exe+347ED4 - 88 83 A9000000        - mov [rbx+000000A9],al				<< INTERCEPT HERE
;Ryse.exe+347EDA - F3 0F58 83 A4000000   - addss xmm0,[rbx+000000A4]
;Ryse.exe+347EE2 - F3 0F11 43 30         - movss [rbx+30],xmm0					// FOV
;Ryse.exe+347EE7 - EB 31                 - jmp Ryse.exe+347F1A					<< CONTINUE HERE
	mov byte ptr [rbx+000000A9h],al	
	addss xmm0, dword ptr [rbx+000000A4h]
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movss dword ptr [rbx+30h],xmm0		
exit:
	jmp qword ptr [_fovWriteInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
fovWriteInterceptor ENDP

END