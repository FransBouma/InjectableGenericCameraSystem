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
PUBLIC timestopInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_cameraStructAddress: qword
EXTERN g_timestopStructAddress: qword
EXTERN g_fovStructAddress: qword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWrite1InterceptionContinue: qword
EXTERN _cameraWrite2InterceptionContinue: qword
EXTERN _cameraWrite3InterceptionContinue: qword
EXTERN _timestopInterceptionContinue: qword
EXTERN _fovReadInterceptionContinue: qword

.data
;---------------------------------------------------------------
; Scratch pad
.code


cameraStructInterceptor PROC
;Homefront2_Release.exe+AD5D7D - F3 0F10 4B 0C         - movss xmm1,[rbx+0C]				<< INTERCEPT HERE << Z READ
;Homefront2_Release.exe+AD5D82 - F3 0F10 43 1C         - movss xmm0,[rbx+1C]
;Homefront2_Release.exe+AD5D87 - F3 0F11 4D 24         - movss [rbp+24],xmm1
;Homefront2_Release.exe+AD5D8C - F3 0F10 4B 2C         - movss xmm1,[rbx+2C]
;Homefront2_Release.exe+AD5D91 - F3 0F11 45 28         - movss [rbp+28],xmm0				<< CONTINUE HERE
;Homefront2_Release.exe+AD5D96 - 8B 45 24              - mov eax,[rbp+24]
;Homefront2_Release.exe+AD5D99 - 89 87 2C010000        - mov [rdi+0000012C],eax
;Homefront2_Release.exe+AD5D9F - 8B 45 28              - mov eax,[rbp+28]
	mov [g_cameraStructAddress], rbx
originalCode:
	movss xmm1, dword ptr [rbx+0Ch]
	movss xmm0, dword ptr [rbx+1Ch]
	movss dword ptr [rbp+24h],xmm1
	movss xmm1, dword ptr [rbx+2Ch]
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP


cameraWrite1Interceptor PROC
;Homefront2_Release.exe+9497BF - F3 41 0F5C C0         - subss xmm0,xmm8
;Homefront2_Release.exe+9497C4 - 0F28 D4               - movaps xmm2,xmm4
;Homefront2_Release.exe+9497C7 - F3 0F58 D7            - addss xmm2,xmm7							
;Homefront2_Release.exe+9497CB - F3 0F11 43 18         - movss [rbx+18],xmm0						<< INTERCEPT HERE << MATRIX VALUE WRITE
;Homefront2_Release.exe+9497D0 - F3 0F10 44 24 40      - movss xmm0,[rsp+40]
;Homefront2_Release.exe+9497D6 - F3 0F11 53 08         - movss [rbx+08],xmm2
;Homefront2_Release.exe+9497DB - F3 0F10 54 24 38      - movss xmm2,[rsp+38]
;Homefront2_Release.exe+9497E1 - F3 0F5C E7            - subss xmm4,xmm7
;Homefront2_Release.exe+9497E5 - F3 41 0F58 F0         - addss xmm6,xmm8							
;Homefront2_Release.exe+9497EA - F3 0F11 53 1C         - movss [rbx+1C],xmm2						<<< WRITE Y
;Homefront2_Release.exe+9497EF - F3 0F11 2B            - movss [rbx],xmm5
;Homefront2_Release.exe+9497F3 - F3 0F11 5B 04         - movss [rbx+04],xmm3						<<< MATRIX VALUES
;Homefront2_Release.exe+9497F8 - F3 44 0F11 4B 10      - movss [rbx+10],xmm9
;Homefront2_Release.exe+9497FE - F3 0F11 4B 14         - movss [rbx+14],xmm1
;Homefront2_Release.exe+949803 - F3 0F11 63 20         - movss [rbx+20],xmm4
;Homefront2_Release.exe+949808 - F3 0F11 73 24         - movss [rbx+24],xmm6
;Homefront2_Release.exe+94980D - F3 44 0F11 53 28      - movss [rbx+28],xmm10
;Homefront2_Release.exe+949813 - F3 0F11 43 2C         - movss [rbx+2C],xmm0						<< WRITE Z
;Homefront2_Release.exe+949818 - E8 63BF75FF           - call Homefront2_Release.exe+A5780			<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
noWrites:
	movss xmm0,dword ptr [rsp+40h]
	movss xmm2,dword ptr [rsp+38h]
	subss xmm4,xmm7
	addss xmm6,xmm8		
	jmp exit
originalCode:
	movss dword ptr [rbx+18h],xmm0	
	movss xmm0,dword ptr [rsp+40h]
	movss dword ptr [rbx+08h],xmm2
	movss xmm2,dword ptr [rsp+38h]
	subss xmm4,xmm7
	addss xmm6,xmm8		
	movss dword ptr [rbx+1Ch],xmm2	
	movss dword ptr [rbx],xmm5
	movss dword ptr [rbx+04h],xmm3	
	movss dword ptr [rbx+10h],xmm9
	movss dword ptr [rbx+14h],xmm1
	movss dword ptr [rbx+20h],xmm4
	movss dword ptr [rbx+24h],xmm6
	movss dword ptr [rbx+28h],xmm10
	movss dword ptr [rbx+2Ch],xmm0	
exit:
	jmp qword ptr [_cameraWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite1Interceptor ENDP


cameraWrite2Interceptor PROC
; This is a generic copy routine and it is used a LOT. We can't simply disable things here as nothing will work then. 
; So we only disable the writes if rcx is equal to the camera struct. 
;Homefront2_Release.exe+72D90 - 8B 02                 - mov eax,[rdx]					<< INTERCEPT HERE << WRITE matrix _11
;Homefront2_Release.exe+72D92 - 89 01                 - mov [rcx],eax
;Homefront2_Release.exe+72D94 - 8B 42 04              - mov eax,[rdx+04]
;Homefront2_Release.exe+72D97 - 89 41 04              - mov [rcx+04],eax
;Homefront2_Release.exe+72D9A - 8B 42 08              - mov eax,[rdx+08]
;Homefront2_Release.exe+72D9D - 89 41 08              - mov [rcx+08],eax
;Homefront2_Release.exe+72DA0 - 8B 42 0C              - mov eax,[rdx+0C]				<< WRITE X
;Homefront2_Release.exe+72DA3 - 89 41 0C              - mov [rcx+0C],eax
;Homefront2_Release.exe+72DA6 - 8B 42 10              - mov eax,[rdx+10]
;Homefront2_Release.exe+72DA9 - 89 41 10              - mov [rcx+10],eax
;Homefront2_Release.exe+72DAC - 8B 42 14              - mov eax,[rdx+14]
;Homefront2_Release.exe+72DAF - 89 41 14              - mov [rcx+14],eax
;Homefront2_Release.exe+72DB2 - 8B 42 18              - mov eax,[rdx+18]
;Homefront2_Release.exe+72DB5 - 89 41 18              - mov [rcx+18],eax
;Homefront2_Release.exe+72DB8 - 8B 42 1C              - mov eax,[rdx+1C]
;Homefront2_Release.exe+72DBB - 89 41 1C              - mov [rcx+1C],eax
;Homefront2_Release.exe+72DBE - 8B 42 20              - mov eax,[rdx+20]
;Homefront2_Release.exe+72DC1 - 89 41 20              - mov [rcx+20],eax
;Homefront2_Release.exe+72DC4 - 8B 42 24              - mov eax,[rdx+24]
;Homefront2_Release.exe+72DC7 - 89 41 24              - mov [rcx+24],eax
;Homefront2_Release.exe+72DCA - 8B 42 28              - mov eax,[rdx+28]
;Homefront2_Release.exe+72DCD - 89 41 28              - mov [rcx+28],eax
;Homefront2_Release.exe+72DD0 - 8B 42 2C              - mov eax,[rdx+2C]
;Homefront2_Release.exe+72DD3 - 89 41 2C              - mov [rcx+2C],eax				<< WRITE Z
;Homefront2_Release.exe+72DD6 - 8B 42 30              - mov eax,[rdx+30]				<< CONTINUE HERE	
;Homefront2_Release.exe+72DD9 - 89 41 30              - mov [rcx+30],eax				<< WRITE FOV
;Homefront2_Release.exe+72DDC - 8B 42 34              - mov eax,[rdx+34]
;Homefront2_Release.exe+72DDF - 89 41 34              - mov [rcx+34],eax				
	cmp byte ptr [g_cameraEnabled], 1						; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode
	cmp qword ptr rcx, [g_cameraStructAddress]
	je exit
originalCode:
	mov eax,[rdx]	
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
exit:
	jmp qword ptr [_cameraWrite2InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite2Interceptor ENDP

cameraWrite3Interceptor PROC
;Homefront2_Release.exe+949701 - 44 0F28 D2            - movaps xmm10,xmm2
;Homefront2_Release.exe+949705 - F3 0F11 44 24 38      - movss [rsp+38],xmm0
;Homefront2_Release.exe+94970B - F3 0F10 45 A0         - movss xmm0,[rbp-60]
;Homefront2_Release.exe+949710 - F3 0F11 4C 24 3C      - movss [rsp+3C],xmm1					<< INTERCEPT HERE
;Homefront2_Release.exe+949716 - F3 0F58 C3            - addss xmm0,xmm3
;Homefront2_Release.exe+94971A - F3 0F11 5C 24 20      - movss [rsp+20],xmm3
;Homefront2_Release.exe+949720 - F3 44 0F11 5B 0C      - movss [rbx+0C],xmm11					<< X WRITE
;Homefront2_Release.exe+949726 - F3 0F11 44 24 40      - movss [rsp+40],xmm0					<< CONTINUE HERE
;Homefront2_Release.exe+94972C - F3 0F10 05 802E6601   - movss xmm0,[Homefront2_Release.exe+1FAC5B4] { [2.00] }
	movss dword ptr [rsp+3Ch],xmm1
	addss xmm0,xmm3
	movss dword ptr [rsp+20h],xmm3
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movss dword ptr [rbx+0Ch],xmm11								; X WRITE
exit:
	jmp qword ptr [_cameraWrite3InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite3Interceptor ENDP


timestopInterceptor PROC
;Homefront2_Release.exe+D5F221 - 44 0F28 54 24 50      - movaps xmm10,[rsp+50]									
;Homefront2_Release.exe+D5F227 - 44 0F28 4C 24 60      - movaps xmm9,[rsp+60]
;Homefront2_Release.exe+D5F22D - 80 BF 34020000 00     - cmp byte ptr [rdi+00000234],00 { 0 }					<< TIMESTOP READ. Has Jump to this address, so can't intercept. 
;Homefront2_Release.exe+D5F234 - F2 0F10 15 34D42401   - movsd xmm2,[Homefront2_Release.exe+1FAC670] { [1.00] } 
;...
;Homefront2_Release.exe+D5F248 - 66 0FEF C0            - pxor xmm0,xmm0					<< INTERCEPT HERE. rdi has same value.
;Homefront2_Release.exe+D5F24C - F2 48 0F2A 47 40      - cvtsi2sd xmm0,[rdi+40]
;Homefront2_Release.exe+D5F252 - F2 0F5E D0            - divsd xmm2,xmm0
;Homefront2_Release.exe+D5F256 - 66 0FEF C0            - pxor xmm0,xmm0
;Homefront2_Release.exe+D5F25A - F2 48 0F2A C1         - cvtsi2sd xmm0,rcx
;Homefront2_Release.exe+D5F25F - F2 0F59 C2            - mulsd xmm0,xmm2				<< CONTINUE HERE
;Homefront2_Release.exe+D5F263 - F2 0F5A C8            - cvtsd2ss xmm1,xmm0
;Homefront2_Release.exe+D5F267 - F3 0F59 CB            - mulss xmm1,xmm3
	mov [g_timestopStructAddress], rdi
originalCode:
	pxor xmm0,xmm0                     
	cvtsi2sd xmm0,qword ptr [rdi+40h]
	divsd xmm2,xmm0                    
	pxor xmm0,xmm0                     
	cvtsi2sd xmm0,rcx                  
exit:
	jmp qword ptr [_timestopInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
timestopInterceptor ENDP

fovReadInterceptor PROC
;Homefront2_Release.exe+90C3D8 - F3 0F10 50 1C         - movss xmm2,[rax+1C]
;Homefront2_Release.exe+90C3DD - F3 0F10 78 30         - movss xmm7,[rax+30]
;Homefront2_Release.exe+90C3E2 - 33 DB                 - xor ebx,ebx
;Homefront2_Release.exe+90C3E4 - 44 8B FB              - mov r15d,ebx
;Homefront2_Release.exe+90C3E7 - F3 0F10 0A            - movss xmm1,[rdx]							<<< INTERCEPT HERE <<< FOV READ
;Homefront2_Release.exe+90C3EB - F3 0F11 44 24 20      - movss [rsp+20],xmm0
;Homefront2_Release.exe+90C3F1 - F3 0F10 40 2C         - movss xmm0,[rax+2C]						
;Homefront2_Release.exe+90C3F6 - 8B C3                 - mov eax,ebx								<< CONTINUE HERE
;Homefront2_Release.exe+90C3F8 - F3 0F59 0D 242A6A01   - mulss xmm1,[Homefront2_Release.exe+1FAEE24]
	mov [g_fovStructAddress], rdx
originalCode:
	movss xmm1, dword ptr [rdx]	
	movss dword ptr [rsp+20h],xmm0
	movss xmm0, dword ptr [rax+2Ch]
exit:
	jmp qword ptr [_fovReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
fovReadInterceptor ENDP

END