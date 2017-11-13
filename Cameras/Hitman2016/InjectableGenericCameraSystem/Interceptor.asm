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
PUBLIC cameraAddressInterceptor
PUBLIC cameraWriteInterceptor1
PUBLIC cameraWriteInterceptor2
PUBLIC cameraWriteInterceptor3
PUBLIC cameraReadInterceptor1
PUBLIC gamespeedAddressInterceptor
PUBLIC fovWriteInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraStructAddress: qword
EXTERN g_gamespeedStructAddress: qword
EXTERN g_cameraEnabled: byte
EXTERN g_aimFrozen: byte
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWriteInterceptionContinue1: qword
EXTERN _cameraWriteInterceptionContinue2: qword
EXTERN _cameraWriteInterceptionContinue3: qword
EXTERN _cameraReadInterceptionContinue1: qword
EXTERN _gamespeedInterceptionContinue: qword
EXTERN _fovWriteInterceptionContinue: qword

; Scratch pad
;hitman.exe+E609DA - 0F28 CE               - movaps xmm1,xmm6
;hitman.exe+E609DD - 8B 0D 19EFD300        - mov ecx,[hitman.exe+1B9F8FC]
;hitman.exe+E609E3 - F3 0F10 05 9D6C0C02   - movss xmm0,[hitman.exe+2F27688] << supersampling read here
;hitman.exe+E609EB - 85 C9                 - test ecx,ecx
;hitman.exe+E609ED - 74 08                 - je hitman.exe+E609F7
;
;---------------------------------------------------------------
.code

cameraAddressInterceptor PROC
;hitman.exe+43B9E24 - F3 0F10 44 24 50      - movss xmm0,[rsp+50]
;hitman.exe+43B9E2A - F3 0F11 83 90000000   - movss [rbx+00000090],xmm0		<<<<<<<< HERE
;hitman.exe+43B9E32 - F3 0F10 44 24 58      - movss xmm0,[rsp+58]
;hitman.exe+43B9E38 - F3 0F11 83 98000000   - movss [rbx+00000098],xmm0		<<<<<<<< CONTINUE
;hitman.exe+43B9E40 - F3 0F11 8B 94000000   - movss [rbx+00000094],xmm1
;
	; Game jmps to this location due to the hook set in C function SetCameraStructInterceptorHook
	cmp byte ptr [rbx+038h], 0 							; check if this is the camera in rbx. For this game: Check with a 0-check. Could also check +20 or +24 for 0 if the above fails
	jne originalCode
	mov [g_cameraStructAddress], rbx					; intercept address of camera struct
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movss dword ptr [rbx+090h], xmm0					; original statement
exit:
    movss xmm0, dword ptr [rsp+058h]					; original statement
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraAddressInterceptor ENDP

gamespeedAddressInterceptor PROC
;hitman.exe+43BCC87 - 48 C1 F8 14           - sar rax,14 { 20 }
;hitman.exe+43BCC8B - 48 89 43 28           - mov [rbx+28],rax				<<< HERE
;hitman.exe+43BCC8F - 48 8B 4B 18           - mov rcx,[rbx+18]
;hitman.exe+43BCC93 - 48 89 4B 20           - mov [rbx+20],rcx
;hitman.exe+43BCC97 - 48 01 43 18           - add [rbx+18],rax
;hitman.exe+43BCC9B - EB 3B                 - jmp hitman.exe+43BCCD8		<<< CONTINUE. JMP offset can differ per build.
	mov [g_gamespeedStructAddress], rbx
	mov [rbx+028h],rax
	mov rcx,[rbx+018h]
	mov [rbx+020h],rcx
	add [rbx+018h],rax
	jmp qword ptr [_gamespeedInterceptionContinue]
gamespeedAddressInterceptor ENDP


;-------------------------------------------------------------------
; Camera values write interceptions. For each block of statements in the game's exe which write to the camera values, we intercept them and execute them if our
; own camera is disabled, otherwise we skip them. Each block checks whether the current struct pointer is the camera struct. If it's not the camera struct, it will
; execute the code regardless of whether our camera is enabled.

cameraWriteInterceptor1 PROC
;hitman.exe+43B9E14 - F3 0F10 4C 24 54      - movss xmm1,[rsp+54]
;hitman.exe+43B9E1A - 0F28 00               - movaps xmm0,[rax]					<<<< HERE
;hitman.exe+43B9E1D - 0F11 83 80000000      - movups [rbx+00000080],xmm0
;hitman.exe+43B9E24 - F3 0F10 44 24 50      - movss xmm0,[rsp+50]
;hitman.exe+43B9E2A - F3 0F11 83 90000000   - movss [rbx+00000090],xmm0			<<<< CONTINUE
	; Game jmps to this location due to the hook set in C function SetMatrixWriteInterceptorHooks. 
	; first check if this is really a call for the camera. Other logic will use this code too. Check rbx with our known cameraStruct address to be sure
	cmp qword ptr rbx, [g_cameraStructAddress]
	jne originalCode
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movaps xmm0, xmmword ptr [rax]						; original statement
	movups xmmword ptr [rbx+080h],xmm0					; original statement
exit:
	movss xmm0, dword ptr [rsp+050h]					; original statement
	jmp qword ptr [_cameraWriteInterceptionContinue1]	; jmp back into the original game code which is the location after the original statements above.
cameraWriteInterceptor1 ENDP

cameraWriteInterceptor2 PROC
;hitman.exe+43B9E32 - F3 0F10 44 24 58      - movss xmm0,[rsp+58]
;hitman.exe+43B9E38 - F3 0F11 83 98000000   - movss [rbx+00000098],xmm0			<<<<< HERE
;hitman.exe+43B9E40 - F3 0F11 8B 94000000   - movss [rbx+00000094],xmm1
;hitman.exe+43B9E48 - EB 44                 - jmp hitman.exe+43B9E8E			<<<<< CONTINUE

	; Game jmps to this location due to the hook set in C function SetMatrixWriteInterceptorHooks. 
	; first check if this is really a call for the camera. Other logic will use this code too. Check rbx with our known cameraStruct address to be sure
	cmp qword ptr rbx, [g_cameraStructAddress]
	jne originalCode
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movss dword ptr [rbx+98h],xmm0						; original statement
	movss dword ptr [rbx+94h],xmm1						; original statement
exit:
	jmp qword ptr [_cameraWriteInterceptionContinue2]	; jmp back into the original game code which is the location after the original statements above.
cameraWriteInterceptor2 ENDP

cameraWriteInterceptor3 PROC
;hitman.exe+43B9E5D - 0F28 00               - movaps xmm0,[rax]
;hitman.exe+43B9E60 - 0F11 83 80000000      - movups [rbx+00000080],xmm0		<<<< HERE
;hitman.exe+43B9E67 - 0F28 4F 30            - movaps xmm1,[rdi+30]
;hitman.exe+43B9E6B - 0F28 C1               - movaps xmm0,xmm1
;hitman.exe+43B9E6E - F3 0F11 8B 90000000   - movss [rbx+00000090],xmm1
;hitman.exe+43B9E76 - 0FC6 C1 55            - shufps xmm0,xmm1,55 { 85 }
;hitman.exe+43B9E7A - 0FC6 C9 AA            - shufps xmm1,xmm1,-56 { 170 }
;hitman.exe+43B9E7E - F3 0F11 8B 98000000   - movss [rbx+00000098],xmm1
;hitman.exe+43B9E86 - F3 0F11 83 94000000   - movss [rbx+00000094],xmm0
;hitman.exe+43B9E8E - 48 8B 03              - mov rax,[rbx]						<<<< CONTINUE
	; Game jmps to this location due to the hook set in C function SetMatrixWriteInterceptorHooks. 
	; first check if this is really a call for the camera. Other logic will use this code too. Check rbx with our known cameraStruct address to be sure
	cmp qword ptr rbx, [g_cameraStructAddress]
	jne originalCode
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movups xmmword ptr [rbx+80h],xmm0  					; original statement
	movaps xmm1,xmmword ptr [rdi+30h]  					; original statement
	movaps xmm0,xmm1  									; original statement
	movss  dword ptr [rbx+90h],xmm1  					; original statement
	shufps xmm0,xmm1,55h  								; original statement
	shufps xmm1,xmm1,0AAh  								; original statement
	movss  dword ptr [rbx+98h],xmm1  					; original statement
	movss  dword ptr [rbx+94h],xmm0						; original statement
exit:
	jmp qword ptr [_cameraWriteInterceptionContinue3]	; jmp back into the original game code which is the location after the original statements above.
cameraWriteInterceptor3 ENDP

cameraReadInterceptor1 PROC
; v1.13.2
;0000000140BDE4C0 | 40 53                    | push rbx										<< INTERCEPT HERE
;0000000140BDE4C2 | 48 81 EC 80 00 00 00     | sub rsp,80                            
;0000000140BDE4C9 | F6 81 AE 00 00 00 02     | test byte ptr ds:[rcx+AE],2           
;0000000140BDE4D0 | 48 8B D9                 | mov rbx,rcx									<< CONTINUE
;0000000140BDE4D3 | 0F 84 53 02 00 00        | je hitman_dump.140BDE72C              
;0000000140BDE4D9 | 48 8B 41 18              | mov rax,qword ptr ds:[rcx+18]         
;0000000140BDE4DD | F3 0F 10 81 98 00 00 00  | movss xmm0,dword ptr ds:[rcx+98]      
;0000000140BDE4E5 | 0F 29 7C 24 60           | movaps xmmword ptr ss:[rsp+60],xmm7   
;0000000140BDE4EA | F3 0F 7E B9 90 00 00 00  | movq xmm7,qword ptr ds:[rcx+90]       
;0000000140BDE4F2 | 44 0F 29 44 24 50        | movaps xmmword ptr ss:[rsp+50],xmm8   
;0000000140BDE4F8 | 0F C6 F8 04              | shufps xmm7,xmm0,4                    
;0000000140BDE4FC | 44 0F 10 81 80 00 00 00  | movups xmm8,xmmword ptr ds:[rcx+80]   
;0000000140BDE504 | 48 85 C0                 | test rax,rax                          
	cmp byte ptr [g_cameraEnabled], 1					
	jne originalCode
	cmp byte ptr [g_aimFrozen], 1
	jne originalCode
	; aim is frozen and camera is enabled. We now issue a 'ret', which means it will return to the caller of the original code as we jmp-ed to this location
	; from there. This means we won't continue in the original code. 
	ret
originalCode:
	push rbx
	sub rsp, 00000080h
	test byte ptr [rcx+000000AEh],02h
exit:
	jmp qword ptr [_cameraReadInterceptionContinue1]	; jmp back into the original game code which is the location after the original statements above.
cameraReadInterceptor1 ENDP


fovWriteInterceptor PROC
; FOV write:  (1.13.2)
; hitman.exe+398799 - 0F28 C1               - movaps xmm0,xmm1
; hitman.exe+39879C - 48 8B 89 E8060000     - mov rcx,[rcx+000006E8]
; hitman.exe+3987A3 - 48 85 C9              - test rcx,rcx
; hitman.exe+3987A6 - 74 05                 - je hitman.exe+3987AD
; hitman.exe+3987A8 - E8 A3C0FEFF           - call hitman.exe+384850
; hitman.exe+3987AD - 0F28 C8               - movaps xmm1,xmm0
; hitman.exe+3987B0 - F3 0F11 83 00070000   - movss [rbx+00000700],xmm0				<< INTERCEPT HERE	<< FOV WRITE (Degrees. Default is 40)
; hitman.exe+3987B8 - 48 8B CB              - mov rcx,rbx
; hitman.exe+3987BB - 48 83 C4 20           - add rsp,20 { 32 }
; hitman.exe+3987BF - 5B                    - pop rbx								<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1					
	je exit
originalCode:
	movss dword ptr [rbx+00000700h],xmm0
exit:
	mov rcx,rbx
	add rsp,20h
	jmp qword ptr [_fovWriteInterceptionContinue]	; jmp back into the original game code which is the location after the original statements above.
fovWriteInterceptor ENDP

END