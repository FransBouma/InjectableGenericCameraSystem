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
PUBLIC fovWriteInterceptor
PUBLIC widgetRenderInterceptor 
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_cameraStructAddress: qword
EXTERN g_ownCoordinates: qword
EXTERN g_ownAngles: qword
EXTERN g_fovValue: dword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _fovWriteInterceptionContinue : qword
EXTERN _crossHairRenderInterceptionContinue: qword
EXTERN _widgetRenderInterceptionContinue: qword
.data

;---------------------------------------------------------------
; Scratch pad
; The following function renders widgets. To replace the first statement with a ret, no widgets are rendered, effectively a hud toggle.
; 0000000140D90CE0 Export ABSHUD::DrawStandardWidgets 
;StormGame-Win64-Shipping.exe+D90CE0 - 48 8B C4              - mov rax,rsp							<< REPLACE WITH 'ret' (c9) to disable widgets.
;StormGame-Win64-Shipping.exe+D90CE3 - 41 54                 - push r12
;StormGame-Win64-Shipping.exe+D90CE5 - 41 56                 - push r14
;StormGame-Win64-Shipping.exe+D90CE7 - 41 57                 - push r15
;StormGame-Win64-Shipping.exe+D90CE9 - 48 83 EC 30           - sub rsp,30 { 00000030 }
;StormGame-Win64-Shipping.exe+D90CED - 48 C7 40 E0 FEFFFFFF  - mov [rax-20],FFFFFFFE { -2 }
;StormGame-Win64-Shipping.exe+D90CF5 - 48 89 58 08           - mov [rax+08],rbx
;StormGame-Win64-Shipping.exe+D90CF9 - 48 89 68 10           - mov [rax+10],rbp
;StormGame-Win64-Shipping.exe+D90CFD - 48 89 70 18           - mov [rax+18],rsi
;StormGame-Win64-Shipping.exe+D90D01 - 48 89 78 20           - mov [rax+20],rdi
;StormGame-Win64-Shipping.exe+D90D05 - 45 8B F9              - mov r15d,r9d
;StormGame-Win64-Shipping.exe+D90D08 - 45 8B F0              - mov r14d,r8d
;StormGame-Win64-Shipping.exe+D90D0B - 4C 8B E2              - mov r12,rdx
;StormGame-Win64-Shipping.exe+D90D0E - 48 8B F9              - mov rdi,rcx
;StormGame-Win64-Shipping.exe+D90D11 - C7 40 D8 00000000     - mov [rax-28],00000000 { 0 }
;---------------------------------------------------------------
.code


cameraStructInterceptor PROC
; Struct interceptor is also a write interceptor.
; From 1.1.2, 28/04/2017
;StormGame-Win64-Shipping.exe+3C0E1F - CC                    - int 3 
;StormGame-Win64-Shipping.exe+3C0E20 - 8B 81 1C040000        - mov eax,[rcx+0000041C]			<< INTERCEPT HERE
;StormGame-Win64-Shipping.exe+3C0E26 - 89 02                 - mov [rdx],eax					<< WRITE X
;StormGame-Win64-Shipping.exe+3C0E28 - 8B 81 20040000        - mov eax,[rcx+00000420]
;StormGame-Win64-Shipping.exe+3C0E2E - 89 42 04              - mov [rdx+04],eax
;StormGame-Win64-Shipping.exe+3C0E31 - 8B 81 24040000        - mov eax,[rcx+00000424]
;StormGame-Win64-Shipping.exe+3C0E37 - 89 42 08              - mov [rdx+08],eax
;StormGame-Win64-Shipping.exe+3C0E3A - 8B 81 28040000        - mov eax,[rcx+00000428]			
;StormGame-Win64-Shipping.exe+3C0E40 - 41 89 00              - mov [r8],eax						<< WRITE ROT X
;StormGame-Win64-Shipping.exe+3C0E43 - 8B 81 2C040000        - mov eax,[rcx+0000042C]
;StormGame-Win64-Shipping.exe+3C0E49 - 41 89 40 04           - mov [r8+04],eax
;StormGame-Win64-Shipping.exe+3C0E4D - 8B 81 30040000        - mov eax,[rcx+00000430]
;StormGame-Win64-Shipping.exe+3C0E53 - 41 89 40 08           - mov [r8+08],eax
;StormGame-Win64-Shipping.exe+3C0E57 - C3                    - ret								<< CONTINUE HERE
;StormGame-Win64-Shipping.exe+3C0E58 - CC                    - int 3 

; This function intercepts the camera address and also blocks the writes by reading from our own two structs. 
	mov [g_cameraStructAddress], rcx
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
	; it can be the structs aren't created yet. We check on that and skip to the original code if they're not here yet. 
	cmp qword ptr [g_ownCoordinates], 0
	je originalCode
	cmp qword ptr [g_ownAngles], 0
	je originalCode
readFromOwnStructs:
	push rcx
	mov rcx, [g_ownCoordinates]
	mov eax, [rcx]
	mov [rdx],eax			
	mov eax, [rcx+4h]
	mov [rdx+04h],eax
	mov eax,[rcx+8h]
	mov [rdx+08h],eax
	mov rcx, [g_ownAngles]
	mov eax,[rcx]
	mov [r8],eax				
	mov eax,[rcx+4h]
	mov [r8+04h],eax
	mov eax,[rcx+8h]
	mov [r8+08h],eax
	pop rcx
	jmp exit
originalCode:
	mov eax, [rcx+0000041Ch]
	mov [rdx],eax			
	mov eax, [rcx+00000420h]
	mov [rdx+04h],eax
	mov eax,[rcx+00000424h]
	mov [rdx+08h],eax
	mov eax,[rcx+00000428h]
	mov [r8],eax				
	mov eax,[rcx+0000042Ch]
	mov [r8+04h],eax
	mov eax,[rcx+00000430h]
	mov [r8+08h],eax
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP


fovWriteInterceptor PROC
; This one is nasty. To be able to write the fov we have to inject it into the xmm0 register. This is needed because
; the fov is overwritten by a memmove so we can't intercept it. This way we can inject our own fov value. 
;ABSPlayerCamera::execAdjustFOVForViewport
;StormGame-Win64-Shipping.exe+D16CC3 - 48 8B D3              - mov rdx,rbx
;StormGame-Win64-Shipping.exe+D16CC6 - 48 89 43 24           - mov [rbx+24],rax
;StormGame-Win64-Shipping.exe+D16CCA - FF 15 182DB102        - call qword ptr [StormGame-Win64-Shipping.exe+38299E8] { ->StormGame-Win64-Shipping.exe+64BF0 }
;StormGame-Win64-Shipping.exe+D16CD0 - 4C 8B 44 24 50        - mov r8,[rsp+50]
;StormGame-Win64-Shipping.exe+D16CD5 - F3 0F10 4C 24 48      - movss xmm1,[rsp+48]
;StormGame-Win64-Shipping.exe+D16CDB - 48 8B CE              - mov rcx,rsi
;StormGame-Win64-Shipping.exe+D16CDE - E8 1DC01100           - call StormGame-Win64-Shipping.exe+E32D00
;StormGame-Win64-Shipping.exe+D16CE3 - 48 8B 5C 24 40        - mov rbx,[rsp+40]						<< INTERCEPT HERE
;StormGame-Win64-Shipping.exe+D16CE8 - F3 0F11 07            - movss [rdi],xmm0
;StormGame-Win64-Shipping.exe+D16CEC - 48 83 C4 20           - add rsp,20 { 00000020 }
;StormGame-Win64-Shipping.exe+D16CF0 - 5F                    - pop rdi
;StormGame-Win64-Shipping.exe+D16CF1 - 5E                    - pop rsi
;StormGame-Win64-Shipping.exe+D16CF2 - 5D                    - pop rbp
;StormGame-Win64-Shipping.exe+D16CF3 - C3                    - ret									<< CONTINUE HERE
;StormGame-Win64-Shipping.exe+D16CF4 - CC                    - int 3 
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode									; our own camera is enabled, just skip the writes
	; inject our fov value into the xmm0 register.
	push rcx
	mov ecx, [g_fovValue]
	movd xmm0, ecx
	pop rcx
originalCode:
	mov rbx,[rsp+40h]
	movss dword ptr [rdi],xmm0
	add rsp,20h
	pop rdi
	pop rsi
	pop rbp
exit:
	jmp qword ptr [_fovWriteInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
fovWriteInterceptor ENDP


crossHairRenderInterceptor PROC
; The following code tests whether to render the crosshair. By simply returning 0 in eax, the crosshair is hidden. It will return 1 or 0 but returning 0 w/o
; going through the whole function is easier. 
;ABSPawn::ShouldDrawCrosshair
;StormGame-Win64-Shipping.exe+E1FC90 - 40 53                 - push rbx										<< INTERCEPT HERE
;StormGame-Win64-Shipping.exe+E1FC92 - 48 83 EC 20           - sub rsp,20 { 00000020 }
;StormGame-Win64-Shipping.exe+E1FC96 - 48 8B D9              - mov rbx,rcx
;StormGame-Win64-Shipping.exe+E1FC99 - 41 B9 01000000        - mov r9d,00000001 { 1 }
;StormGame-Win64-Shipping.exe+E1FC9F - 48 8D 15 62EC6C00     - lea rdx,[StormGame-Win64-Shipping.exe+14EE908] { ("WaitForReviveOrRestart") }  << CONTINUE HERE
;StormGame-Win64-Shipping.exe+E1FCA6 - 48 8D 4C 24 30        - lea rcx,[rsp+30]
;StormGame-Win64-Shipping.exe+E1FCAB - 45 8B C1              - mov r8d,r9d
;StormGame-Win64-Shipping.exe+E1FCAE - E8 9D5F2AFF           - call StormGame-Win64-Shipping.exe+C5C50
;StormGame-Win64-Shipping.exe+E1FCB3 - 48 8B 8B 28070000     - mov rcx,[rbx+00000728]
;StormGame-Win64-Shipping.exe+E1FCBA - 48 85 C9              - test rcx,rcx
;StormGame-Win64-Shipping.exe+E1FCBD - 74 2C                 - je StormGame-Win64-Shipping.exe+E1FCEB
;StormGame-Win64-Shipping.exe+E1FCBF - 48 8B 54 24 30        - mov rdx,[rsp+30]
;StormGame-Win64-Shipping.exe+E1FCC4 - 45 33 C0              - xor r8d,r8d
;StormGame-Win64-Shipping.exe+E1FCC7 - E8 14B225FF           - call StormGame-Win64-Shipping.exe+7AEE0
;StormGame-Win64-Shipping.exe+E1FCCC - 85 C0                 - test eax,eax
;StormGame-Win64-Shipping.exe+E1FCCE - 75 13                 - jne StormGame-Win64-Shipping.exe+E1FCE3
;StormGame-Win64-Shipping.exe+E1FCD0 - 48 8B 83 28070000     - mov rax,[rbx+00000728]
;StormGame-Win64-Shipping.exe+E1FCD7 - F7 80 C4080000 00800000 - test [rax+000008C4],00008000 { 32768 }
;StormGame-Win64-Shipping.exe+E1FCE1 - 74 08                 - je StormGame-Win64-Shipping.exe+E1FCEB
;StormGame-Win64-Shipping.exe+E1FCE3 - 33 C0                 - xor eax,eax
;StormGame-Win64-Shipping.exe+E1FCE5 - 48 83 C4 20           - add rsp,20 { 00000020 }
	cmp byte ptr [g_cameraEnabled], 1						; check if the user enabled the camera. 
	jne originalCode
noCrossHair:
	; simply load eax with 0, then do an explit return, so the intercepted function 'ABSPawn::ShouldDrawCrosshair' function effectively ends here
	xor eax, eax
	ret
originalCode:
	push rbx		
	sub rsp,20h
	mov rbx,rcx
	mov r9d,00000001h
exit:
	jmp qword ptr [_crossHairRenderInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
crossHairRenderInterceptor ENDP

widgetRenderInterceptor PROC
; The following function renders widgets. To replace the first statement with a ret, no widgets are rendered, effectively a hud toggle.
; 0000000140D90CE0 Export ABSHUD::DrawStandardWidgets 
;StormGame-Win64-Shipping.exe+D90CE0 - 48 8B C4              - mov rax,rsp				 << INTERCEPT HERE	<< REPLACE WITH 'ret' (c9) to disable widgets.
;StormGame-Win64-Shipping.exe+D90CE3 - 41 54                 - push r12
;StormGame-Win64-Shipping.exe+D90CE5 - 41 56                 - push r14
;StormGame-Win64-Shipping.exe+D90CE7 - 41 57                 - push r15
;StormGame-Win64-Shipping.exe+D90CE9 - 48 83 EC 30           - sub rsp,30 { 00000030 }
;StormGame-Win64-Shipping.exe+D90CED - 48 C7 40 E0 FEFFFFFF  - mov [rax-20],FFFFFFFE { -2 }
;StormGame-Win64-Shipping.exe+D90CF5 - 48 89 58 08           - mov [rax+08],rbx			<< CONTINUE HERE
;StormGame-Win64-Shipping.exe+D90CF9 - 48 89 68 10           - mov [rax+10],rbp
;StormGame-Win64-Shipping.exe+D90CFD - 48 89 70 18           - mov [rax+18],rsi
;StormGame-Win64-Shipping.exe+D90D01 - 48 89 78 20           - mov [rax+20],rdi
;StormGame-Win64-Shipping.exe+D90D05 - 45 8B F9              - mov r15d,r9d
;StormGame-Win64-Shipping.exe+D90D08 - 45 8B F0              - mov r14d,r8d
	cmp byte ptr [g_cameraEnabled], 1						; check if the user enabled the camera. 
	jne originalCode
noWidgets:
	; simply do an explit return, so the intercepted function 'ABSHUD::DrawStandardWidgets' function effectively ends here
	ret
originalCode:
	mov rax,rsp			
	push r12
	push r14
	push r15
	sub rsp,30h
	mov dword ptr [rax-20h],0FFFFFFFEh
exit:
	jmp qword ptr [_widgetRenderInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
widgetRenderInterceptor ENDP


END