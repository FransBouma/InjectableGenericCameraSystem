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
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_cameraStructAddress: qword
EXTERN g_fovConstructAddress: qword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWrite1InterceptionContinue: qword
EXTERN _cameraWrite2InterceptionContinue: qword
EXTERN _cameraWrite3InterceptionContinue: qword
EXTERN _cameraWrite4InterceptionContinue: qword
EXTERN _cameraWrite5InterceptionContinue: qword
EXTERN _fovReadInterceptionContinue : qword
.data

;---------------------------------------------------------------
; Scratch pad
; player intercept. Is matrix (3x4, where the the coords are the lower row, write like this:
;matrixInMemory[0] = rotationMatrix._11;
;matrixInMemory[1] = rotationMatrix._21;
;matrixInMemory[2] = rotationMatrix._31;
;matrixInMemory[3] = newCoords.x;
;matrixInMemory[4] = rotationMatrix._12;
;matrixInMemory[5] = rotationMatrix._22;
;matrixInMemory[6] = rotationMatrix._32;
;matrixInMemory[7] = newCoords.y;
;matrixInMemory[8] = rotationMatrix._13;
;matrixInMemory[9] = rotationMatrix._23;
;matrixInMemory[10] = rotationMatrix._33;
;matrixInMemory[11] = newCoords.z;
;
;Prey.AK::Monitor::PostCode+24A6D9 - F3 45 0F11 7C 24 0C   - movss [r12+0C],xmm15			// write x			<< INTERCEPT HERE
;Prey.AK::Monitor::PostCode+24A6E0 - F3 45 0F11 74 24 1C   - movss [r12+1C],xmm14			// write y
;Prey.AK::Monitor::PostCode+24A6E7 - F3 0F59 C1            - mulss xmm0,xmm1
;Prey.AK::Monitor::PostCode+24A6EB - F3 0F59 E9            - mulss xmm5,xmm1
;Prey.AK::Monitor::PostCode+24A6EF - F3 44 0F59 E3         - mulss xmm12,xmm3
;Prey.AK::Monitor::PostCode+24A6F4 - 41 0F28 CB            - movaps xmm1,xmm11
;Prey.AK::Monitor::PostCode+24A6F8 - 41 0F28 F2            - movaps xmm6,xmm10
;Prey.AK::Monitor::PostCode+24A6FC - F3 0F5C F8            - subss xmm7,xmm0
;Prey.AK::Monitor::PostCode+24A700 - F3 45 0F11 44 24 2C   - movss [r12+2C],xmm8			// write z
;Prey.AK::Monitor::PostCode+24A707 - F3 0F59 F2            - mulss xmm6,xmm2									<< CONTINUE HERE


;//////////////
; These writes are needed for camera functioning: coords are otherwise resulting in 0. it can be just 1 is OK, but at least one.
;// coords write (coords write not really needed)
;Prey.AK::Monitor::PostCode+24EAC5 - F3 0F58 4B 04         - addss xmm1,[rbx+04]
;Prey.AK::Monitor::PostCode+24EACA - F3 0F11 13            - movss [rbx],xmm2			// coords
;Prey.AK::Monitor::PostCode+24EACE - F3 0F11 4B 04         - movss [rbx+04],xmm1
;Prey.AK::Monitor::PostCode+24EAD3 - F3 0F11 43 08         - movss [rbx+08],xmm0
;Prey.AK::Monitor::PostCode+24EAD8 - F3 0F10 53 10         - movss xmm2,[rbx+10]
;Prey.AK::Monitor::PostCode+24EADD - F3 0F10 43 0C         - movss xmm0,[rbx+0C]
; and
;Prey.AK::Monitor::PostCode+24A218 - F3 0F58 4F 6C         - addss xmm1,[rdi+6C]
;Prey.AK::Monitor::PostCode+24A21D - F3 0F58 47 70         - addss xmm0,[rdi+70]
;Prey.AK::Monitor::PostCode+24A222 - F3 0F11 57 1C         - movss [rdi+1C],xmm2			// coords
;Prey.AK::Monitor::PostCode+24A227 - F3 0F11 4F 20         - movss [rdi+20],xmm1
;Prey.AK::Monitor::PostCode+24A22C - F3 0F11 47 24         - movss [rdi+24],xmm0
;Prey.AK::Monitor::PostCode+24A231 - 4D 85 F6              - test r14,r14
;;---------------------------------------------------------------
.code


cameraStructInterceptor PROC
; Struct interceptor is also a write interceptor for coords/quaternion
;Prey.AK::Monitor::PostCode+13D1168 - F3 0F11 4E 0C         - movss [rsi+0C],xmm1		// quaternion						<< intercept here
;Prey.AK::Monitor::PostCode+13D116D - F3 0F11 56 10         - movss [rsi+10],xmm2
;Prey.AK::Monitor::PostCode+13D1172 - F3 0F11 5E 14         - movss [rsi+14],xmm3
;Prey.AK::Monitor::PostCode+13D1177 - F3 0F11 46 18         - movss [rsi+18],xmm0
;Prey.AK::Monitor::PostCode+13D117C - F3 0F11 3E            - movss [rsi],xmm7			// coords
;Prey.AK::Monitor::PostCode+13D1180 - F3 0F11 76 04         - movss [rsi+04],xmm6
;Prey.AK::Monitor::PostCode+13D1185 - F3 44 0F11 46 08      - movss [rsi+08],xmm8
;Prey.AK::Monitor::PostCode+13D118B - E8 503FD5FF           - call Prey.AK::Monitor::PostCode+11250E0						<< continue here
; This function intercepts the camera address and also blocks the writes by reading from our own two structs. 
	mov [g_cameraStructAddress], rsi
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movss dword ptr [rsi+0Ch],xmm1				; quaternion	 (x/y/z/w)
	movss dword ptr [rsi+10h],xmm2
	movss dword ptr [rsi+14h],xmm3
	movss dword ptr [rsi+18h],xmm0
	movss dword ptr [rsi],xmm7					; coords
	movss dword ptr [rsi+04h],xmm6
	movss dword ptr [rsi+08h],xmm8
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP


cameraWrite1Interceptor PROC
;Prey.AK::Monitor::PostCode+2494E0 - 8B 02                 - mov eax,[rdx]								<<< INTERCEPT HERE
;Prey.AK::Monitor::PostCode+2494E2 - 89 01                 - mov [rcx],eax				// coords			
;Prey.AK::Monitor::PostCode+2494E4 - 8B 42 04              - mov eax,[rdx+04]
;Prey.AK::Monitor::PostCode+2494E7 - 89 41 04              - mov [rcx+04],eax
;Prey.AK::Monitor::PostCode+2494EA - 8B 42 08              - mov eax,[rdx+08]
;Prey.AK::Monitor::PostCode+2494ED - 89 41 08              - mov [rcx+08],eax
;Prey.AK::Monitor::PostCode+2494F0 - 8B 42 0C              - mov eax,[rdx+0C]
;Prey.AK::Monitor::PostCode+2494F3 - 89 41 0C              - mov [rcx+0C],eax			// qx
;Prey.AK::Monitor::PostCode+2494F6 - 8B 42 10              - mov eax,[rdx+10]
;Prey.AK::Monitor::PostCode+2494F9 - 89 41 10              - mov [rcx+10],eax
;Prey.AK::Monitor::PostCode+2494FC - 8B 42 14              - mov eax,[rdx+14]
;Prey.AK::Monitor::PostCode+2494FF - 89 41 14              - mov [rcx+14],eax
;Prey.AK::Monitor::PostCode+249502 - 8B 42 18              - mov eax,[rdx+18]
;Prey.AK::Monitor::PostCode+249505 - 89 41 18              - mov [rcx+18],eax			// qw
;Prey.AK::Monitor::PostCode+249508 - 8B 42 1C              - mov eax,[rdx+1C]								<<<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	mov [rcx],eax	
	mov eax,[rdx+04h]
	mov [rcx+04h],eax
	mov eax,[rdx+08h]
	mov [rcx+08h],eax
	mov eax,[rdx+0Ch]
	mov [rcx+0Ch],eax
	mov eax,[rdx+10h]
	mov [rcx+10h],eax
	mov eax,[rdx+14h]
	mov [rcx+14h],eax
	mov eax,[rdx+18h]
	mov [rcx+18h],eax
exit:
	jmp qword ptr [_cameraWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite1Interceptor ENDP


cameraWrite2Interceptor PROC
; coord write intercept is broken up in two blocks as it has a relative read in the middle.... WHY is that statement placed at that spot!
;Prey.AK::Monitor::PostCode+24A38A - F3 44 0F58 68 04      - addss xmm13,[rax+04]			<< INTERCEPT HERE
;Prey.AK::Monitor::PostCode+24A390 - F3 44 0F58 20         - addss xmm12,[rax]
;Prey.AK::Monitor::PostCode+24A395 - F3 44 0F58 70 08      - addss xmm14,[rax+08]
;Prey.AK::Monitor::PostCode+24A39B - F3 44 0F11 67 1C      - movss [rdi+1C],xmm12			// write x
;Prey.AK::Monitor::PostCode+24A3A1 - F3 44 0F11 6F 20      - movss [rdi+20],xmm13			// write y
;Prey.AK::Monitor::PostCode+24A3A7 - F3 44 0F10 2D 08DAC401  - movss xmm13,[Prey.CreateAudioInputSource+51B6B8]		<< CONTINUE HERE
	addss xmm13, dword ptr [rax+04h]					; original statement
	addss xmm12, dword ptr [rax]
	addss xmm14, dword ptr [rax+08h]
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
	movss dword ptr [rdi+1Ch],xmm12
	movss dword ptr [rdi+20h],xmm13
exit:
	jmp qword ptr [_cameraWrite2InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite2Interceptor ENDP

cameraWrite3Interceptor PROC
;Prey.AK::Monitor::PostCode+24A3B0 - F3 44 0F11 77 24      - movss [rdi+24],xmm14			<< INTERCEPT HERE, write Z
;Prey.AK::Monitor::PostCode+24A3B6 - F3 0F10 8D 88000000   - movss xmm1,[rbp+00000088]
;Prey.AK::Monitor::PostCode+24A3BE - 48 8D 4F 1C           - lea rcx,[rdi+1C]
;Prey.AK::Monitor::PostCode+24A3C2 - E8 79410000           - call Prey.AK::Monitor::PostCode+24E540			<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1						; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je noWrite												; our own camera is enabled, just skip the writes
	movss dword ptr [rdi+24h],xmm14		
noWrite:
	movss xmm1, dword ptr [rbp+00000088h]
	lea rcx,[rdi+1Ch]
exit:
	jmp qword ptr [_cameraWrite3InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite3Interceptor ENDP


cameraWrite4Interceptor PROC
;Prey.AK::Monitor::PostCode+24EAC5 - F3 0F58 4B 04         - addss xmm1,[rbx+04]			<< INTERCEPT HERE
;Prey.AK::Monitor::PostCode+24EACA - F3 0F11 13            - movss [rbx],xmm2				; coords write	
;Prey.AK::Monitor::PostCode+24EACE - F3 0F11 4B 04         - movss [rbx+04],xmm1
;Prey.AK::Monitor::PostCode+24EAD3 - F3 0F11 43 08         - movss [rbx+08],xmm0
;Prey.AK::Monitor::PostCode+24EAD8 - F3 0F10 53 10         - movss xmm2,[rbx+10]			<< CONTINUE HERE
;Prey.AK::Monitor::PostCode+24EADD - F3 0F10 43 0C         - movss xmm0,[rbx+0C]
	addss xmm1, dword ptr [rbx+04h]
	cmp byte ptr [g_cameraEnabled], 1						; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit													; our own camera is enabled, just skip the writes
originalCode:
	movss dword ptr [rbx],xmm2	
	movss dword ptr [rbx+04h],xmm1
	movss dword ptr [rbx+08h],xmm0
exit:
	jmp qword ptr [_cameraWrite4InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite4Interceptor ENDP


cameraWrite5Interceptor PROC
;Prey.AK::Monitor::PostCode+24A218 - F3 0F58 4F 6C         - addss xmm1,[rdi+6C]			<< INTERCEPT HERE
;Prey.AK::Monitor::PostCode+24A21D - F3 0F58 47 70         - addss xmm0,[rdi+70]
;Prey.AK::Monitor::PostCode+24A222 - F3 0F11 57 1C         - movss [rdi+1C],xmm2			// coords
;Prey.AK::Monitor::PostCode+24A227 - F3 0F11 4F 20         - movss [rdi+20],xmm1
;Prey.AK::Monitor::PostCode+24A22C - F3 0F11 47 24         - movss [rdi+24],xmm0
;Prey.AK::Monitor::PostCode+24A231 - 4D 85 F6              - test r14,r14					<< CONTINUE HERE
	addss xmm1,dword ptr [rdi+6Ch]
	addss xmm0,dword ptr [rdi+70h]
	cmp byte ptr [g_cameraEnabled], 1						; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit													; our own camera is enabled, just skip the writes
originalCode:
	movss dword ptr [rdi+1Ch],xmm2
	movss dword ptr [rdi+20h],xmm1
	movss dword ptr [rdi+24h],xmm0
exit:
	jmp qword ptr [_cameraWrite5InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite5Interceptor ENDP


fovReadInterceptor PROC
;Prey.AK::Monitor::PostCode+11EB4CA - 48 8B 05 4F510E01     - mov rax,[Prey.AmdPowerXpressRequestHighPerformance+860E0] <<<< INTERCEP HERE
;Prey.AK::Monitor::PostCode+11EB4D1 - F3 0F10 53 0C         - movss xmm2,[rbx+0C]
;Prey.AK::Monitor::PostCode+11EB4D6 - 0F28 D8               - movaps xmm3,xmm0
;Prey.AK::Monitor::PostCode+11EB4D9 - F3 0F10 48 08         - movss xmm1,[rax+08]							<< Read FOV. 
;Prey.AK::Monitor::PostCode+11EB4DE - 0F2E CA               - ucomiss xmm1,xmm2
;Prey.AK::Monitor::PostCode+11EB4E1 - 75 09                 - jne Prey.AK::Monitor::PostCode+11EB4EC					<<< CONTINUE HERE
;Prey.AK::Monitor::PostCode+11EB4E3 - 0F57 C0               - xorps xmm0,xmm0
;Prey.AK::Monitor::PostCode+11EB4E6 - 48 83 C4 20           - add rsp,20 { 32 }
;Prey.AK::Monitor::PostCode+11EB4EA - 5B                    - pop rbx
;Prey.AK::Monitor::PostCode+11EB4EB - C3                    - ret 
	mov [g_fovConstructAddress], rax
originalCode:
	movss xmm2, dword ptr [rbx+0Ch]
	movaps xmm3,xmm0
	movss xmm1, dword ptr [rax+08h]
	ucomiss xmm1,xmm2
exit:
	jmp qword ptr [_fovReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
fovReadInterceptor ENDP

END