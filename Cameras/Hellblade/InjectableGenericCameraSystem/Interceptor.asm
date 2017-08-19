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
PUBLIC timestopInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_gamePaused: byte
EXTERN g_cameraStructAddress: qword
EXTERN g_fovValue: dword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _fovReadInterceptionContinue: qword
EXTERN _cameraWriteInterceptionContinue: qword
EXTERN _timestopInterceptionContinue: qword
.data
;---------------------------------------------------------------
; Scratch pad
.code


cameraStructInterceptor PROC
;// Used in-game and in photomode
;// UE uses angles for rotation. In UE3 it uses packed 16 bit ints, in UE4 it uses floats, in degrees (0-360.0)
;Relocated code. Simply skip the writes if the camera is enabled.
;HellbladeGame-Win64-Shipping.exe+1940B55 - 48 8D 55 90           - lea rdx,[rbp-70]					// 0x7FF6947B0B55
;HellbladeGame-Win64-Shipping.exe+1940B59 - 8B 88 F8080000        - mov ecx,[rax+000008F8]
;HellbladeGame-Win64-Shipping.exe+1940B5F - 8B 44 24 58           - mov eax,[rsp+58]
;HellbladeGame-Win64-Shipping.exe+1940B63 - 89 8F 70040000        - mov [rdi+00000470],ecx
;HellbladeGame-Win64-Shipping.exe+1940B69 - 48 8D 8F C0040000     - lea rcx,[rdi+000004C0]
;HellbladeGame-Win64-Shipping.exe+1940B70 - 83 A7 AC040000 FC     - and dword ptr [rdi+000004AC],-04
;HellbladeGame-Win64-Shipping.exe+1940B77 - F2 0F11 87 80040000   - movsd [rdi+00000480],xmm0		<< INTERCEPT HERE. Write X & Y
;HellbladeGame-Win64-Shipping.exe+1940B7F - F2 0F10 44 24 5C      - movsd xmm0,[rsp+5C]
;HellbladeGame-Win64-Shipping.exe+1940B85 - F2 0F11 87 8C040000   - movsd [rdi+0000048C],xmm0			// Write Rotation around X/ around Z
;HellbladeGame-Win64-Shipping.exe+1940B8D - 0F10 44 24 68         - movups xmm0,[rsp+68]
;HellbladeGame-Win64-Shipping.exe+1940B92 - 89 87 88040000        - mov [rdi+00000488],eax				// Write Z (UP)
;HellbladeGame-Win64-Shipping.exe+1940B98 - 8B 44 24 64           - mov eax,[rsp+64]
;HellbladeGame-Win64-Shipping.exe+1940B9C - 89 87 94040000        - mov [rdi+00000494],eax				// Write Rotation around Y (into the screen)
;HellbladeGame-Win64-Shipping.exe+1940BA2 - 8B 44 24 7C           - mov eax,[rsp+7C]				<< CONTINUE HERE
;HellbladeGame-Win64-Shipping.exe+1940BA6 - 0F11 87 98040000      - movups [rdi+00000498],xmm0			// Write FOV
;HellbladeGame-Win64-Shipping.exe+1940BAD - 83 E0 03              - and eax,03 { 3 }					
;HellbladeGame-Win64-Shipping.exe+1940BB0 - F3 0F10 44 24 78      - movss xmm0,[rsp+78]
;HellbladeGame-Win64-Shipping.exe+1940BB6 - 09 87 AC040000        - or [rdi+000004AC],eax
	mov [g_cameraStructAddress], rdi
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit
originalCode:
	movsd qword ptr [rdi+480h],xmm0
	movsd xmm0,qword ptr [rsp+5Ch]
	movsd qword ptr [rdi+48Ch],xmm0
	movups xmm0,xmmword ptr [rsp+68h]
	mov dword ptr [rdi+488h],eax
	mov eax,dword ptr [rsp+64h]
	mov dword ptr [rdi+494h],eax
	;mov eax,dword ptr [rsp+7Ch]
	;movups xmmword ptr [rdi+498h],xmm0
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP

cameraWriteInterceptor PROC
; Menu camera block write. We'll block it out when camera is enabled. 
;HellbladeGame-Win64-Shipping.exe+1959211 - F2 0F11 83 80040000   - movsd [rbx+00000480],xmm0			<< INTERCEPT HERE
;HellbladeGame-Win64-Shipping.exe+1959219 - F2 0F10 44 24 2C      - movsd xmm0,[rsp+2C]
;HellbladeGame-Win64-Shipping.exe+195921F - F2 0F11 83 8C040000   - movsd [rbx+0000048C],xmm0
;HellbladeGame-Win64-Shipping.exe+1959227 - 0F10 44 24 38         - movups xmm0,[rsp+38]
;HellbladeGame-Win64-Shipping.exe+195922C - 89 83 88040000        - mov [rbx+00000488],eax
;HellbladeGame-Win64-Shipping.exe+1959232 - 8B 44 24 34           - mov eax,[rsp+34]
;HellbladeGame-Win64-Shipping.exe+1959236 - 89 83 94040000        - mov [rbx+00000494],eax
;HellbladeGame-Win64-Shipping.exe+195923C - 8B 44 24 4C           - mov eax,[rsp+4C]					<<< CONTINUE HERE
;HellbladeGame-Win64-Shipping.exe+1959240 - 0F11 83 98040000      - movups [rbx+00000498],xmm0			
;HellbladeGame-Win64-Shipping.exe+1959247 - 83 E0 03              - and eax,03 { 3 }
;HellbladeGame-Win64-Shipping.exe+195924A - F3 0F10 44 24 48      - movss xmm0,[rsp+48]
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit
originalCode:
	movsd qword ptr [rbx+00000480h],xmm0
	movsd xmm0, qword ptr [rsp+2Ch]
	movsd qword ptr [rbx+0000048Ch],xmm0
	movups xmm0, xmmword ptr [rsp+38h]
	mov dword ptr [rbx+00000488h],eax
	mov eax, dword ptr [rsp+34h]
	mov dword ptr [rbx+00000494h],eax
exit:
	jmp qword ptr [_cameraWriteInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWriteInterceptor ENDP



fovReadInterceptor PROC
;HellbladeGame-Win64-Shipping.exe+1944DD0 - F3 0F10 81 14040000   - movss xmm0,[rcx+00000414]			<< INTERCEPT HERE
;HellbladeGame-Win64-Shipping.exe+1944DD8 - 0F57 C9               - xorps xmm1,xmm1
;HellbladeGame-Win64-Shipping.exe+1944DDB - 0F2F C1               - comiss xmm0,xmm1
;HellbladeGame-Win64-Shipping.exe+1944DDE - 77 08                 - ja HellbladeGame-Win64-Shipping.exe+1944DE8
;HellbladeGame-Win64-Shipping.exe+1944DE0 - F3 0F10 81 98040000   - movss xmm0,[rcx+00000498]         // FOV READ
;HellbladeGame-Win64-Shipping.exe+1944DE8 - C3                    - ret									<< CONTINUE HERE
;
; If the camera is enabled, we'll simply return our own fov value in xmm0. Otherwise we'll execute the original code. 
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	jne originalCode
returnOurOwn:
	push rcx
	mov ecx, [g_fovValue]
	movd xmm0, ecx
	pop rcx
	jmp exit
originalCode:
	movss xmm0,dword ptr [rcx+414h]
	xorps xmm1,xmm1                             
	comiss xmm0,xmm1                            
	ja exit
	movss xmm0,dword ptr [rcx+498h]
exit:
	jmp qword ptr [_fovReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
fovReadInterceptor ENDP


timestopInterceptor PROC
; UE4 doesn't use a byte to signal a paused game, though the cheat manager could in theory be used for that, it's not always available
; In the engine it calls this function, which will return 1 for a paused game, or 0 if not. 
; We'll intercept the start, and simply return 1 in al if the user pressed numpad 0 and return immediately, or continue the original code if the 
; user hasn't paused the game. The complete code is given below for future reference of timestop in UE powered games.
;hellbladeGame-Win64-Shipping.exe+1801AE0 - 40 53                 - push rbx												<<< INTERCEPT HERE
;HellbladeGame-Win64-Shipping.exe+1801AE2 - 48 83 EC 20           - sub rsp,20 { 32 }
;HellbladeGame-Win64-Shipping.exe+1801AE6 - 41 B0 01              - mov r8l,01 { 1 }
;HellbladeGame-Win64-Shipping.exe+1801AE9 - 33 D2                 - xor edx,edx
;HellbladeGame-Win64-Shipping.exe+1801AEB - 48 8B D9              - mov rbx,rcx
;HellbladeGame-Win64-Shipping.exe+1801AEE - E8 3D972B00           - call HellbladeGame-Win64-Shipping.exe+1ABB230			<<< CONTINUE HERE
;HellbladeGame-Win64-Shipping.exe+1801AF3 - 48 85 C0              - test rax,rax
;HellbladeGame-Win64-Shipping.exe+1801AF6 - 74 1B                 - je HellbladeGame-Win64-Shipping.exe+1801B13
;HellbladeGame-Win64-Shipping.exe+1801AF8 - 48 83 B8 70050000 00  - cmp qword ptr [rax+00000570],00 { 0 }
;HellbladeGame-Win64-Shipping.exe+1801B00 - 74 11                 - je HellbladeGame-Win64-Shipping.exe+1801B13
;HellbladeGame-Win64-Shipping.exe+1801B02 - F3 0F10 83 0C090000   - movss xmm0,[rbx+0000090C]
;HellbladeGame-Win64-Shipping.exe+1801B0A - 0F2F 83 F8080000      - comiss xmm0,[rbx+000008F8]
;HellbladeGame-Win64-Shipping.exe+1801B11 - 76 5E                 - jna HellbladeGame-Win64-Shipping.exe+1801B71
;HellbladeGame-Win64-Shipping.exe+1801B13 - F6 83 A0090000 08     - test byte ptr [rbx+000009A0],08 { 8 }
;HellbladeGame-Win64-Shipping.exe+1801B1A - 74 0D                 - je HellbladeGame-Win64-Shipping.exe+1801B29
;HellbladeGame-Win64-Shipping.exe+1801B1C - 48 8B CB              - mov rcx,rbx
;HellbladeGame-Win64-Shipping.exe+1801B1F - E8 2CAF2B00           - call HellbladeGame-Win64-Shipping.exe+1ABCA50
;HellbladeGame-Win64-Shipping.exe+1801B24 - 83 F8 03              - cmp eax,03 { 3 }
;HellbladeGame-Win64-Shipping.exe+1801B27 - 74 48                 - je HellbladeGame-Win64-Shipping.exe+1801B71
;HellbladeGame-Win64-Shipping.exe+1801B29 - 48 8B 0D A87C0E02     - mov rcx,[HellbladeGame-Win64-Shipping.exe+38E97D8] { [DDD130100] }
;HellbladeGame-Win64-Shipping.exe+1801B30 - 48 8B D3              - mov rdx,rbx
;HellbladeGame-Win64-Shipping.exe+1801B33 - E8 08292800           - call HellbladeGame-Win64-Shipping.exe+1A84440
;HellbladeGame-Win64-Shipping.exe+1801B38 - 84 C0                 - test al,al
;HellbladeGame-Win64-Shipping.exe+1801B3A - 75 35                 - jne HellbladeGame-Win64-Shipping.exe+1801B71
;HellbladeGame-Win64-Shipping.exe+1801B3C - F7 83 A0090000 00000200 - test [rbx+000009A0],20000 { 131072 }
;HellbladeGame-Win64-Shipping.exe+1801B46 - 75 29                 - jne HellbladeGame-Win64-Shipping.exe+1801B71
;HellbladeGame-Win64-Shipping.exe+1801B48 - E8 3332F5FF           - call HellbladeGame-Win64-Shipping.exe+1754D80
;HellbladeGame-Win64-Shipping.exe+1801B4D - 85 C0                 - test eax,eax
;HellbladeGame-Win64-Shipping.exe+1801B4F - 75 20                 - jne HellbladeGame-Win64-Shipping.exe+1801B71
;HellbladeGame-Win64-Shipping.exe+1801B51 - 48 8B CB              - mov rcx,rbx
;HellbladeGame-Win64-Shipping.exe+1801B54 - E8 57B02B00           - call HellbladeGame-Win64-Shipping.exe+1ABCBB0
;HellbladeGame-Win64-Shipping.exe+1801B59 - 84 C0                 - test al,al
;HellbladeGame-Win64-Shipping.exe+1801B5B - 74 0C                 - je HellbladeGame-Win64-Shipping.exe+1801B69
;HellbladeGame-Win64-Shipping.exe+1801B5D - F7 83 A0090000 00100000 - test [rbx+000009A0],1000 { 4096 }
;HellbladeGame-Win64-Shipping.exe+1801B67 - 75 08                 - jne HellbladeGame-Win64-Shipping.exe+1801B71
;HellbladeGame-Win64-Shipping.exe+1801B69 - 32 C0                 - xor al,al											<<<< return 0, game isn't paused
;HellbladeGame-Win64-Shipping.exe+1801B6B - 48 83 C4 20           - add rsp,20 { 32 }
;HellbladeGame-Win64-Shipping.exe+1801B6F - 5B                    - pop rbx
;HellbladeGame-Win64-Shipping.exe+1801B70 - C3                    - ret 
;HellbladeGame-Win64-Shipping.exe+1801B71 - B0 01                 - mov al,01 { 1 }										<<<< return 1, game is paused.
;HellbladeGame-Win64-Shipping.exe+1801B73 - 48 83 C4 20           - add rsp,20 { 32 }
;HellbladeGame-Win64-Shipping.exe+1801B77 - 5B                    - pop rbx
;HellbladeGame-Win64-Shipping.exe+1801B78 - C3                    - ret 

	cmp byte ptr [g_gamePaused], 0
	je originalCode						; game not paused, continue as if nothing happened
gamePaused:
	; simply return 1 in al and return, as we have to return the call here not in the original code
	mov al, 01h
	ret
originalCode:
	push rbx					
	sub rsp,20h
	mov r8b,01h
	xor edx,edx
	mov rbx,rcx
exit:
	jmp qword ptr [_timestopInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
timestopInterceptor ENDP

END