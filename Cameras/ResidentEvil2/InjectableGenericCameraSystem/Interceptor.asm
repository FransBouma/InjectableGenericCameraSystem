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
PUBLIC cameraWrite3Interceptor
PUBLIC cameraWrite4Interceptor
PUBLIC timestopReadInterceptor
PUBLIC resolutionScaleReadInterceptor
PUBLIC displayTypeInterceptor
PUBLIC dofSelectorWriteInterceptor

;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_cameraStructAddress: qword
EXTERN g_resolutionScaleAddress: qword
EXTERN g_timestopStructAddress: qword
EXTERN g_displayTypeStructAddress: qword
EXTERN g_dofStructAddress: qword

;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWrite1InterceptionContinue: qword
EXTERN _cameraWrite2InterceptionContinue: qword
EXTERN _cameraWrite3InterceptionContinue: qword
EXTERN _cameraWrite4InterceptionContinue: qword
EXTERN _timestopReadInterceptionContinue: qword
EXTERN _resolutionScaleReadInterceptionContinue: qword
EXTERN _displayTypeInterceptionContinue: qword
EXTERN _dofSelectorWriteInterceptionContinue: qword

.data

.code

cameraStructInterceptor PROC
; intercepts camera and also blocks FOV write 1
;re2.exe+AD0B445 - 75 13                 - jne re2.exe+AD0B45A
;re2.exe+AD0B447 - 45 31 C0              - xor r8d,r8d
;re2.exe+AD0B44A - 8D 50 38              - lea edx,[rax+38]
;re2.exe+AD0B44D - 48 89 D9              - mov rcx,rbx
;re2.exe+AD0B450 - E8 7B760CF7           - call re2.exe+1DD2AD0
;re2.exe+AD0B455 - E9 510A0000           - jmp re2.exe+AD0BEAB
;re2.exe+AD0B45A - F3 0F10 40 38         - movss xmm0,[rax+38]				<< INTERCEPT HERE
;re2.exe+AD0B45F - F3 0F11 82 A4000000   - movss [rdx+000000A4],xmm0					<< WRITE FOV
;re2.exe+AD0B467 - 48 83 79 18 00        - cmp qword ptr [rcx+18],00
;re2.exe+AD0B46C - 0F85 390A0000         - jne re2.exe+AD0BEAB				<< CONTINUE HERE
;re2.exe+AD0B472 - 48 8B 8A B8000000     - mov rcx,[rdx+000000B8]
;re2.exe+AD0B479 - 44 0F29 4C 24 50      - movaps [rsp+50],xmm9
;re2.exe+AD0B47F - 48 85 C9              - test rcx,rcx
;re2.exe+AD0B482 - 74 23                 - je re2.exe+AD0B4A7
	mov [g_cameraStructAddress], rdx
	movss xmm0, dword ptr [rax+38h]
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rdx+000000A4h],xmm0
exit:
	cmp qword ptr [rcx+18h],00h
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP

cameraWrite1Interceptor PROC
; fov write 2
;re2.exe+AD0B845 - 0F84 DA050000         - je re2.exe+AD0BE25
;re2.exe+AD0B84B - F3 0F10 78 30         - movss xmm7,[rax+30]						
;re2.exe+AD0B850 - F3 44 0F10 40 34      - movss xmm8,[rax+34]						<< INTERCEPT HERE
;re2.exe+AD0B856 - 8B 40 38              - mov eax,[rax+38]							
;re2.exe+AD0B859 - 89 87 A4000000        - mov [rdi+000000A4],eax					<< WRITE FOV
;re2.exe+AD0B85F - 48 8B 43 50           - mov rax,[rbx+50]							<< CONTINUE HERE
;re2.exe+AD0B863 - 48 8B 48 18           - mov rcx,[rax+18]
;re2.exe+AD0B867 - 48 85 C9              - test rcx,rcx						
;re2.exe+AD0B86A - 0F85 2D060000         - jne re2.exe+AD0BE9D
	movss xmm8, dword ptr [rax+34h]	
	mov eax,[rax+38h]
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	mov [rdi+000000A4h],eax
exit:
	jmp qword ptr [_cameraWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite1Interceptor ENDP

cameraWrite2Interceptor PROC
; fov write 3
;re2.exe+AD0B9EC - F3 0F5A C0            - cvtss2sd xmm0,xmm0
;re2.exe+AD0B9F0 - F2 0F59 05 483C10F9   - mulsd xmm0,[re2.exe+3E0F640]
;re2.exe+AD0B9F8 - F2 0F5A C0            - cvtsd2ss xmm0,xmm0							<< INTERCEPT HERE
;re2.exe+AD0B9FC - F3 0F11 87 A4000000   - movss [rdi+000000A4],xmm0					<< WRITE FOV
;re2.exe+AD0BA04 - 48 8B 43 50           - mov rax,[rbx+50]							
;re2.exe+AD0BA08 - 48 8B 48 18           - mov rcx,[rax+18]								<< CONTINUE HERE
;re2.exe+AD0BA0C - 48 85 C9              - test rcx,rcx
;re2.exe+AD0BA0F - 0F85 88040000         - jne re2.exe+AD0BE9D
	cvtsd2ss xmm0,xmm0		
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rdi+000000A4h],xmm0
exit:
	mov rax,[rbx+50h]
	jmp qword ptr [_cameraWrite2InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite2Interceptor ENDP

cameraWrite3Interceptor PROC
; coords write
;re2.exe+AD0B7D9 - 0F85 BE060000         - jne re2.exe+AD0BE9D
;re2.exe+AD0B7DF - F3 0F10 86 80000000   - movss xmm0,[rsi+00000080]
;re2.exe+AD0B7E7 - F3 0F10 8E 84000000   - movss xmm1,[rsi+00000084]
;re2.exe+AD0B7EF - F3 0F10 96 88000000   - movss xmm2,[rsi+00000088]
;re2.exe+AD0B7F7 - F3 0F11 87 80000000   - movss [rdi+00000080],xmm0				<< INTERCEPT HERE	<< X
;re2.exe+AD0B7FF - F3 0F11 8F 84000000   - movss [rdi+00000084],xmm1					<< Y
;re2.exe+AD0B807 - F3 0F11 97 88000000   - movss [rdi+00000088],xmm2					<< Z
;re2.exe+AD0B80F - 48 8B 43 50           - mov rax,[rbx+50]							<< CONTINUE HERE
;re2.exe+AD0B813 - 48 83 78 18 00        - cmp qword ptr [rax+18],00 { 0 }
;re2.exe+AD0B818 - 0F85 7F060000         - jne re2.exe+AD0BE9D
;re2.exe+AD0B81E - 0F10 86 90000000      - movups xmm0,[rsi+00000090]
;re2.exe+AD0B825 - 0F11 87 90000000      - movups [rdi+00000090],xmm0
;re2.exe+AD0B82C - 48 8B 43 50           - mov rax,[rbx+50]
;re2.exe+AD0B830 - 48 83 78 18 00        - cmp qword ptr [rax+18],00 { 0 }
;re2.exe+AD0B835 - 0F85 62060000         - jne re2.exe+AD0BE9D
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rdi+00000080h],xmm0
	movss dword ptr [rdi+00000084h],xmm1
	movss dword ptr [rdi+00000088h],xmm2
exit:
	jmp qword ptr [_cameraWrite3InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite3Interceptor ENDP

cameraWrite4Interceptor PROC
; quaternion write
;re2.exe+AD0B813 - 48 83 78 18 00        - cmp qword ptr [rax+18],00 { 0 }
;re2.exe+AD0B818 - 0F85 7F060000         - jne re2.exe+AD0BE9D
;re2.exe+AD0B81E - 0F10 86 90000000      - movups xmm0,[rsi+00000090]				<< INTERCEPT HERE
;re2.exe+AD0B825 - 0F11 87 90000000      - movups [rdi+00000090],xmm0				<< WRITE quaternion.
;re2.exe+AD0B82C - 48 8B 43 50           - mov rax,[rbx+50]							<< CONTINUE HERE
;re2.exe+AD0B830 - 48 83 78 18 00        - cmp qword ptr [rax+18],00 { 0 }
;re2.exe+AD0B835 - 0F85 62060000         - jne re2.exe+AD0BE9D
;re2.exe+AD0B83B - 48 8B 86 A8000000     - mov rax,[rsi+000000A8]
;re2.exe+AD0B842 - 48 85 C0              - test rax,rax
;re2.exe+AD0B845 - 0F84 DA050000         - je re2.exe+AD0BE25
;re2.exe+AD0B84B - F3 0F10 78 30         - movss xmm7,[rax+30]
;re2.exe+AD0B850 - F3 44 0F10 40 34      - movss xmm8,[rax+34]
;re2.exe+AD0B856 - 8B 40 38              - mov eax,[rax+38]
;re2.exe+AD0B859 - 89 87 A4000000        - mov [rdi+000000A4],eax
	movups xmm0, xmmword ptr [rsi+00000090h]
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movups [rdi+00000090h],xmm0
exit:
	jmp qword ptr [_cameraWrite4InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite4Interceptor ENDP


resolutionScaleReadInterceptor PROC
;re2.exe+10026BCF - E8 BCA92EF2           - call re2.exe+2311590
;re2.exe+10026BD4 - 48 8B 05 355605F7     - mov rax,[re2.exe+707C210] { (17F58950) }
;re2.exe+10026BDB - 48 8D 4C 24 58        - lea rcx,[rsp+58]
;re2.exe+10026BE0 - C7 44 24 58 CDCCCC3D  - mov [rsp+58],3DCCCCCD { (0) }
;re2.exe+10026BE8 - F3 0F10 80 CC110000   - movss xmm0,[rax+000011CC]				<< INTERCEPT HERE << READ resolution scaling.
;re2.exe+10026BF0 - 48 8D 44 24 54        - lea rax,[rsp+54]
;re2.exe+10026BF5 - F3 41 0F59 46 40      - mulss xmm0,[r14+40]
;re2.exe+10026BFB - 0F2F C7               - comiss xmm0,xmm7						<< CONTINUE HERE
;re2.exe+10026BFE - F3 0F11 44 24 54      - movss [rsp+54],xmm0
;re2.exe+10026C04 - 48 0F46 C1            - cmovbe rax,rcx
;re2.exe+10026C08 - F3 0F10 08            - movss xmm1,[rax]
;re2.exe+10026C0C - 0F2F F1               - comiss xmm6,xmm1
originalCode:
	mov [g_resolutionScaleAddress], rax
	movss xmm0, dword ptr [rax+000011CCh]
	lea rax,[rsp+54h]
	mulss xmm0, dword ptr [r14+40h]
exit:
	jmp qword ptr [_resolutionScaleReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
resolutionScaleReadInterceptor ENDP


timestopReadInterceptor PROC
;re2.exe+EF6E35A - 0F57 C0               - xorps xmm0,xmm0
;re2.exe+EF6E35D - 89 C1                 - mov ecx,eax
;re2.exe+EF6E35F - F3 48 0F2A C1         - cvtsi2ss xmm0,rcx
;re2.exe+EF6E364 - F3 0F58 C1            - addss xmm0,xmm1
;re2.exe+EF6E368 - F3 0F11 87 88030000   - movss [rdi+00000388],xmm0
;re2.exe+EF6E370 - C7 87 84030000 0000803F - mov [rdi+00000384],3F800000 { (0) }
;re2.exe+EF6E37A - F3 0F10 97 A0030000   - movss xmm2,[rdi+000003A0]
;re2.exe+EF6E382 - F3 0F10 87 84030000   - movss xmm0,[rdi+00000384]					<< INTERCEPT HERE
;re2.exe+EF6E38A - 0F2F C2               - comiss xmm0,xmm2
;re2.exe+EF6E38D - F3 0F10 9F 80030000   - movss xmm3,[rdi+00000380]					<< READ from gamespeed: SET TO 1.0 for normal procedure, 0.00001 otherwise.
;re2.exe+EF6E395 - 76 03                 - jna re2.exe+EF6E39A							<< CONTINUE HERE
;re2.exe+EF6E397 - 0F28 C2               - movaps xmm0,xmm2
;re2.exe+EF6E39A - F3 0F59 C3            - mulss xmm0,xmm3
;re2.exe+EF6E39E - 0F2F 87 A4030000      - comiss xmm0,[rdi+000003A4]
;re2.exe+EF6E3A5 - F3 0F11 87 84030000   - movss [rdi+00000384],xmm0
	mov [g_timestopStructAddress], rdi
	movss xmm0, dword ptr [rdi+00000384h]
	comiss xmm0,xmm2
	movss xmm3, dword ptr [rdi+00000380h]
exit:
	jmp qword ptr [_timestopReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
timestopReadInterceptor ENDP

displayTypeInterceptor PROC
;re2.exe+EF9BE37 - 72 06                 - jb re2.exe+EF9BE3F
;re2.exe+EF9BE39 - F3 44 0F11 4B 4C      - movss [rbx+4C],xmm9
;re2.exe+EF9BE3F - F3 44 0F10 15 4471E7F4  - movss xmm10,[re2.exe+3E12F8C] { (2270.00) }
;re2.exe+EF9BE48 - 48 8D 45 10           - lea rax,[rbp+10]
;re2.exe+EF9BE4C - 48 89 45 18           - mov [rbp+18],rax
;re2.exe+EF9BE50 - 48 8D 3D A94106F1     - lea rdi,[re2.exe] { (9460301) }
;re2.exe+EF9BE57 - 8B 43 74              - mov eax,[rbx+74]							<< READ camera type (set to 0 for fit).
;re2.exe+EF9BE5A - FF C8                 - dec eax
;re2.exe+EF9BE5C - 83 F8 11              - cmp eax,11 { 17 }
;re2.exe+EF9BE5F - 0F87 3B010000         - ja re2.exe+EF9BFA0
;re2.exe+EF9BE65 - 48 98                 - cdqe 
;re2.exe+EF9BE67 - 8B 8C 87 1CF9DD01     - mov ecx,[rdi+rax*4+01DDF91C]
;
; We'll intercept a bit higher in the function as a RIP value is in the way, the RBX register is the same value:
;re2.exe+EF9BD9D - 48 8B 49 10           - mov rcx,[rcx+10]
;re2.exe+EF9BDA1 - F3 0F10 0D 23F31CF8   - movss xmm1,[re2.exe+716B0CC] { (0.00) }
;re2.exe+EF9BDA9 - 44 0F29 44 24 40      - movaps [rsp+40],xmm8				<< INTERCEPT HERE
;re2.exe+EF9BDAF - 44 0F29 4C 24 30      - movaps [rsp+30],xmm9
;re2.exe+EF9BDB5 - F3 0F11 45 10         - movss [rbp+10],xmm0
;re2.exe+EF9BDBA - F3 0F11 4D 14         - movss [rbp+14],xmm1				<< CONTINUE HERE
;re2.exe+EF9BDBF - 44 0F29 54 24 20      - movaps [rsp+20],xmm10
;re2.exe+EF9BDC5 - 48 85 C9              - test rcx,rcx
;re2.exe+EF9BDC8 - 74 20                 - je re2.exe+EF9BDEA
	mov [g_displayTypeStructAddress], rbx
	movaps xmmword ptr [rsp+40h],xmm8
	movaps xmmword ptr [rsp+30h],xmm9
	movss dword ptr [rbp+10h],xmm0
exit:
	jmp qword ptr [_displayTypeInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
displayTypeInterceptor ENDP


dofSelectorWriteInterceptor PROC
; use the whole block.
;re2.exe+102BC830 - 89 51 4C              - mov [rcx+4C],edx							<< INTERCEPT HERE << DOF type write. Set to 1.
;re2.exe+102BC833 - 85 D2                 - test edx,edx
;re2.exe+102BC835 - 74 0E                 - je re2.exe+102BC845
;re2.exe+102BC837 - 83 EA 01              - sub edx,01 { 1 }
;re2.exe+102BC83A - 74 09                 - je re2.exe+102BC845
;re2.exe+102BC83C - 83 FA 01              - cmp edx,01 { 1 }
;re2.exe+102BC83F - 75 08                 - jne re2.exe+102BC849
;re2.exe+102BC841 - 88 51 50              - mov [rcx+50],dl
;re2.exe+102BC844 - C3                    - ret 
;re2.exe+102BC845 - C6 41 50 00           - mov byte ptr [rcx+50],00 { 0 }
;re2.exe+102BC849 - C3                    - ret											<< CONTINUE HERE
	mov [g_dofStructAddress], rcx
	cmp byte ptr [g_cameraEnabled], 1
	je noWrite
	mov [rcx+4Ch],edx				
noWrite:
	test edx,edx
	je setToZero
	sub edx,01h
	je setToZero
	cmp edx,01h
	jne exit
	mov [rcx+50h],dl
	jmp exit
setToZero:
	mov byte ptr [rcx+50h],00
exit:
	jmp qword ptr [_dofSelectorWriteInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
dofSelectorWriteInterceptor ENDP

END