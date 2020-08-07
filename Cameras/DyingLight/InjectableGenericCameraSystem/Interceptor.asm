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
PUBLIC lodReadInterceptor
PUBLIC timestopReadInterceptor
PUBLIC todWriteInterceptor
PUBLIC todReadInterceptor
PUBLIC getScreenWidthInterceptor

;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_cameraStructAddress: qword
EXTERN g_lodStructAddress: qword
EXTERN g_timestopStructAddress: qword
EXTERN g_todStructAddress: qword
EXTERN g_resolutionStructAddress: qword			; defined in UniversalD3D11Hooker.cpp
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWrite1InterceptionContinue: qword
EXTERN _lodReadInterceptionContinue: qword
EXTERN _timestopReadInterceptionContinue: qword
EXTERN _todWriteInterceptionContinue: qword
EXTERN _todReadInterceptionContinue: qword
EXTERN _getScreenWidthInterceptionContinue: qword

.data

;---------------------------------------------------------------
; Scratch pad

.code


cameraStructInterceptor PROC
; IBaseCamera.GetViewMatrix()
;engine_x64_rwdi.dll+121E90 - 48 8B 41 08           - mov rax,[rcx+08]								<< INTERCEPT HERE<< Get pointer to matrix struct. 
;engine_x64_rwdi.dll+121E94 - 48 85 C0              - test rax,rax
;engine_x64_rwdi.dll+121E97 - 74 05                 - je engine_x64_rwdi.dll+121E9E
;engine_x64_rwdi.dll+121E99 - 48 83 C0 10           - add rax,10									
;engine_x64_rwdi.dll+121E9D - C3                    - ret 
;engine_x64_rwdi.dll+121E9E - 48 8D 05 0B248E00     - lea rax,[engine_x64_rwdi.dll+A042B0]			<< CONTINUE HERE (if je is taken)
;engine_x64_rwdi.dll+121EA5 - C3                    - ret 
; there's little space here, so we need to include the 'ret' in the code. So for the situations where rax isn't a null pointer, we simply don't jump back to the original code, as there's no
; code left to execute, we simply issue the ret here. 
	mov rax,[rcx+08h]
	test rax,rax
	je nullpointer
	add rax,10h
	mov [g_cameraStructAddress], rax
	ret
nullpointer:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP


cameraWrite1Interceptor PROC
; There's a generic function which writes to the viewmatrix. We proceed as normal if the camera isn't enabled or the target of the function (in rcx) isn't equal to the camera struct we've intercepted.
; if it's equal or the camera is enabled we simply return. 
;engine_x64_rwdi.dll+676510 - 48 83 EC 48           - sub rsp,48							<< INTERCEPT HERE
;engine_x64_rwdi.dll+676514 - 0F29 74 24 30         - movaps [rsp+30],xmm6
;engine_x64_rwdi.dll+676519 - F3 41 0F10 70 08      - movss xmm6,[r8+08]
;engine_x64_rwdi.dll+67651F - 0F29 7C 24 20         - movaps [rsp+20],xmm7					<< CONTINUE HERE (if target isn't our camera struct)
;engine_x64_rwdi.dll+676524 - F3 41 0F10 38         - movss xmm7,[r8]
;engine_x64_rwdi.dll+676529 - 44 0F29 44 24 10      - movaps [rsp+10],xmm8
;engine_x64_rwdi.dll+67652F - F3 45 0F10 40 04      - movss xmm8,[r8+04]
;engine_x64_rwdi.dll+676535 - 0F28 C7               - movaps xmm0,xmm7
;engine_x64_rwdi.dll+676538 - 41 0F28 D0            - movaps xmm2,xmm8
;engine_x64_rwdi.dll+67653C - F3 0F59 C7            - mulss xmm0,xmm7
;engine_x64_rwdi.dll+676540 - 0F28 CE               - movaps xmm1,xmm6
;engine_x64_rwdi.dll+676543 - 44 0F29 0C 24         - movaps [rsp],xmm9
;engine_x64_rwdi.dll+676548 - F3 41 0F59 D0         - mulss xmm2,xmm8
;engine_x64_rwdi.dll+67654D - F3 0F59 CE            - mulss xmm1,xmm6
;engine_x64_rwdi.dll+676551 - F3 44 0F10 0D A2371700  - movss xmm9,[engine_x64_rwdi.dll+7E9CFC]
;engine_x64_rwdi.dll+67655A - F3 0F58 D0            - addss xmm2,xmm0
;engine_x64_rwdi.dll+67655E - F3 0F58 D1            - addss xmm2,xmm1
;engine_x64_rwdi.dll+676562 - F3 0F51 DA            - sqrtss xmm3,xmm2
	push rcx
	cmp byte ptr [g_cameraEnabled], 1
	jne originalCode
	sub rcx, 30h		; sub 0x30 because the address used in this function is 0x30 off with the address we have.
	cmp qword ptr rcx, [g_cameraStructAddress]
	jne originalCode
	pop rcx
	; it's targeting our struct, and camera is enabled, skip the whole function, so we simply issue a ret here
	ret
originalCode:
	pop rcx
	sub rsp,48h
	movaps xmmword ptr [rsp+30h],xmm6
	movss xmm6, dword ptr [r8+08h]
exit:
	jmp qword ptr [_cameraWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite1Interceptor ENDP


lodReadInterceptor PROC
;engine_x64_rwdi.dll+5098A5 - F3 0F10 88 30020000    - movss xmm1,[rax+00000230]					<< INTERCEPT HERE
;engine_x64_rwdi.dll+5098AD - F3 0F59 4A 28          - mulss xmm1,[rdx+28]							<< LOD1 Read. set to 10000.0f
;engine_x64_rwdi.dll+5098B2 - F3 0F59 C9             - mulss xmm1,xmm1		
;engine_x64_rwdi.dll+5098B6 - F3 0F5E C1             - divss xmm0,xmm1								<< CONTINUE HERE
;engine_x64_rwdi.dll+5098BA - F3 41 0F11 86 84010000 - movss [r14+00000184],xmm0
;engine_x64_rwdi.dll+5098C3 - 49 8B 40 30            - mov rax,[r8+30]
;engine_x64_rwdi.dll+5098C7 - F3 0F10 88 30020000    - movss xmm1,[rax+00000230]
;engine_x64_rwdi.dll+5098CF - F3 0F59 4A 2C          - mulss xmm1,[rdx+2C]							<< LOD2 Read. set to 10000.0f
;engine_x64_rwdi.dll+5098D4 - F3 0F59 C9             - mulss xmm1,xmm1
;engine_x64_rwdi.dll+5098D8 - F3 0F5E D9             - divss xmm3,xmm1
;engine_x64_rwdi.dll+5098DC - F3 41 0F11 9E 88010000 - movss [r14+00000188],xmm3
;engine_x64_rwdi.dll+5098E5 - 49 8B 48 08            - mov rcx,[r8+08]
	mov [g_lodStructAddress], rdx
originalCode:
	movss xmm1, dword ptr [rax+00000230h]
	mulss xmm1, dword ptr [rdx+28h]
	mulss xmm1,xmm1
exit:
	jmp qword ptr [_lodReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
lodReadInterceptor ENDP


todWriteInterceptor PROC
;engine_x64_rwdi.dll+254409 - 0F29 74 24 20         - movaps [rsp+20],xmm6
;engine_x64_rwdi.dll+25440E - 0F28 F2               - movaps xmm6,xmm2
;engine_x64_rwdi.dll+254411 - E8 1A204F00           - call engine_x64_rwdi.dll+746430
;engine_x64_rwdi.dll+254416 - 48 85 C0              - test rax,rax									<< INTERCEPT HERE
;engine_x64_rwdi.dll+254419 - 74 1C                 - je engine_x64_rwdi.dll+254437
;engine_x64_rwdi.dll+25441B - F3 0F11 30            - movss [rax],xmm6								<< WRITE ToD (float: hour (0/24) is value > 0, fraction is minutes % of hour)
;engine_x64_rwdi.dll+25441F - 8B 43 08              - mov eax,[rbx+08]
;engine_x64_rwdi.dll+254422 - 83 F8 FE              - cmp eax,-02
;engine_x64_rwdi.dll+254425 - 72 10                 - jb engine_x64_rwdi.dll+254437					<< CONTINUE
;engine_x64_rwdi.dll+254427 - E8 14AADDFF           - call engine_x64_rwdi.dll+2EE40
;engine_x64_rwdi.dll+25442C - 48 8B D3              - mov rdx,rbx
;engine_x64_rwdi.dll+25442F - 48 8B C8              - mov rcx,rax
;engine_x64_rwdi.dll+254432 - E8 89A8DDFF           - call engine_x64_rwdi.dll+2ECC0
;engine_x64_rwdi.dll+254437 - 0F28 74 24 20         - movaps xmm6,[rsp+20]
;engine_x64_rwdi.dll+25443C - 48 83 C4 30           - add rsp,30 { 48 }
;engine_x64_rwdi.dll+254440 - 5B                    - pop rbx
;engine_x64_rwdi.dll+254441 - F3 C3                 - repe ret 
	test rax,rax					
	je nullptr										; we have to do it this way, as the original code doesn't allow a 14 byte jump statement otherwise, and we can't copy the calls as they're RIP relative
	cmp qword ptr rax, [g_todStructAddress]			; if we're not targeting the address we found, we are not writing to the real value so we can just proceed with the original code
	jne originalCode
	cmp byte ptr [g_cameraEnabled], 1
	je nowrite
originalCode:
	movss dword ptr [rax],xmm6				
nowrite:
	mov eax,[rbx+08h]
	cmp eax,-02h
exit:
	jmp qword ptr [_todWriteInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
nullptr:
	movaps xmm6, xmmword ptr [rsp+20h]
	add rsp,30h
	pop rbx
	ret							; masm doesn't know what 'repe ret' means, so we just issue a 'ret' here, which is fine. the prefix is only needed on very old AMD CPUs and ppl using these can't run the game anyway. 
todWriteInterceptor ENDP


todReadInterceptor PROC
;gamedll_x64_rwdi.dll+109C791 - 48 8B 93 A8090000     - mov rdx,[rbx+000009A8]
;gamedll_x64_rwdi.dll+109C798 - 41 B8 04000000        - mov r8d,00000004 { 4 }
;gamedll_x64_rwdi.dll+109C79E - 8B 52 50              - mov edx,[rdx+50]
;gamedll_x64_rwdi.dll+109C7A1 - 48 8B C8              - mov rcx,rax
;gamedll_x64_rwdi.dll+109C7A4 - E8 47291D00           - call gamedll_x64_rwdi.dll+126F0F0
;gamedll_x64_rwdi.dll+109C7A9 - 48 8B CF              - mov rcx,rdi								<< INTERCEPT HERE
;gamedll_x64_rwdi.dll+109C7AC - 48 85 C0              - test rax,rax
;gamedll_x64_rwdi.dll+109C7AF - 48 0F45 C8            - cmovne rcx,rax
;gamedll_x64_rwdi.dll+109C7B3 - F3 0F10 39            - movss xmm7,[rcx]			<< READ ToD (float: hour (0/24) is value > 0, fraction is minutes % of hour)
;gamedll_x64_rwdi.dll+109C7B7 - EB 03                 - jmp gamedll_x64_rwdi.dll+109C7BC		<< CONTINUE HERE
;gamedll_x64_rwdi.dll+109C7B9 - 0F28 FE               - movaps xmm7,xmm6			<< A relative jump lands here!
;gamedll_x64_rwdi.dll+109C7BC - 48 83 BB B0090000 00  - cmp qword ptr [rbx+000009B0],00 
;gamedll_x64_rwdi.dll+109C7C4 - 74 2C                 - je gamedll_x64_rwdi.dll+109C7F2
	mov rcx,rdi		
	test rax,rax
	cmovne rcx,rax
	mov [g_todStructAddress], rcx
	movss xmm7, dword ptr [rcx]
exit:
	jmp qword ptr [_todReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
todReadInterceptor ENDP


timestopReadInterceptor PROC
;engine_x64_rwdi.dll+28FFC0 - 48 8B 41 08           - mov rax,[rcx+08]								<< INTERCEPT HERE
;engine_x64_rwdi.dll+28FFC4 - 48 05 280F0000        - add rax,00000F28
;engine_x64_rwdi.dll+28FFCA - 80 78 30 00           - cmp byte ptr [rax+30],00						<< Read pause byte. 
;engine_x64_rwdi.dll+28FFCE - 74 09                 - je engine_x64_rwdi.dll+28FFD9					<< CONTINUE HERE
;engine_x64_rwdi.dll+28FFD0 - 80 78 48 00           - cmp byte ptr [rax+48],00						<< Read second pause byte. Keep byte at 0 for pause.
;engine_x64_rwdi.dll+28FFD4 - 75 03                 - jne engine_x64_rwdi.dll+28FFD9
;engine_x64_rwdi.dll+28FFD6 - B0 01                 - mov al,01 { 1 }
;engine_x64_rwdi.dll+28FFD8 - C3                    - ret 
;engine_x64_rwdi.dll+28FFD9 - 32 C0                 - xor al,al
;engine_x64_rwdi.dll+28FFDB - C3                    - ret 
	mov rax,[rcx+08h]
	add rax,00000F28h
	cmp byte ptr [rax+30h], 00	
	mov [g_timestopStructAddress], rax
exit:
	jmp qword ptr [_timestopReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
timestopReadInterceptor ENDP


getScreenWidthInterceptor PROC
;engine_x64_rwdi.dll+232D80 - 48 8B 41 08           - mov rax,[rcx+08]				<< INTERCEPT HERE
;engine_x64_rwdi.dll+232D84 - 48 8B 88 C0000000     - mov rcx,[rax+000000C0]
;engine_x64_rwdi.dll+232D8B - 8B 01                 - mov eax,[rcx]
;engine_x64_rwdi.dll+232D8D - C3                    - ret							
;engine_x64_rwdi.dll+232D8E - CC                    - int 3							<< CONTINUE HERE. That's fine, the 'ret' is included in our code which will return to the caller.
;engine_x64_rwdi.dll+232D8F - CC                    - int 3 
	mov rax,[rcx+08h]		
	mov rcx,[rax+000000C0h]
	mov [g_resolutionStructAddress], rcx
	mov eax,[rcx]
	ret		
	; no jump back as that's not needed.
getScreenWidthInterceptor ENDP
END