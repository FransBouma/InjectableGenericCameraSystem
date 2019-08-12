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
PUBLIC cameraInitInterceptor

;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_cameraDataInitialized: byte

;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraInitInterceptionContinue: qword

.data

.code
cameraInitInterceptor PROC
;WHGame.DLL+1F8A641 - F3 0F10 06            - movss xmm0,[rsi]
;WHGame.DLL+1F8A645 - 48 8D 0D 546F6F01     - lea rcx,[WHGame.DLL+36815A0] { (711.31) }
;WHGame.DLL+1F8A64C - F3 0F11 05 4C6F6F01   - movss [WHGame.DLL+36815A0],xmm0 { (711.31) } >>>>>>> Intercept here
;WHGame.DLL+1F8A654 - F3 0F10 4E 04         - movss xmm1,[rsi+04]
;WHGame.DLL+1F8A659 - F3 0F11 0D 436F6F01   - movss [WHGame.DLL+36815A4],xmm1 { (3458.19) }
;WHGame.DLL+1F8A661 - 48 8D 98 E8000000     - lea rbx,[rax+000000E8]
;WHGame.DLL+1F8A668 - F3 0F10 46 08         - movss xmm0,[rsi+08]
;WHGame.DLL+1F8A66D - F3 0F11 05 336F6F01   - movss [WHGame.DLL+36815A8],xmm0 { (66.34) }
;WHGame.DLL+1F8A675 - F3 0F10 4E 0C         - movss xmm1,[rsi+0C]
;WHGame.DLL+1F8A67A - F3 0F11 0D 2A6F6F01   - movss [WHGame.DLL+36815AC],xmm1 { (0.02) }
;WHGame.DLL+1F8A682 - F3 0F10 46 10         - movss xmm0,[rsi+10]
;WHGame.DLL+1F8A687 - F3 0F11 05 216F6F01   - movss [WHGame.DLL+36815B0],xmm0 { (-0.05) }
;WHGame.DLL+1F8A68F - F3 0F10 4E 14         - movss xmm1,[rsi+14]
;WHGame.DLL+1F8A694 - F3 0F11 0D 186F6F01   - movss [WHGame.DLL+36815B4],xmm1 { (0.95) }
;WHGame.DLL+1F8A69C - F3 0F10 46 18         - movss xmm0,[rsi+18]
;WHGame.DLL+1F8A6A1 - F3 0F11 05 0F6F6F01   - movss [WHGame.DLL+36815B8],xmm0 { (-0.32) }
;WHGame.DLL+1F8A6A9 - F3 0F10 4E 30         - movss xmm1,[rsi+30]							<<<<<<<< Continue here.
;WHGame.DLL+1F8A6AE - F3 0F59 0D 52973A00   - mulss xmm1,[WHGame.DLL+2333E08] { (180.00) }
;WHGame.DLL+1F8A6B6 - F3 0F5E 0D 72973A00   - divss xmm1,[WHGame.DLL+2333E30] { (3.14) }
;WHGame.DLL+1F8A6BE - F3 0F11 0D F66E6F01   - movss [WHGame.DLL+36815BC],xmm1 { (65.00) }

	; if the camera isn't enabled we'll execute all code regardless
	; if the camera is enabled, we'll execute the writes once. 
	cmp byte ptr [g_cameraEnabled], 1
	jne originalCode
	cmp byte ptr [g_cameraDataInitialized], 1
	je exit
originalCode:
	movss dword ptr [rcx],xmm0
	movss xmm1,dword ptr [rsi+04h]
	movss dword ptr [rcx+04h],xmm1
	movss xmm0,dword ptr [rsi+08h]
	movss dword ptr [rcx+08h],xmm0
	movss xmm1,dword ptr [rsi+0Ch]
	movss dword ptr [rcx+0ch],xmm1
	movss xmm0,dword ptr [rsi+10h]
	movss dword ptr [rcx+10h],xmm0
	movss xmm1,dword ptr [rsi+14h]
	movss dword ptr [rcx+14h],xmm1
	movss xmm0,dword ptr [rsi+18h]
	movss dword ptr [rcx+18h],xmm0
	mov byte ptr [g_cameraDataInitialized], 1
exit:
	lea rbx,[rax+000000E8h]
	jmp qword ptr [_cameraInitInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraInitInterceptor ENDP

END