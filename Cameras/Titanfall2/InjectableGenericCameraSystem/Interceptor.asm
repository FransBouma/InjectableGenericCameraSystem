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
PUBLIC cameraWrite10Interceptor
PUBLIC cameraWrite11Interceptor
PUBLIC cameraWrite12Interceptor
PUBLIC cameraWrite13Interceptor
PUBLIC cameraWrite14Interceptor

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
EXTERN _cameraWrite5InterceptionContinue: qword
EXTERN _cameraWrite6InterceptionContinue: qword
EXTERN _cameraWrite7InterceptionContinue1: qword
EXTERN _cameraWrite7InterceptionContinue2: qword
EXTERN _cameraWrite8InterceptionContinue: qword
EXTERN _cameraWrite9InterceptionContinue: qword
EXTERN _cameraWrite10InterceptionContinue: qword
EXTERN _cameraWrite11InterceptionContinue: qword
EXTERN _cameraWrite12InterceptionContinue: qword
EXTERN _cameraWrite13InterceptionContinue: qword
EXTERN _cameraWrite14InterceptionContinue: qword

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
;client.dll+142C67 - 48 8B 03              - mov rax,[rbx]						
;client.dll+142C6A - FF 90 B0050000        - call qword ptr [rax+000005B0]
;client.dll+142C70 - 8B 08                 - mov ecx,[rax]
;client.dll+142C72 - 89 0F                 - mov [rdi],ecx						<< PITCH
;client.dll+142C74 - 8B 48 04              - mov ecx,[rax+04]
;client.dll+142C77 - 89 4F 04              - mov [rdi+04],ecx					<< YAW
;client.dll+142C7A - 8B 40 08              - mov eax,[rax+08]
;client.dll+142C7D - 89 47 08              - mov [rdi+08],eax					<< ROLL
;client.dll+142C80 - E8 8BAA3A00           - call client.dll+4ED710				<< CONTINUE HERE
;client.dll+142C85 - 33 D2                 - xor edx,edx
;client.dll+142C87 - 48 8B CB              - mov rcx,rbx

	mov [g_cameraStructAddress], rsi
	cmp byte ptr [g_cameraEnabled], 1
	je nowrites
originalCode:
	mov [rsi],ecx		
	mov ecx,[rax+04h]
	mov [rsi+04h],ecx	
	mov eax,[rax+08h]
	mov rcx,rbx
	mov [rsi+08h],eax	
	mov rax,[rbx]			
	call qword ptr [rax+000005B0h]
	mov ecx,[rax]
	mov [rdi],ecx					
	mov ecx,[rax+04h]
	mov [rdi+04h],ecx				
	mov eax,[rax+08h]
	mov [rdi+08h],eax
	jmp exit
nowrites:
	mov ecx,[rax+04h]
	mov eax,[rax+08h]
	mov rcx,rbx
	mov rax,[rbx]			
	call qword ptr [rax+000005B0h]
	mov ecx,[rax]
	mov ecx,[rax+04h]
	mov eax,[rax+08h]
exit:
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
;client.dll+142CE8 - F3 0F10 45 A7         - movss xmm0,[rbp-59]
;client.dll+142CED - F3 0F10 4D AB         - movss xmm1,[rbp-55]
;client.dll+142CF2 - F3 0F11 06            - movss [rsi],xmm0				<< INTERCEPT HERE<< X
;client.dll+142CF6 - F3 0F10 45 AF         - movss xmm0,[rbp-51]
;client.dll+142CFB - F3 0F11 4E 04         - movss [rsi+04],xmm1			<< Y
;client.dll+142D00 - F3 0F10 4D 97         - movss xmm1,[rbp-69]
;client.dll+142D05 - F3 0F11 46 08         - movss [rsi+08],xmm0			<< Z
;client.dll+142D0A - F3 0F10 45 9B         - movss xmm0,[rbp-65]
;client.dll+142D0F - F3 0F11 0F            - movss [rdi],xmm1				<< PITCH
;client.dll+142D13 - F3 0F10 4D 9F         - movss xmm1,[rbp-61]
;client.dll+142D18 - F3 0F11 47 04         - movss [rdi+04],xmm0			<< YAW
;client.dll+142D1D - F3 0F11 4F 08         - movss [rdi+08],xmm1			<< ROLL
;client.dll+142D22 - EB 56                 - jmp client.dll+142D7A			<< CONTINUE HERE
;client.dll+142D24 - 4C 8D 45 97           - lea r8,[rbp-69]
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rsi], xmm0	
	movss xmm0,dword ptr [rbp-51h]
	movss dword ptr [rsi+04h],xmm1
	movss xmm1,dword ptr [rbp-69h]
	movss dword ptr [rsi+08h],xmm0
	movss xmm0,dword ptr [rbp-65h]
	movss dword ptr [rdi],xmm1	
	movss xmm1,dword ptr [rbp-61h]
	movss dword ptr [rdi+04h],xmm0
	movss dword ptr [rdi+08h],xmm1
exit:
	jmp qword ptr [_cameraWrite3InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite3Interceptor ENDP

cameraWrite4Interceptor PROC
; 3rd person camera / cutscenes
;client.dll+14FB77 - 4C 8D 4C 24 40        - lea r9,[rsp+40]
;client.dll+14FB7C - 4C 8D 44 24 30        - lea r8,[rsp+30]
;client.dll+14FB81 - F3 0F10 44 24 20      - movss xmm0,[rsp+20]
;client.dll+14FB87 - F3 0F10 4C 24 24      - movss xmm1,[rsp+24]
;client.dll+14FB8D - 8B 08                 - mov ecx,[rax]
;client.dll+14FB8F - 48 8D 54 24 50        - lea rdx,[rsp+50]
;client.dll+14FB94 - 89 0F                 - mov [rdi],ecx						<< INTERCEPT HERE << WRITE X
;client.dll+14FB96 - 8B 48 04              - mov ecx,[rax+04]
;client.dll+14FB99 - 89 4F 04              - mov [rdi+04],ecx					<< Y
;client.dll+14FB9C - 8B 40 08              - mov eax,[rax+08]
;client.dll+14FB9F - 48 8B CE              - mov rcx,rsi
;client.dll+14FBA2 - 89 47 08              - mov [rdi+08],eax					<< Z
;client.dll+14FBA5 - C7 46 08 00000000     - mov [rsi+08],00000000				<< ROLL
;client.dll+14FBAC - F3 0F11 06            - movss [rsi],xmm0					<< PITCH
;client.dll+14FBB0 - F3 0F11 4E 04         - movss [rsi+04],xmm1				<< YAW
;client.dll+14FBB5 - E8 D6764D00           - call client.dll+627290				<< CONTINUE HERE
;client.dll+14FBBA - 8B 0D C00BAE00        - mov ecx,[client.dll+C30780] { [00000001] }
;client.dll+14FBC0 - F3 0F10 7C 24 28      - movss xmm7,[rsp+28]
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	mov [rdi],ecx
	mov ecx,[rax+04h]
	mov [rdi+04h],ecx		
	mov eax,[rax+08h]
	mov [rdi+08h],eax		
	mov dword ptr [rsi+08h],00000000h
	movss dword ptr [rsi],xmm0		
	movss dword ptr [rsi+04h],xmm1	
exit:
	mov rcx,rsi
	jmp qword ptr [_cameraWrite4InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite4Interceptor ENDP

cameraWrite5Interceptor PROC
; 3rd person camera / cutscenes
;client.dll+14FD66 - 49 8B 73 18           - mov rsi,[r11+18]
;client.dll+14FD6A - 0F28 C7               - movaps xmm0,xmm7
;client.dll+14FD6D - 0F28 CF               - movaps xmm1,xmm7
;client.dll+14FD70 - F3 0F59 7C 24 58      - mulss xmm7,[rsp+58]
;client.dll+14FD76 - F3 0F59 44 24 50      - mulss xmm0,[rsp+50]
;client.dll+14FD7C - F3 0F59 4C 24 54      - mulss xmm1,[rsp+54]
;client.dll+14FD82 - F3 0F58 7F 08         - addss xmm7,[rdi+08]
;client.dll+14FD87 - F3 0F58 07            - addss xmm0,[rdi]
;client.dll+14FD8B - F3 0F58 4F 04         - addss xmm1,[rdi+04]
;client.dll+14FD90 - F3 0F11 7F 08         - movss [rdi+08],xmm7					<< INTERCEPT HERE << Z
;client.dll+14FD95 - 0F28 7C 24 70         - movaps xmm7,[rsp+70]
;client.dll+14FD9A - F3 0F11 07            - movss [rdi],xmm0						<< X
;client.dll+14FD9E - F3 0F11 4F 04         - movss [rdi+04],xmm1					<< Y
;client.dll+14FDA3 - 49 8B E3              - mov rsp,r11							<< CONTINUE HERE
;client.dll+14FDA6 - 5F                    - pop rdi
;client.dll+14FDA7 - C3                    - ret 
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rdi+08h],xmm7	
	movss dword ptr [rdi],xmm0		
	movss dword ptr [rdi+04h],xmm1	
exit:
	movaps xmm7, xmmword ptr [rsp+70h]
	jmp qword ptr [_cameraWrite5InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite5Interceptor ENDP

cameraWrite6Interceptor PROC
; cutscenes
;client.dll+35B30D - 74 5C                 - je client.dll+35B36B
;client.dll+35B30F - 48 8B 10              - mov rdx,[rax]
;client.dll+35B312 - 48 8B C8              - mov rcx,rax
;client.dll+35B315 - FF 52 40              - call qword ptr [rdx+40]
;client.dll+35B318 - 49 8B 14 24           - mov rdx,[r12]
;client.dll+35B31C - 8B 08                 - mov ecx,[rax]
;client.dll+35B31E - 89 0B                 - mov [rbx],ecx				<< INTERCEPT HERE <<X
;client.dll+35B320 - 8B 48 04              - mov ecx,[rax+04]
;client.dll+35B323 - 89 4B 04              - mov [rbx+04],ecx			<<Y
;client.dll+35B326 - 8B 48 08              - mov ecx,[rax+08]
;client.dll+35B329 - 89 4B 08              - mov [rbx+08],ecx			<<Z
;client.dll+35B32C - 49 8B CC              - mov rcx,r12
;client.dll+35B32F - FF 52 48              - call qword ptr [rdx+48]
;client.dll+35B332 - 8B 08                 - mov ecx,[rax]
;client.dll+35B334 - 89 0F                 - mov [rdi],ecx				<< pitch
;client.dll+35B336 - 8B 48 04              - mov ecx,[rax+04]
;client.dll+35B339 - 89 4F 04              - mov [rdi+04],ecx			<< yaw
;client.dll+35B33C - 8B 40 08              - mov eax,[rax+08]
;client.dll+35B33F - 49 8B CC              - mov rcx,r12
;client.dll+35B342 - 89 47 08              - mov [rdi+08],eax			<< roll
;client.dll+35B345 - 49 8B 04 24           - mov rax,[r12]				<< CONTINUE HERE
;client.dll+35B349 - FF 90 28050000        - call qword ptr [rax+00000528]
	cmp byte ptr [g_cameraEnabled], 1
	je nowrites
originalCode:
	mov [rbx],ecx			
	mov ecx,[rax+04h]
	mov [rbx+04h],ecx		
	mov ecx,[rax+08h]
	mov [rbx+08h],ecx		
	mov rcx,r12
	call qword ptr [rdx+48h]
	mov ecx,[rax]
	mov [rdi],ecx			
	mov ecx,[rax+04h]
	mov [rdi+04h],ecx		
	mov eax,[rax+08h]
	mov rcx,r12
	mov [rdi+08h],eax		
	jmp exit
nowrites:
	mov ecx,[rax+04h]
	mov ecx,[rax+08h]
	mov rcx,r12
	call qword ptr [rdx+48h]
	mov ecx,[rax]
	mov ecx,[rax+04h]
	mov eax,[rax+08h]
	mov rcx,r12
exit:
	jmp qword ptr [_cameraWrite6InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite6Interceptor ENDP

cameraWrite7Interceptor PROC
;client.dll+62D46A - F3 0F10 35 06302A00   - movss xmm6,[client.dll+8D0478] { [57.30] }
;client.dll+62D472 - 44 0F57 05 36AD3D00   - xorps xmm8,[client.dll+A081B0] { [80000000] }
;client.dll+62D47A - 41 0F28 CA            - movaps xmm1,xmm10						<< INTERCEPT HERE
;client.dll+62D47E - F3 0F59 C6            - mulss xmm0,xmm6
;client.dll+62D482 - F3 0F11 47 04         - movss [rdi+04],xmm0					<< Yaw
;client.dll+62D487 - 41 0F28 C0            - movaps xmm0,xmm8
;client.dll+62D48B - E8 9C3D1D00           - call client.dll+80122C					<< CONTINUE1 HERE (Camera is disabled, or rdi isn't our camera)
;client.dll+62D490 - 41 0F28 CC            - movaps xmm1,xmm12
;client.dll+62D494 - F3 0F59 C6            - mulss xmm0,xmm6
;client.dll+62D498 - F3 0F11 07            - movss [rdi],xmm0						<< Pitch
;client.dll+62D49C - 41 0F28 C3            - movaps xmm0,xmm11
;client.dll+62D4A0 - E8 873D1D00           - call client.dll+80122C
;client.dll+62D4A5 - F3 0F59 C6            - mulss xmm0,xmm6
;client.dll+62D4A9 - F3 0F11 47 08         - movss [rdi+08],xmm0					<< Roll
;client.dll+62D4AE - EB 49                 - jmp client.dll+62D4F9					<< CONTINUE2 HERE (Camera is enabled and rdi is our camera)
;client.dll+62D4B0 - 0F57 35 F9AC3D00      - xorps xmm6,[client.dll+A081B0] { [80000000] }
	push rcx
	cmp byte ptr [g_cameraEnabled], 1
	jne originalCode
	; check if the value in rdi + 0x0C is our camera (as this code is used by many datablocks). 
	mov qword ptr rcx, [g_cameraStructAddress]
	add rcx, 0Ch
	cmp qword ptr rdi, rcx
	je exit2			; camera is enabled and it's targeting our camera struct, skip the complete block of statements
originalCode:
	movaps xmm1,xmm10	
	mulss xmm0,xmm6
	movss dword ptr [rdi+04h],xmm0
	movaps xmm0,xmm8
exit1:
	pop rcx
	; jmp back to the original code right after our injected jmp statement so the original code will run
	jmp qword ptr [_cameraWrite7InterceptionContinue1]
exit2:
	pop rcx
	; jmp back to the end of the block we want to intercept, skipping all calls too. 
	jmp qword ptr [_cameraWrite7InterceptionContinue2]
cameraWrite7Interceptor ENDP

cameraWrite8Interceptor PROC
;client.dll+142D36 - 74 42                 - je client.dll+142D7A
;client.dll+142D38 - F3 0F10 45 A7         - movss xmm0,[rbp-59]
;client.dll+142D3D - F3 0F10 4D AB         - movss xmm1,[rbp-55]
;client.dll+142D42 - 48 8B CB              - mov rcx,rbx
;client.dll+142D45 - F3 0F11 06            - movss [rsi],xmm0					<< Intercept here << X
;client.dll+142D49 - F3 0F10 45 AF         - movss xmm0,[rbp-51]
;client.dll+142D4E - F3 0F11 4E 04         - movss [rsi+04],xmm1				<< Y
;client.dll+142D53 - F3 0F10 4D 97         - movss xmm1,[rbp-69]
;client.dll+142D58 - F3 0F11 46 08         - movss [rsi+08],xmm0				<< Z
;client.dll+142D5D - F3 0F10 45 9B         - movss xmm0,[rbp-65]
;client.dll+142D62 - F3 0F11 0F            - movss [rdi],xmm1					<< Pitch
;client.dll+142D66 - F3 0F10 4D 9F         - movss xmm1,[rbp-61]
;client.dll+142D6B - F3 0F11 47 04         - movss [rdi+04],xmm0				<< Yaw
;client.dll+142D70 - F3 0F11 4F 08         - movss [rdi+08],xmm1				<< Roll
;client.dll+142D75 - E8 866D0000           - call client.dll+149B00				<< Continue here
;client.dll+142D7A - 48 8B 05 4F3D9C00     - mov rax,[client.dll+B06AD0] { [7FFD04D65C30] }
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rsi],xmm0	
	movss xmm0,dword ptr [rbp-51h]
	movss dword ptr [rsi+04h],xmm1
	movss xmm1,dword ptr [rbp-69h]
	movss dword ptr [rsi+08h],xmm0
	movss xmm0,dword ptr [rbp-65h]
	movss dword ptr [rdi],xmm1	
	movss xmm1,dword ptr [rbp-61h]
	movss dword ptr [rdi+04h],xmm0
	movss dword ptr [rdi+08h],xmm1
exit:
	jmp qword ptr [_cameraWrite8InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite8Interceptor ENDP


cameraWrite9Interceptor PROC
;client.dll+14302F - E8 2C421800           - call client.dll+2C7260
;client.dll+143034 - F3 0F10 07            - movss xmm0,[rdi]
;client.dll+143038 - F3 0F58 00            - addss xmm0,[rax]
;client.dll+14303C - F3 0F11 07            - movss [rdi],xmm0					<< INTERCEPT HERE << Pitch
;client.dll+143040 - F3 0F10 48 04         - movss xmm1,[rax+04]
;client.dll+143045 - F3 0F58 4F 04         - addss xmm1,[rdi+04]
;client.dll+14304A - F3 0F11 4F 04         - movss [rdi+04],xmm1				<< Yaw
;client.dll+14304F - F3 0F10 40 08         - movss xmm0,[rax+08]
;client.dll+143054 - F3 0F58 47 08         - addss xmm0,[rdi+08]
;client.dll+143059 - F3 0F11 47 08         - movss [rdi+08],xmm0				<< Roll
;client.dll+14305E - 48 8D 4D B7           - lea rcx,[rbp-49]					<< CONTINUE HERE
;client.dll+143062 - 48 8B D3              - mov rdx,rbx
;client.dll+143065 - E8 46281800           - call client.dll+2C58B0
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rdi],xmm0	
	movss xmm1,dword ptr [rax+04h]
	addss xmm1,dword ptr [rdi+04h]
	movss dword ptr [rdi+04h],xmm1
	movss xmm0,dword ptr [rax+08h]
	addss xmm0,dword ptr [rdi+08h]
	movss dword ptr [rdi+08h],xmm0
exit:
	jmp qword ptr [_cameraWrite9InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite9Interceptor ENDP

cameraWrite10Interceptor PROC
;client.dll+143065 - E8 46281800           - call client.dll+2C58B0
;client.dll+14306A - 48 8B D7              - mov rdx,rdi
;client.dll+14306D - 48 8B CB              - mov rcx,rbx
;client.dll+143070 - 41 0F28 D0            - movaps xmm2,xmm8
;client.dll+143074 - F3 0F10 00            - movss xmm0,[rax]
;client.dll+143078 - F3 0F58 07            - addss xmm0,[rdi]
;client.dll+14307C - F3 0F11 07            - movss [rdi],xmm0					<< INTERCEPT HERE << Pitch
;client.dll+143080 - F3 0F10 48 04         - movss xmm1,[rax+04]
;client.dll+143085 - F3 0F58 4F 04         - addss xmm1,[rdi+04]
;client.dll+14308A - F3 0F11 4F 04         - movss [rdi+04],xmm1				<< Yaw
;client.dll+14308F - F3 0F10 40 08         - movss xmm0,[rax+08]
;client.dll+143094 - F3 0F58 47 08         - addss xmm0,[rdi+08]
;client.dll+143099 - F3 0F11 47 08         - movss [rdi+08],xmm0				<< Roll
;client.dll+14309E - E8 CDE81700           - call client.dll+2C1970				<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rdi],xmm0	
	movss xmm1,dword ptr [rax+04h]
	addss xmm1,dword ptr [rdi+04h]
	movss dword ptr [rdi+04h],xmm1
	movss xmm0,dword ptr [rax+08h]
	addss xmm0,dword ptr [rdi+08h]
	movss dword ptr [rdi+08h],xmm0
exit:
	jmp qword ptr [_cameraWrite10InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite10Interceptor ENDP

cameraWrite11Interceptor PROC
;client.dll+142DA0 - 48 8D 55 A7           - lea rdx,[rbp-59]
;client.dll+142DA4 - F3 0F58 06            - addss xmm0,[rsi]
;client.dll+142DA8 - F3 0F58 4E 04         - addss xmm1,[rsi+04]
;client.dll+142DAD - 48 8B CB              - mov rcx,rbx
;client.dll+142DB0 - F3 0F11 06            - movss [rsi],xmm0					<< INTERCEPT HERE << X
;client.dll+142DB4 - F3 0F10 45 AF         - movss xmm0,[rbp-51]
;client.dll+142DB9 - F3 0F11 4E 04         - movss [rsi+04],xmm1				<< Y
;client.dll+142DBE - F3 0F58 46 08         - addss xmm0,[rsi+08]
;client.dll+142DC3 - F3 0F11 4D AB         - movss [rbp-55],xmm1
;client.dll+142DC8 - F3 0F11 46 08         - movss [rsi+08],xmm0				<< Z
;client.dll+142DCD - F3 0F10 06            - movss xmm0,[rsi]					<< CONTINUE HERE
;client.dll+142DD1 - F3 0F10 4F 04         - movss xmm1,[rdi+04]
;client.dll+142DD6 - F3 0F11 45 A7         - movss [rbp-59],xmm0
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rsi],xmm0	
	movss xmm0,dword ptr [rbp-51h]
	movss dword ptr [rsi+04h],xmm1
	addss xmm0,dword ptr [rsi+08h]
	movss dword ptr [rbp-55h],xmm1
	movss dword ptr [rsi+08h],xmm0
exit:
	jmp qword ptr [_cameraWrite11InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite11Interceptor ENDP

cameraWrite12Interceptor PROC
;client.dll+3628B3 - 0F28 CB               - movaps xmm1,xmm3
;client.dll+3628B6 - F3 0F59 81 F8030000   - mulss xmm0,[rcx+000003F8]
;client.dll+3628BE - F3 0F58 02            - addss xmm0,[rdx]
;client.dll+3628C2 - F3 0F11 02            - movss [rdx],xmm0					<< INTERCEPT HERE << X
;client.dll+3628C6 - F3 0F59 89 FC030000   - mulss xmm1,[rcx+000003FC]
;client.dll+3628CE - F3 0F58 4A 04         - addss xmm1,[rdx+04]
;client.dll+3628D3 - F3 0F11 4A 04         - movss [rdx+04],xmm1				<< Y
;client.dll+3628D8 - F3 0F10 4C 24 28      - movss xmm1,[rsp+28]
;client.dll+3628DE - F3 0F59 99 00040000   - mulss xmm3,[rcx+00000400]
;client.dll+3628E6 - F3 0F58 5A 08         - addss xmm3,[rdx+08]
;client.dll+3628EB - 0F28 C1               - movaps xmm0,xmm1
;client.dll+3628EE - F3 0F11 5A 08         - movss [rdx+08],xmm3				<< Z
;client.dll+3628F3 - F3 0F59 81 04040000   - mulss xmm0,[rcx+00000404]
;client.dll+3628FB - F3 41 0F58 00         - addss xmm0,[r8]
;client.dll+362900 - F3 41 0F11 00         - movss [r8],xmm0					<< Pitch
;client.dll+362905 - F3 0F59 89 08040000   - mulss xmm1,[rcx+00000408]
;client.dll+36290D - F3 41 0F58 48 08      - addss xmm1,[r8+08]
;client.dll+362913 - F3 41 0F11 48 08      - movss [r8+08],xmm1					<< Roll
;client.dll+362919 - C3                    - ret								<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1
	je nowrites 
originalCode:
	movss dword ptr [rdx],xmm0			
	mulss xmm1,dword ptr [rcx+000003FCh]
	addss xmm1,dword ptr [rdx+04h]
	movss dword ptr [rdx+04h],xmm1		
	movss xmm1,dword ptr [rsp+28h]
	mulss xmm3,dword ptr [rcx+00000400h]
	addss xmm3,dword ptr [rdx+08h]
	movaps xmm0,xmm1
	movss dword ptr [rdx+08h],xmm3		
	mulss xmm0,dword ptr [rcx+00000404h]
	addss xmm0,dword ptr [r8]
	movss dword ptr [r8],xmm0			
	mulss xmm1,dword ptr [rcx+00000408h]
	addss xmm1,dword ptr [r8+08h]
	movss dword ptr [r8+08h],xmm1
	jmp exit
nowrites:
	mulss xmm1,dword ptr [rcx+000003FCh]
	addss xmm1,dword ptr [rdx+04h]
	movss xmm1,dword ptr [rsp+28h]
	mulss xmm3,dword ptr [rcx+00000400h]
	addss xmm3,dword ptr [rdx+08h]
	movaps xmm0,xmm1
	mulss xmm0,dword ptr [rcx+00000404h]
	addss xmm0,dword ptr [r8]
	mulss xmm1,dword ptr [rcx+00000408h]
	addss xmm1,dword ptr [r8+08h]
exit:
	jmp qword ptr [_cameraWrite12InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite12Interceptor ENDP

cameraWrite13Interceptor PROC
;client.dll+2CDB44 - 75 34                 - jne client.dll+2CDB7A
;client.dll+2CDB46 - F3 0F10 87 101E0000   - movss xmm0,[rdi+00001E10]
;client.dll+2CDB4E - F3 0F58 03            - addss xmm0,[rbx]
;client.dll+2CDB52 - F3 0F11 03            - movss [rbx],xmm0					<< INTERCEPT HERE << X
;client.dll+2CDB56 - F3 0F10 8F 141E0000   - movss xmm1,[rdi+00001E14]
;client.dll+2CDB5E - F3 0F58 4B 04         - addss xmm1,[rbx+04]
;client.dll+2CDB63 - F3 0F11 4B 04         - movss [rbx+04],xmm1				<< Y
;client.dll+2CDB68 - F3 0F10 87 181E0000   - movss xmm0,[rdi+00001E18]
;client.dll+2CDB70 - F3 0F58 43 08         - addss xmm0,[rbx+08]
;client.dll+2CDB75 - F3 0F11 43 08         - movss [rbx+08],xmm0				<< Z
;client.dll+2CDB7A - 48 8B 5C 24 30        - mov rbx,[rsp+30]					<< CONTINUE HERE
;client.dll+2CDB7F - 48 83 C4 20           - add rsp,20 { 32 }
;client.dll+2CDB83 - 5F                    - pop rdi
;client.dll+2CDB84 - C3                    - ret 
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rbx],xmm0			
	movss xmm1,dword ptr [rdi+00001E14h]
	addss xmm1,dword ptr [rbx+04h]
	movss dword ptr [rbx+04h],xmm1		
	movss xmm0,dword ptr [rdi+00001E18h]
	addss xmm0,dword ptr [rbx+08h]
	movss dword ptr [rbx+08h],xmm0		
exit:
	jmp qword ptr [_cameraWrite13InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite13Interceptor ENDP

cameraWrite14Interceptor PROC
;client.dll+142980 - E8 5BAA4E00           - call client.dll+62D3E0
;client.dll+142985 - 4C 8B C6              - mov r8,rsi
;client.dll+142988 - 48 8B D7              - mov rdx,rdi
;client.dll+14298B - F3 0F10 44 24 3C      - movss xmm0,[rsp+3C]
;client.dll+142991 - F3 0F10 4C 24 4C      - movss xmm1,[rsp+4C]
;client.dll+142997 - 48 8B CD              - mov rcx,rbp
;client.dll+14299A - F3 0F11 07            - movss [rdi],xmm0					<< INTERCEPT HERE << X
;client.dll+14299E - F3 0F10 44 24 5C      - movss xmm0,[rsp+5C]
;client.dll+1429A4 - F3 0F11 4F 04         - movss [rdi+04],xmm1				<< Y
;client.dll+1429A9 - F3 0F11 47 08         - movss [rdi+08],xmm0				<< Z
;client.dll+1429AE - E8 AD5F0100           - call client.dll+158960				<< CONTINUE HERE
;client.dll+1429B3 - 4C 8D 9C 24 C0000000  - lea r11,[rsp+000000C0]
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rdi],xmm0
	movss xmm0,dword ptr [rsp+5Ch]
	movss dword ptr [rdi+04h],xmm1
	movss dword ptr [rdi+08h],xmm0
exit:
	jmp qword ptr [_cameraWrite14InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite14Interceptor ENDP

END