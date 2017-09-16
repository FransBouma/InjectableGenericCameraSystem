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
PUBLIC cameraCutsceneStructInterceptor
PUBLIC cameraCutsceneWrite1Interceptor
PUBLIC fovWriteInterceptor
PUBLIC gamespeedWriteInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_gamePaused: byte
EXTERN g_cameraStructAddress: qword
EXTERN g_cameraCutsceneStructAddress: qword
EXTERN g_timestopStructAddress: qword
EXTERN g_gamespeedStructAddress: qword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraCutsceneStructInterceptionContinue: qword
EXTERN _cameraCutsceneWrite1InterceptionContinue: qword
EXTERN _fovWriteInterceptionContinue: qword
EXTERN _gamespeedInterceptionContinue: qword

.data
;---------------------------------------------------------------
; Scratch pad
.code


cameraStructInterceptor PROC
;mgsvtpp.exe+32BE6C0 - 0F28 02               - movaps xmm0,[rdx]				<< INTERCEPT HERE
;mgsvtpp.exe+32BE6C3 - 0F29 81 F0000000      - movaps [rcx+000000F0],xmm0		// quaternion
;mgsvtpp.exe+32BE6CA - 0F28 4A 10            - movaps xmm1,[rdx+10]
;mgsvtpp.exe+32BE6CE - 0F29 89 00010000      - movaps [rcx+00000100],xmm1		// coords
;mgsvtpp.exe+32BE6D5 - C3                    - ret								<< CONTINUE HERE
;
; We'll also block writes when the camera is enabled. 
	mov [g_cameraStructAddress], rcx
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
noWrites:
	; we'll keep the reads, as it's not known if caller wants to do something with them...
	movaps xmm0, xmmword ptr [rdx]
	movaps xmm1, xmmword ptr [rdx+10h]
	jmp exit
originalCode:
	movaps xmm0, xmmword ptr [rdx]
	movaps xmmword ptr [rcx+000000F0h],xmm0
	movaps xmm1, xmmword ptr [rdx+10h]
	movaps xmmword ptr [rcx+00000100h],xmm1
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP


cameraCutsceneStructInterceptor PROC
;14329CDF7 - 44 0F28 9C 24 C0000000  - movaps xmm11,[rsp+000000C0]
;14329CE00 - 44 0F28 8C 24 E0000000  - movaps xmm9,[rsp+000000E0]
;14329CE09 - 44 0F28 84 24 F0000000  - movaps xmm8,[rsp+000000F0]
;14329CE12 - 0F29 4C 24 30         - movaps [rsp+30],xmm1
;14329CE17 - 0F28 40 10            - movaps xmm0,[rax+10]
;14329CE1B - 48 8B 83 80000000     - mov rax,[rbx+00000080]
;14329CE22 - 0F29 44 24 40         - movaps [rsp+40],xmm0
;14329CE27 - 0F29 88 20010000      - movaps [rax+00000120],xmm1			<< INTERCEPT HERE// quaternion write
;14329CE2E - 0F28 44 24 40         - movaps xmm0,[rsp+40]
;14329CE33 - 0F29 80 30010000      - movaps [rax+00000130],xmm0			// coords write.
;14329CE3A - 41 8B 46 60           - mov eax,[r14+60]					<< CONTINUE HERE
;
; We'll also block writes when the camera is enabled. 
	mov [g_cameraCutsceneStructAddress], rax
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
noWrites:
	; we'll keep the reads, as it's not known if caller wants to do something with them...
	movaps xmm0, xmmword ptr [rsp+40h]
	jmp exit
originalCode:
	movaps xmmword ptr [rax+00000120h],xmm1
	movaps xmm0, xmmword ptr [rsp+40h]
	movaps xmmword ptr [rax+00000130h],xmm0
exit:
	jmp qword ptr [_cameraCutsceneStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraCutsceneStructInterceptor ENDP

cameraCutsceneWrite1Interceptor PROC
;14329D7BB - 0F28 4C 24 50         - movaps xmm1,[rsp+50]
;14329D7C0 - 0F5C D7               - subps xmm2,xmm7
;14329D7C3 - 0F55 C2               - andnps xmm0,xmm2
;14329D7C6 - 0F56 C6               - orps xmm0,xmm6
;14329D7C9 - 0F29 80 20010000      - movaps [rax+00000120],xmm0			<< INTERCEPT HERE// quaternion
;14329D7D0 - 0F29 88 30010000      - movaps [rax+00000130],xmm1			// coords
;14329D7D7 - 48 8B 81 80000000     - mov rax,[rcx+00000080]
;14329D7DE - F3 0F10 05 5AA38DFF   - movss xmm0,[142B77B40]				<< CONTINUE HERE
;14329D7E6 - F3 0F58 40 3C         - addss xmm0,[rax+3C]
;14329D7EB - F3 0F11 40 3C         - movss [rax+3C],xmm0
;14329D7F0 - 0FB6 05 29A38DFF      - movzx eax,byte ptr [142B77B20]
;14329D7F7 - A8 01                 - test al,01 { 1 }
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit
originalCode:
	movaps xmmword ptr [rax+00000120h],xmm0	
	movaps xmmword ptr [rax+00000130h],xmm1	
exit:
	mov rax,[rcx+00000080h]
	jmp qword ptr [_cameraCutsceneWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraCutsceneWrite1Interceptor ENDP


fovWriteInterceptor PROC
;mgsvtpp.exe+403C121 - F3 0F59 8E F8020000   - mulss xmm1,[rsi+000002F8]
;mgsvtpp.exe+403C129 - F3 0F10 86 C8050000   - movss xmm0,[rsi+000005C8]
;mgsvtpp.exe+403C131 - 48 8B 8E 80030000     - mov rcx,[rsi+00000380]
;mgsvtpp.exe+403C138 - F3 0F5C C8            - subss xmm1,xmm0
;mgsvtpp.exe+403C13C - F3 0F59 8E B8050000   - mulss xmm1,[rsi+000005B8]		<< INTERCEPT HERE
;mgsvtpp.exe+403C144 - F3 0F58 C8            - addss xmm1,xmm0
;mgsvtpp.exe+403C148 - F3 0F11 49 0C         - movss [rcx+0C],xmm1				// FOV write. 
;mgsvtpp.exe+403C14D - 8B 86 24020000        - mov eax,[rsi+00000224]			<< CONTINUE HERE
;mgsvtpp.exe+403C153 - C1 E8 08              - shr eax,08				
;mgsvtpp.exe+403C156 - A8 01                 - test al,01
	mulss xmm1, dword ptr [rsi+000005B8h]
	addss xmm1,xmm0
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit
originalCode:
	movss dword ptr [rcx+0Ch],xmm1		
exit:
	mov eax,[rsi+00000224h]	
	jmp qword ptr [_fovWriteInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
fovWriteInterceptor ENDP


gamespeedWriteInterceptor PROC
;142EB3833 - F3 0F11 7F 08         - movss [rdi+08],xmm7				<< INTERCEPT HERE
;142EB3838 - F3 0F11 77 0C         - movss [rdi+0C],xmm6				// write gamespeed. 1.0 is normal speed. 0.00001 is almost paused.
;142EB383D - 48 8B 47 28           - mov rax,[rdi+28]
;142EB3841 - 48 85 C0              - test rax,rax						<< CONTINUE HERE
;142EB3844 - 74 42                 - je 142EB3888
;142EB3846 - F3 0F10 48 0C         - movss xmm1,[rax+0C]	
;142EB384B - F3 0F10 40 08         - movss xmm0,[rax+08]	
;142EB3850 - 48 8D 4C 24 50        - lea rcx,[rsp+50]
;142EB3855 - 0F5A C9               - cvtps2pd xmm1,xmm1
;142EB3858 - 0F5A C0               - cvtps2pd xmm0,xmm0
;142EB385B - F2 0F59 C8            - mulsd xmm1,xmm0
;142EB385F - E8 0C55FAFF           - call 142E58D70
	mov [g_gamespeedStructAddress], rdi
	movss dword ptr [rdi+08h],xmm7
	cmp byte ptr [g_gamePaused], 1
	je exit
originalCode:
	movss dword ptr [rdi+0Ch],xmm6					; write gamespeed.
exit:
	mov rax,[rdi+28h]
	jmp qword ptr [_gamespeedInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
gamespeedWriteInterceptor ENDP


END