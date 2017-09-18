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
PUBLIC timestopReadInterceptor
PUBLIC dofStructInterceptor
PUBLIC dofWriteInterceptor
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
EXTERN g_dofStructAddress: qword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraCutsceneStructInterceptionContinue: qword
EXTERN _cameraCutsceneWrite1InterceptionContinue: qword
EXTERN _fovWriteInterceptionContinue: qword
EXTERN _gamespeedInterceptionContinue: qword
EXTERN _timestopInterceptionContinue: qword
EXTERN _dofStructInterceptionContinue: qword
EXTERN _dofWriteInterceptionContinue: qword

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
	cmp byte ptr [g_cameraEnabled], 1					
	je exit
originalCode:
	movss dword ptr [rcx+0Ch],xmm1		
exit:
	mov eax,[rsi+00000224h]	
	jmp qword ptr [_fovWriteInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
fovWriteInterceptor ENDP


gamespeedWriteInterceptor PROC
;142EB3833 - F3 0F11 7F 08         - movss [rdi+08],xmm7				<< INTERCEPT HERE
;142EB3838 - F3 0F11 77 0C         - movss [rdi+0C],xmm6				// write gamespeed. 1.0 is normal speed. 0.00001 is almost paused. Only works in gameplay.
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


dofStructInterceptor PROC
;1432A5F3F - 0F28 48 20            - movaps xmm1,[rax+20]
;1432A5F43 - 0F29 8F 90000000      - movaps [rdi+00000090],xmm1
;1432A5F4A - 0F28 40 30            - movaps xmm0,[rax+30]
;1432A5F4E - 0F29 87 A0000000      - movaps [rdi+000000A0],xmm0
;1432A5F55 - F3 41 0F10 4E 38      - movss xmm1,[r14+38]					<< INTERCEPT HERE
;1432A5F5B - F3 0F11 8F 08010000   - movss [rdi+00000108],xmm1				<< Distance write.
;1432A5F63 - F3 41 0F10 56 4C      - movss xmm2,[r14+4C]					<< CONTINUE HERE
;1432A5F69 - F3 41 0F10 46 48      - movss xmm0,[r14+48]
;1432A5F6F - F3 0F11 87 68010000   - movss [rdi+00000168],xmm0
;1432A5F77 - F3 0F11 97 6C010000   - movss [rdi+0000016C],xmm2
;1432A5F7F - 4C 8D 4D 07           - lea r9,[rbp+07]
;1432A5F83 - 4C 8D 45 17           - lea r8,[rbp+17]
	mov [g_dofStructAddress], rdi
	movss xmm1, dword ptr [r14+38h]
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rdi+00000108h],xmm1
exit:
	jmp qword ptr [_dofStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
dofStructInterceptor ENDP


dofWriteInterceptor PROC
;1432A5FA0 - F3 0F10 4D 17         - movss xmm1,[rbp+17]					<< INTERCEPT HERE
;1432A5FA5 - F3 0F11 8F 10010000   - movss [rdi+00000110],xmm1				<< focal length (FoV)
;1432A5FAD - F3 0F10 45 07         - movss xmm0,[rbp+07]
;1432A5FB2 - F3 0F11 87 14010000   - movss [rdi+00000114],xmm0				
;1432A5FBA - F3 41 0F10 4E 44      - movss xmm1,[r14+44]					
;1432A5FC0 - F3 0F11 8F 24010000   - movss [rdi+00000124],xmm1				<< Aperture write.	(0.5-32)
;1432A5FC8 - F3 41 0F10 46 50      - movss xmm0,[r14+50]					<< CONTINUE HERE
;1432A5FCE - F3 0F11 87 28010000   - movss [rdi+00000128],xmm0
;1432A5FD6 - 41 80 BE E7000000 00  - cmp byte ptr [r14+000000E7],00 { 0 }
	cmp byte ptr [g_cameraEnabled], 1
	jne originalCode
noWrites:
	movss xmm1,dword ptr [rbp+17h]
	movss xmm0, dword ptr [rbp+07h]
	movss xmm1, dword ptr [r14+44h]		
	jmp exit
originalCode:
	movss xmm1,dword ptr [rbp+17h]
	movss dword ptr [rdi+00000110h],xmm1
	movss xmm0, dword ptr [rbp+07h]
	movss dword ptr [rdi+00000114h],xmm0
	movss xmm1, dword ptr [r14+44h]		
	movss dword ptr [rdi+00000124h],xmm1
exit:
	jmp qword ptr [_dofWriteInterceptionContinue]
dofWriteInterceptor ENDP

END