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
PUBLIC cameraWriteInterceptor
PUBLIC gameSpeedInterceptor
PUBLIC hudToggleInterceptor
PUBLIC timeStopInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraStructAddress: qword
EXTERN g_gameSpeedStructAddress: qword
EXTERN g_hudToggleStructAddress: qword
EXTERN g_timeStopStructAddress: qword
EXTERN g_cameraEnabled: byte
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWriteInterceptionContinue: qword
EXTERN _gameSpeedInterceptorContinue: qword
EXTERN _hudToggleInterceptorContinue: qword
EXTERN _timeStopStructInterceptionContinue: qword

;---------------------------------------------------------------
; Scratch pad
; Better timestop
;DXMD.exe+34B9863 - 83 B9 E4000000 00     - cmp dword ptr [rcx+000000E4],00		<<<< INTERCEPT HERE
;DXMD.exe+34B986A - 77 09                 - ja DXMD.exe+34B9875
;DXMD.exe+34B986C - 83 B9 EC000000 00     - cmp dword ptr [rcx+000000EC],00
;DXMD.exe+34B9873 - 76 02                 - jna DXMD.exe+34B9877
;DXMD.exe+34B9875 - 31 C0                 - xor eax,eax
;DXMD.exe+34B9877 - 83 B9 E8000000 00     - cmp dword ptr [rcx+000000E8],00		<<< TIMESTOP READ
;DXMD.exe+34B987E - 77 09                 - ja DXMD.exe+34B9889
;DXMD.exe+34B9880 - 83 B9 F0000000 00     - cmp dword ptr [rcx+000000F0],00
;DXMD.exe+34B9887 - 76 0B                 - jna DXMD.exe+34B9894
;DXMD.exe+34B9889 - B8 0FA24FC3           - mov eax,C34FA20F
;DXMD.exe+34B988E - 8D 80 F25DB03C        - lea eax,[rax+3CB05DF2]
;DXMD.exe+34B9894 - F3 C3                 - repe ret							<<<< CONTINUE HERE
;---------------------------------------------------------------
.code

cameraAddressInterceptor PROC
; The camera address interceptor uses the FOV read code to obtain the structure address for FOV, which is used to reliably locate the camera matrix as well (as it's relative to the FOV)
; Original code
;DXMD.exe+38496CD - F3 0F10 9B 3C010000   - movss xmm3,[rbx+0000013C]		<<<< INTERCEPT HERE (FOV READ)
;DXMD.exe+38496D5 - F3 0F10 8B 08020000   - movss xmm1,[rbx+00000208]
;DXMD.exe+38496DD - F3 0F10 93 0C020000   - movss xmm2,[rbx+0000020C]
;DXMD.exe+38496E5 - 0F28 E3               - movaps xmm4,xmm3				<<<< CONTINUE HERE
	; Game jmps to this location due to the hook set in C function SetCameraStructInterceptorHook
	mov [g_cameraStructAddress], rbx
originalCode:
	movss xmm3, dword ptr [rbx+0000013Ch]
	movss xmm1, dword ptr [rbx+00000208h]
	movss xmm2, dword ptr [rbx+0000020Ch]
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraAddressInterceptor ENDP


timeStopInterceptor PROC
;DXMD.exe+34BA012 - 48 8D 81 EC000000     - lea rax,[rcx+000000EC]				<<< INTERCEPT HERE
;DXMD.exe+34BA019 - 83 78 F8 00           - cmp dword ptr [rax-08],00			<<<< TIMESTOP READ
;DXMD.exe+34BA01D - 77 15                 - ja DXMD.exe+34BA034
;DXMD.exe+34BA01F - 83 38 00              - cmp dword ptr [rax],00
;DXMD.exe+34BA022 - 77 10                 - ja DXMD.exe+34BA034
;DXMD.exe+34BA024 - FF C2                 - inc edx
;DXMD.exe+34BA026 - 48 83 C0 04           - add rax,04
;DXMD.exe+34BA02A - 83 FA 02              - cmp edx,02
;DXMD.exe+34BA02D - 72 EA                 - jb DXMD.exe+34BA019
;DXMD.exe+34BA02F - 30 C0                 - xor al,al
;DXMD.exe+34BA031 - C3                    - ret 
;DXMD.exe+34BA032 - 00 00                 - add [rax],al
;DXMD.exe+34BA034 - B0 01                 - mov al,01
;DXMD.exe+34BA036 - C3                    - ret									<<< CONTINUE HERE
	mov g_timeStopStructAddress, rcx
	lea rax,[rcx+000000ECh]
check:		
	cmp dword ptr [rax-08h],00	
	ja continue1
	cmp dword ptr [rax],00
	ja continue1
	inc edx
	add rax,04h
	cmp edx,02h
	jb check
	xor al,al
	jmp exit
	add [rax],al
continue1:
	mov al,01h
exit:
	jmp qword ptr [_timeStopStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
timeStopInterceptor ENDP


cameraWriteInterceptor PROC
;DXMD.exe+3550B8F - 0FC6 C2 49            - shufps xmm0,xmm2,49				<<< INTERCEPT HERE
;DXMD.exe+3550B93 - 0F29 4B 50            - movaps [rbx+50],xmm1					<< WRITE MATRIX
;DXMD.exe+3550B97 - 0F28 CA               - movaps xmm1,xmm2
;DXMD.exe+3550B9A - 0FC6 CD 0A            - shufps xmm1,xmm5,0A 
;DXMD.exe+3550B9E - 0F29 43 60            - movaps [rbx+60],xmm0					<< WRITE MATRIX
;DXMD.exe+3550BA2 - 0FC6 CD 98            - shufps xmm1,xmm5,-68
;DXMD.exe+3550BA6 - 0F29 4B 70            - movaps [rbx+70],xmm1					<< WRITE COORDS
;DXMD.exe+3550BAA - C6 43 18 00           - mov byte ptr [rbx+18],00		<<<< CONTINUE HERE
;
; As it's unclear what will happen with the values in xmm1 and xmm0 after the code, we'll execute the original code when the camera is active, but just don't write to memory.
	cmp qword ptr rbx, [g_cameraStructAddress]
	jne originalCode									; code operates on other struct than the camera struct, leave it.
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je noCameraWrite									; our own camera is enabled, just skip the writes
originalCode:
	shufps xmm0,xmm2, 49h
	movaps xmmword ptr [rbx+50h],xmm1
	movaps xmm1,xmm2  
	shufps xmm1,xmm5, 0Ah                 
	movaps xmmword ptr [rbx+60h],xmm0
	shufps xmm1,xmm5, 98h
	movaps xmmword ptr [rbx+70h],xmm1
	jmp exit
noCameraWrite:
	shufps xmm0,xmm2, 49h
	movaps xmm1,xmm2  
	shufps xmm1,xmm5, 0Ah                 
	shufps xmm1,xmm5, 98h
exit:
	jmp qword ptr [_cameraWriteInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWriteInterceptor ENDP

gameSpeedInterceptor PROC
;DXMD.exe+34C3306 - F3 48 0F2C C8         - cvttss2si rcx,xmm0			<<< INTERCEPT HERE
;DXMD.exe+34C330B - 48 0FAF C8            - imul rcx,rax
;DXMD.exe+34C330F - 48 C1 F9 14           - sar rcx,14 { 20 }
;DXMD.exe+34C3313 - 49 89 48 30           - mov [r8+30],rcx				
;DXMD.exe+34C3317 - 49 01 48 20           - add [r8+20],rcx				<<< CONTINUE HERE
	mov [g_gameSpeedStructAddress], r8	; gamespeed address struct is in r8
originalCode:
	cvttss2si rcx,xmm0
	imul rcx,rax
	sar rcx,14h
	mov [r8+30h],rcx					
	jmp qword ptr [_gameSpeedInterceptorContinue]
gameSpeedInterceptor ENDP

hudToggleInterceptor PROC
;DXMD.exe+4304190 - 80 79 20 00           - cmp byte ptr [rcx+20],00			<< INTERCEPT HERE
;DXMD.exe+4304194 - 74 06                 - je DXMD.exe+430419C					(second return)
;DXMD.exe+4304196 - F3 0F10 41 24         - movss xmm0,[rcx+24]			<< HUD Toggle (1.0f is show, 0.0f is hide)
;DXMD.exe+430419B - C3                    - ret 
;DXMD.exe+430419C - F3 0F10 41 28         - movss xmm0,[rcx+28]
;DXMD.exe+43041A1 - C3                    - ret 								<< CONTINUE HERE.
	mov [g_hudToggleStructAddress], rcx
originalCode:
	cmp byte ptr [rcx+20h], 00
	je secondValue
	movss xmm0, dword ptr [rcx+24h]		
	jmp exit
secondValue:
	movss xmm0, dword ptr [rcx+28h]
exit:
	jmp qword ptr [_hudToggleInterceptorContinue]
hudToggleInterceptor ENDP

END