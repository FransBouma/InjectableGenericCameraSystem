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
PUBLIC cameraWriteInterceptor
PUBLIC fovReadInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: BYTE
EXTERN g_cameraStructAddress: qword
EXTERN g_fovStructAddress: qword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWriteInterceptionContinue: qword
EXTERN _fovReadInterceptionContinue: qword

.data
;---------------------------------------------------------------
; Scratch pad
.code


cameraStructInterceptor PROC
;starwarsjedifallenorder.exe+1B839A60 - 48 89 5C 24 08        - mov [rsp+08],rbx
;starwarsjedifallenorder.exe+1B839A65 - 57                    - push rdi
;starwarsjedifallenorder.exe+1B839A66 - 48 83 EC 20           - sub rsp,20 { 32 }
;starwarsjedifallenorder.exe+1B839A6A - F2 0F10 02            - movsd xmm0,[rdx]				<< INTERCEPT HERE << Coords read. (x,y)
;starwarsjedifallenorder.exe+1B839A6E - 48 89 CF              - mov rdi,rcx
;starwarsjedifallenorder.exe+1B839A71 - F2 0F11 01            - movsd [rcx],xmm0
;starwarsjedifallenorder.exe+1B839A75 - 48 89 D3              - mov rbx,rdx
;starwarsjedifallenorder.exe+1B839A78 - 8B 42 08              - mov eax,[rdx+08]				<< CONTINUE HERE	<< z read
;starwarsjedifallenorder.exe+1B839A7B - 89 41 08              - mov [rcx+08],eax
;starwarsjedifallenorder.exe+1B839A7E - F2 0F10 42 0C         - movsd xmm0,[rdx+0C]
;starwarsjedifallenorder.exe+1B839A83 - F2 0F11 41 0C         - movsd [rcx+0C],xmm0	
;starwarsjedifallenorder.exe+1B839A88 - 8B 42 14              - mov eax,[rdx+14]
;starwarsjedifallenorder.exe+1B839A8B - 89 41 14              - mov [rcx+14],eax
;starwarsjedifallenorder.exe+1B839A8E - 8B 42 18              - mov eax,[rdx+18]
;starwarsjedifallenorder.exe+1B839A91 - 89 41 18              - mov [rcx+18],eax
;starwarsjedifallenorder.exe+1B839A94 - 8B 42 1C              - mov eax,[rdx+1C]
;starwarsjedifallenorder.exe+1B839A97 - 89 41 1C              - mov [rcx+1C],eax
	mov [g_cameraStructAddress], rdx
originalCode:
	movsd xmm0, qword ptr [rdx]	
	mov rdi,rcx
	movsd qword ptr [rcx],xmm0
	mov rbx,rdx
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP

cameraWriteInterceptor PROC
	;starwarsjedifallenorder.exe+1B83D53A - F2 0F10 02            - movsd xmm0,[rdx]
	;starwarsjedifallenorder.exe+1B83D53E - 48 89 CF              - mov rdi,rcx
	;starwarsjedifallenorder.exe+1B83D541 - F2 0F11 01            - movsd [rcx],xmm0				<< INTERCEPT HERE << Coords write. 
	;starwarsjedifallenorder.exe+1B83D545 - 48 89 D3              - mov rbx,rdx
	;starwarsjedifallenorder.exe+1B83D548 - 8B 42 08              - mov eax,[rdx+08]
	;starwarsjedifallenorder.exe+1B83D54B - 89 41 08              - mov [rcx+08],eax				<< Z write
	;starwarsjedifallenorder.exe+1B83D54E - F2 0F10 42 0C         - movsd xmm0,[rdx+0C]
	;starwarsjedifallenorder.exe+1B83D553 - F2 0F11 41 0C         - movsd [rcx+0C],xmm0				<< pitch/yaw write
	;starwarsjedifallenorder.exe+1B83D558 - 8B 42 14              - mov eax,[rdx+14]
	;starwarsjedifallenorder.exe+1B83D55B - 89 41 14              - mov [rcx+14],eax				<< roll write
	;starwarsjedifallenorder.exe+1B83D55E - 8B 42 18              - mov eax,[rdx+18]
	;starwarsjedifallenorder.exe+1B83D561 - 89 41 18              - mov [rcx+18],eax
	;starwarsjedifallenorder.exe+1B83D564 - 8B 42 1C              - mov eax,[rdx+1C]
	;starwarsjedifallenorder.exe+1B83D567 - 89 41 1C              - mov [rcx+1C],eax
	;starwarsjedifallenorder.exe+1B83D56A - 8B 42 20              - mov eax,[rdx+20]
	;starwarsjedifallenorder.exe+1B83D56D - 89 41 20              - mov [rcx+20],eax
	;starwarsjedifallenorder.exe+1B83D570 - 8B 42 24              - mov eax,[rdx+24]
	;starwarsjedifallenorder.exe+1B83D573 - 89 41 24              - mov [rcx+24],eax
	;starwarsjedifallenorder.exe+1B83D576 - 8B 42 28              - mov eax,[rdx+28]
	;starwarsjedifallenorder.exe+1B83D579 - 89 41 28              - mov [rcx+28],eax
	;starwarsjedifallenorder.exe+1B83D57C - 8B 42 2C              - mov eax,[rdx+2C]
	;starwarsjedifallenorder.exe+1B83D57F - 89 41 2C              - mov [rcx+2C],eax
	;starwarsjedifallenorder.exe+1B83D582 - 8B 41 30              - mov eax,[rcx+30]
	;starwarsjedifallenorder.exe+1B83D585 - 33 42 30              - xor eax,[rdx+30]
	;starwarsjedifallenorder.exe+1B83D588 - 83 E0 01              - and eax,01 { 1 }
	;starwarsjedifallenorder.exe+1B83D58B - 31 41 30              - xor [rcx+30],eax				<< NOP for no bars
	;starwarsjedifallenorder.exe+1B83D58E - 8B 4A 30              - mov ecx,[rdx+30]				<< CONTINUE HERE
	;starwarsjedifallenorder.exe+1B83D591 - 33 4F 30              - xor ecx,[rdi+30]

	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode
noWrites:
	mov rbx,rdx
	mov eax,[rdx+08h]
	movsd xmm0, qword ptr[rdx+0Ch]
	mov eax,[rdx+14h]
	jmp exit
originalCode:
	movsd  qword ptr [rcx],xmm0	
	mov rbx,rdx
	mov eax,[rdx+08h]
	mov [rcx+08h],eax	
	movsd xmm0, qword ptr [rdx+0Ch]
	movsd qword ptr [rcx+0Ch],xmm0	
	mov eax,[rdx+14h]
	mov [rcx+14h],eax	
exit:
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
	mov eax,[rcx+30h]
	xor eax,[rdx+30h]
	and eax,01
	;xor [rcx+30],eax		; we'll not do this, ever, so no bars no matter what. 
	jmp qword ptr [_cameraWriteInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWriteInterceptor ENDP

fovReadInterceptor PROC
	;starwarsjedifallenorder.exe+1B8614D8 - 0F1F 84 00 00000000   - nop [rax+rax+00000000]
	;starwarsjedifallenorder.exe+1B8614E0 - 48 81 EC B8050000     - sub rsp,000005B8 { 1464 }
	;starwarsjedifallenorder.exe+1B8614E7 - 0F29 B4 24 A0050000   - movaps [rsp+000005A0],xmm6
	;starwarsjedifallenorder.exe+1B8614EF - 48 8B 05 22DCE4E7     - mov rax,[starwarsjedifallenorder.exe+36AF118] { (-0,06) }
	;starwarsjedifallenorder.exe+1B8614F6 - 48 31 E0              - xor rax,rsp
	;starwarsjedifallenorder.exe+1B8614F9 - 48 89 84 24 90050000  - mov [rsp+00000590],rax				<< INTERCEPT HERE
	;starwarsjedifallenorder.exe+1B861501 - F3 0F10 B1 7C030000   - movss xmm6,[rcx+0000037C]			<< FOV read
	;starwarsjedifallenorder.exe+1B861509 - 31 C0                 - xor eax,eax							<< CONTINUE HERE
	;starwarsjedifallenorder.exe+1B86150B - 0F57 C0               - xorps xmm0,xmm0
	;starwarsjedifallenorder.exe+1B86150E - 89 44 24 20           - mov [rsp+20],eax
	;starwarsjedifallenorder.exe+1B861512 - 0F2F F0               - comiss xmm6,xmm0
	;starwarsjedifallenorder.exe+1B861515 - 77 18                 - ja starwarsjedifallenorder.exe+1B86152F
	;starwarsjedifallenorder.exe+1B861517 - 48 8B 01              - mov rax,[rcx]
	;starwarsjedifallenorder.exe+1B86151A - 48 8D 54 24 30        - lea rdx,[rsp+30]
	;starwarsjedifallenorder.exe+1B86151F - FF 90 78060000        - call qword ptr [rax+00000678]
	mov [g_fovStructAddress], rcx
originalCode:
	mov [rsp+00000590h],rax		
	movss xmm6, dword ptr [rcx+0000037Ch]	
exit:
	jmp qword ptr [_fovReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
fovReadInterceptor ENDP
END