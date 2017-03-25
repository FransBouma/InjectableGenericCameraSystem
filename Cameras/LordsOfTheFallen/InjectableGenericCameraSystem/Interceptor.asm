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
PUBLIC fovWriteInterceptor1
PUBLIC fovWriteInterceptor2
PUBLIC hudToggleInterceptor
PUBLIC camera2WriteInterceptor1
PUBLIC camera2WriteInterceptor2
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_cameraStructAddress: qword
EXTERN g_hudVisible: byte		; 1 == hud visible, 0 == hud hidden
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWriteInterceptionContinue: qword
EXTERN _fovWriteInterception1Continue: qword
EXTERN _fovWriteInterception2Continue: qword
EXTERN _hudToggleInterceptionContinue: qword
EXTERN _camera2WriteInterception1Continue: qword
EXTERN _camera2WriteInterception2Continue: qword
EXTERN _camera2CoordsAddress: qword
EXTERN _camera2QuaternionAddress: qword

.data
hudVisibleValue dd 1.0
hudHiddenValue dd 0.0

;---------------------------------------------------------------
; Scratch pad
;LordsOfTheFallen.exe+D2E6FFE - F3 0F11 40 18         - movss [rax+18],xmm0					<< FOV WRITE
;LordsOfTheFallen.exe+D2E7003 - C6 80 4C010000 01     - mov byte ptr [rax+0000014C],01 { 1 }
;LordsOfTheFallen.exe+D2E700A - 66 C7 80 4E010000 0101 - mov word ptr [rax+0000014E],0101 { 257 }
;LordsOfTheFallen.exe+D2E7013 - C6 80 50010000 01     - mov byte ptr [rax+00000150],01 { 1 }
;
;---------------------------------------------------------------
.code


cameraStructInterceptor PROC
;LordsOfTheFallen.exe+D0D79F7 - F3 0F10 48 4C         - movss xmm1,[rax+4C]				<<< INTERCEPT HERE
;LordsOfTheFallen.exe+D0D79FC - F3 0F10 40 48         - movss xmm0,[rax+48]
;LordsOfTheFallen.exe+D0D7A01 - F3 0F11 4C 24 34      - movss [rsp+34],xmm1
;LordsOfTheFallen.exe+D0D7A07 - F3 0F10 49 18         - movss xmm1,[rcx+18]				<<< FOV READ, RCX contains base address for camera struct.
;LordsOfTheFallen.exe+D0D7A0C - F3 0F11 44 24 30      - movss [rsp+30],xmm0
;LordsOfTheFallen.exe+D0D7A12 - 8B 0E                 - mov ecx,[rsi]
;LordsOfTheFallen.exe+D0D7A14 - 48 01 F9              - add rcx,rdi						<<< CONTINUE HERE
	mov [g_cameraStructAddress], rcx
	movss xmm1, dword ptr [rax+4Ch]
	movss xmm0, dword ptr [rax+48h]
	movss dword ptr [rsp+34h],xmm1
	movss xmm1, dword ptr [rcx+18h]
	movss dword ptr [rsp+30h],xmm0
	mov ecx,[rsi]
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP

cameraWriteInterceptor PROC
;LordsOfTheFallen.exe+D39AEDD - 89 82 BC000000        - mov [rdx+000000BC],eax		<< INTERCEPT HERE, xQ
;LordsOfTheFallen.exe+D39AEE3 - 8B 41 38              - mov eax,[rcx+38]
;LordsOfTheFallen.exe+D39AEE6 - 89 82 B0000000        - mov [rdx+000000B0],eax
;LordsOfTheFallen.exe+D39AEEC - 8B 41 3C              - mov eax,[rcx+3C]
;LordsOfTheFallen.exe+D39AEEF - 89 82 B4000000        - mov [rdx+000000B4],eax
;LordsOfTheFallen.exe+D39AEF5 - 8B 41 40              - mov eax,[rcx+40]
;LordsOfTheFallen.exe+D39AEF8 - 89 82 B8000000        - mov [rdx+000000B8],eax
;LordsOfTheFallen.exe+D39AEFE - 8B 41 48              - mov eax,[rcx+48]
;LordsOfTheFallen.exe+D39AF01 - 89 82 C0000000        - mov [rdx+000000C0],eax		<< X 
;LordsOfTheFallen.exe+D39AF07 - 8B 41 4C              - mov eax,[rcx+4C]
;LordsOfTheFallen.exe+D39AF0A - 89 82 C4000000        - mov [rdx+000000C4],eax		<< Y
;LordsOfTheFallen.exe+D39AF10 - 8B 41 50              - mov eax,[rcx+50]
;LordsOfTheFallen.exe+D39AF13 - 89 82 C8000000        - mov [rdx+000000C8],eax		<< Z
;LordsOfTheFallen.exe+D39AF19 - C6 82 4D010000 01     - mov byte ptr [rdx+0000014D],01 <<<< CONTINUE HERE
	; check for our coord target address. This code is used by multiple target structures
	cmp qword ptr rdx, [g_cameraStructAddress]
	jne originalCode									; code operates on other struct than the camera struct, leave it.
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	mov [rdx+000000BCh],eax
	mov eax,[rcx+38h]
	mov [rdx+000000B0h],eax
	mov eax,[rcx+3Ch]
	mov [rdx+000000B4h],eax
	mov eax,[rcx+40h]
	mov [rdx+000000B8h],eax
	mov eax,[rcx+48h]
	mov [rdx+000000C0h],eax
	mov eax,[rcx+4Ch]
	mov [rdx+000000C4h],eax
	mov eax,[rcx+50h]
	mov [rdx+000000C8h],eax
exit:
	jmp qword ptr [_cameraWriteInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWriteInterceptor ENDP


fovWriteInterceptor1 PROC
;LordsOfTheFallen.exe+D2E5F4C - F3 0F11 40 18         - movss [rax+18],xmm0										<< FOV WRITE. Intercept here
;LordsOfTheFallen.exe+D2E5F51 - C6 80 4C010000 01     - mov byte ptr [rax+0000014C],01 
;LordsOfTheFallen.exe+D2E5F58 - 66 C7 80 4E010000 0101 - mov word ptr [rax+0000014E],0101
;LordsOfTheFallen.exe+D2E5F61 - C6 80 50010000 01     - mov byte ptr [rax+00000150],01
;LordsOfTheFallen.exe+D2E5F68 - 48 8B 49 58           - mov rcx,[rcx+58]									    <<< CONTINUE HERE

	;cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	;je continue											; our own camera is enabled, just skip the writes
originalCode:	
	;movss dword ptr [rax+18h],xmm0		
continue:
	mov byte ptr [rax+0000014Ch],01h
	mov word ptr [rax+0000014Eh],0101h
	mov byte ptr [rax+00000150h],01h
exit:
	jmp qword ptr [_fovWriteInterception1Continue]	; jmp back into the original game code, which is the location after the original statements above.
fovWriteInterceptor1 ENDP

fovWriteInterceptor2 PROC
;LordsOfTheFallen.exe+D2E6FFE - F3 0F11 40 18         - movss [rax+18],xmm0								<< FOV WRITE, INTERCEPT HERE
;LordsOfTheFallen.exe+D2E7003 - C6 80 4C010000 01     - mov byte ptr [rax+0000014C],01 { 1 }
;LordsOfTheFallen.exe+D2E700A - 66 C7 80 4E010000 0101 - mov word ptr [rax+0000014E],0101 { 257 }
;LordsOfTheFallen.exe+D2E7013 - C6 80 50010000 01     - mov byte ptr [rax+00000150],01 { 1 }
;LordsOfTheFallen.exe+D2E701A - F3 C3                 - repe ret										<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je continue											; our own camera is enabled, just skip the writes
originalCode:	
	movss dword ptr [rax+18h],xmm0		
continue:
	mov byte ptr [rax+0000014Ch],01h
	mov word ptr [rax+0000014Eh],0101h
	mov byte ptr [rax+00000150h],01h
exit:
	jmp qword ptr [_fovWriteInterception2Continue]	; jmp back into the original game code, which is the location after the original statements above.
fovWriteInterceptor2 ENDP


hudToggleInterceptor PROC
;LordsOfTheFallen.exe+D16AF87 - F3 0F10 43 50         - movss xmm0,[rbx+50]							<< INTERCEPT HERE. READ OF hud visible value
;LordsOfTheFallen.exe+D16AF8C - 48 83 C4 20           - add rsp,20 { 32 }
;LordsOfTheFallen.exe+D16AF90 - 5B                    - pop rbx
;LordsOfTheFallen.exe+D16AF91 - C3                    - ret 
;LordsOfTheFallen.exe+D16AF92 - 00 66 66              - add [rsi+66],ah
;LordsOfTheFallen.exe+D16AF95 - 66 66 2E 0F1F 84 00 00000000  - nop cs:[rax+rax+00000000]			<< NO CONTINUE, SIMPLY RETURN as we swallow the ret in the interception
	; we'll ruin rbx, which is OK, it will be popped anyway.
	cmp byte ptr [g_hudVisible], 0
	je hudHidden
hudVisible:
	movss xmm0, dword ptr [hudVisibleValue]
	jmp originalCode
hudHidden:
	movss xmm0, dword ptr [hudHiddenValue]
originalCode:
	add rsp,20h
	pop rbx
exit:
	ret										; simply return, we overwrote the ret. 
hudToggleInterceptor ENDP


camera2WriteInterceptor1 PROC
;LordsOfTheFallen.exe+D58411A - F3 0F11 49 10         - movss [rcx+10],xmm1						<< INTERCEPT HERE
;LordsOfTheFallen.exe+D58411F - 8B 42 04              - mov eax,[rdx+04]
;LordsOfTheFallen.exe+D584122 - 89 41 14              - mov [rcx+14],eax
;LordsOfTheFallen.exe+D584125 - 8B 42 08              - mov eax,[rdx+08]
;LordsOfTheFallen.exe+D584128 - B2 15                 - mov dl,15
;LordsOfTheFallen.exe+D58412A - 89 41 18              - mov [rcx+18],eax
;LordsOfTheFallen.exe+D58412D - E8 BE010000           - call LordsOfTheFallen.exe+D5842F0		<< CONTINUE HERE

	; check for our coord target address. This code is used by multiple target structures
	cmp qword ptr rcx, [_camera2CoordsAddress]
	jne originalCode									; code operates on other struct than the camera struct, leave it.
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je noCameraWrite									; our own camera is enabled, just skip the writes
	; as we don't know what the registers are used for futher, our 'disabled' part just doesn't write to memory.
originalCode:
	movss dword ptr [rcx+10h],xmm1	
	mov eax, [rdx+04h]
	mov [rcx+14h],eax
	mov eax,[rdx+08h]
	mov dl,15h
	mov [rcx+18h],eax
	jmp exit
noCameraWrite:
	mov eax, [rdx+04h]
	mov eax,[rdx+08h]
	mov dl,15h
exit:
	jmp qword ptr [_camera2WriteInterception1Continue]	; jmp back into the original game code, which is the location after the original statements above.
camera2WriteInterceptor1 ENDP


camera2WriteInterceptor2 PROC
;LordsOfTheFallen.exe+D5841BC - 41 89 40 0C           - mov [r8+0C],eax			<< WRITE QW      <<< INTERCEPT HERE
;LordsOfTheFallen.exe+D5841C0 - 8B 02                 - mov eax,[rdx]
;LordsOfTheFallen.exe+D5841C2 - 41 89 00              - mov [r8],eax			<< WRITE QX
;LordsOfTheFallen.exe+D5841C5 - 8B 42 04              - mov eax,[rdx+04]
;LordsOfTheFallen.exe+D5841C8 - 41 89 40 04           - mov [r8+04],eax			<< WRITE QY
;LordsOfTheFallen.exe+D5841CC - 8B 42 08              - mov eax,[rdx+08]
;LordsOfTheFallen.exe+D5841CF - B2 15                 - mov dl,15 { 21 }
;LordsOfTheFallen.exe+D5841D1 - 41 89 40 08           - mov [r8+08],eax			<< WRITE QZ
;LordsOfTheFallen.exe+D5841D5 - E8 16010000           - call LordsOfTheFallen.exe+D5842F0		<<< CONTINUE HERE

	; check for our quaternion target address. This code is used by multiple target structures
	cmp qword ptr r8, [_camera2QuaternionAddress]
	jne originalCode									; code operates on other struct than the camera struct, leave it.
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je noCameraWrite									; our own camera is enabled, just skip the writes
	; as we don't know what the registers are used for futher, our 'disabled' part just doesn't write to memory.
originalCode:
	mov [r8+0Ch],eax	
	mov eax,[rdx]
	mov [r8],eax	
	mov eax,[rdx+04h]
	mov [r8+04h],eax	
	mov eax,[rdx+08h]
	mov dl,15h
	mov [r8+08h],eax	
	jmp exit
noCameraWrite:
	mov eax,[rdx]
	mov eax,[rdx+04h]
	mov eax,[rdx+08h]
	mov dl,15h
exit:
	jmp qword ptr [_camera2WriteInterception2Continue]	; jmp back into the original game code, which is the location after the original statements above.
camera2WriteInterceptor2 ENDP

END