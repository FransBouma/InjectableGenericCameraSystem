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
PUBLIC gamespeedStructInterceptor
PUBLIC todStructInterceptor
PUBLIC hotsampleInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_hotsampleEnabled: byte
EXTERN g_hotsampleFactor: dword
EXTERN g_cameraStructAddress: qword
EXTERN g_gamespeedStructAddress: qword
EXTERN g_todStructAddress: qword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWriteInterceptionContinue: qword
EXTERN _gamespeedStructInterceptorContinue: qword
EXTERN _todStructInterceptorContinue: qword
EXTERN _hotsampleInterceptorContinue: qword

.data

;---------------------------------------------------------------
; Scratch pad
; Thanks to DJH (DET) for this location: manipulate r14d & ebp to get bigger framebuffer for shots!
; This is really nifty and simple. I looked at how to intercept the limiter code for the hotsampling, instead he simply manipulated the
; values passed to the routine, no limiter disable needed. To find this, it was a matter of resizing the window and check where the resolution was stored, then check
; which code this reads. I found that, but didn't think manipulating the values in the registers would bring me anything. 
; 
; To built this in: add keys to increase/decrease the scaling factor: 1, 2, 3, 4, 5, 6, 7, 8. 
; Then build a toggle to enable / disable scaling with this factor. Simply multiply r14d and ebp dwords with the scaling factor. 
; the intercepted code should check whether the toggle is enabled, and if so, it should read the multiply factor and multiply both r14d and ebp with the factor
; otherwise simply skip it. Using a factor and a toggle is better, as you can set the factor to a value and simply toggle hotsampling on for a shot and off again, without
; finetuning the factor. 
;
;Disrupt_64.dll+6AA914C - EB 0F                 - jmp Disrupt_64.dll+6AA915D
;Disrupt_64.dll+6AA914E - 01 21                 - add [rcx],esp
;Disrupt_64.dll+6AA9150 - 44 8B B3 70010000     - mov r14d,[rbx+00000170]			<<< WIDTH READ OF FRAMEBUFFER (DWORD values)
;Disrupt_64.dll+6AA9157 - 8B AB 74010000        - mov ebp,[rbx+00000174]			<<< HEIGHT READ OF FRAMEBUFFER. 
;Disrupt_64.dll+6AA915D - 48 8B 4E 08           - mov rcx,[rsi+08]
;Disrupt_64.dll+6AA9161 - 48 85 C9              - test rcx,rcx
;Disrupt_64.dll+6AA9164 - 0F84 44010000         - je Disrupt_64.dll+6AA92AE
;Disrupt_64.dll+6AA916A - 48 8B 01              - mov rax,[rcx]
;Disrupt_64.dll+6AA916D - 48 8D 54 24 30        - lea rdx,[rsp+30]
;Disrupt_64.dll+6AA9172 - FF 50 60              - call qword ptr [rax+60]
;
;---------------------------------------------------------------
.code


cameraStructInterceptor PROC
; v1.13
;Disrupt_64.dll+51E03AF - F3 0F59 35 098727FE   - mulss xmm6,[Disrupt_64.dll+3458AC0] { [0.02] }
;Disrupt_64.dll+51E03B7 - F3 0F11 40 D8         - movss [rax-28],xmm0
;Disrupt_64.dll+51E03BC - F3 0F11 50 E0         - movss [rax-20],xmm2
;Disrupt_64.dll+51E03C1 - E8 7516FF00           - call Disrupt_64.dll+61D1A3B
;Disrupt_64.dll+51E03C6 - 48 8B 44 24 20        - mov rax,[rsp+20]					   <<< INTERCEPT HERE
;Disrupt_64.dll+51E03CB - 48 89 43 70           - mov [rbx+70],rax					   	   <<< X -axe rotation. Writes 64bits, so also writes roll (y axe rotation)! Angles are 2PI rad.
;Disrupt_64.dll+51E03CF - 8B 44 24 28           - mov eax,[rsp+28]
;Disrupt_64.dll+51E03D3 - 89 43 78              - mov [rbx+78],eax					   		<<< Z axe rotation (UP)
;Disrupt_64.dll+51E03D6 - F3 0F11 73 7C         - movss [rbx+7C],xmm6				   	    <<< FOV WRITE
;Disrupt_64.dll+51E03DB - 8B 83 AC0D0000        - mov eax,[rbx+00000DAC]			   <<< CONTINUE HERE
;Disrupt_64.dll+51E03E1 - 0F28 74 24 30         - movaps xmm6,[rsp+30]
;Disrupt_64.dll+51E03E6 - 89 43 58              - mov [rbx+58],eax
;Disrupt_64.dll+51E03E9 - 8B 83 B00D0000        - mov eax,[rbx+00000DB0]

; v1.11
;Disrupt_64.dll+50E323C - F3 0F59 35 74952BFE   - mulss xmm6,[Disrupt_64.dll+339C7B8] { [0.02] }
;Disrupt_64.dll+50E3244 - F3 0F11 40 D8         - movss [rax-28],xmm0
;Disrupt_64.dll+50E3249 - F3 0F11 50 E0         - movss [rax-20],xmm2
;Disrupt_64.dll+50E324E - E8 DD3C2101           - call Disrupt_64.dll+62F6F30
;Disrupt_64.dll+50E3253 - 48 8B 44 24 20        - mov rax,[rsp+20]					   <<< INTERCEPT HERE
;Disrupt_64.dll+50E3258 - 48 89 43 70           - mov [rbx+70],rax							   <<< X -axe rotation. Writes 64bits, so also writes roll (y axe rotation)! Angles are 2PI rad.
;Disrupt_64.dll+50E325C - 8B 44 24 28           - mov eax,[rsp+28]
;Disrupt_64.dll+50E3260 - 89 43 78              - mov [rbx+78],eax								<<< Z axe rotation (UP)
;Disrupt_64.dll+50E3263 - F3 0F11 73 7C         - movss [rbx+7C],xmm6						    <<< FOV WRITE
;Disrupt_64.dll+50E3268 - 8B 83 DC150000        - mov eax,[rbx+000015DC]			   <<< CONTINUE HERE
;Disrupt_64.dll+50E326E - 0F28 74 24 30         - movaps xmm6,[rsp+30]				   
;Disrupt_64.dll+50E3273 - 89 43 58              - mov [rbx+58],eax
;Disrupt_64.dll+50E3276 - 8B 83 E0150000        - mov eax,[rbx+000015E0]

	mov [g_cameraStructAddress], rbx
	mov rax, [rsp+20h]
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	mov [rbx+70h], rax						; write rotation angles around X and Y (pitch / roll, as Y is into the screen)
	mov eax, [rsp+28h]
	mov [rbx+78h],eax									; write rotation angle around Z (up)
	movss dword ptr [rbx+7Ch],xmm6						; write FOV
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP

cameraWriteInterceptor PROC
; v1.13
;Disrupt_64.dll+61D1A3B - 48 83 EC 28           - sub rsp,28 { 40 }
;Disrupt_64.dll+61D1A3F - 8B 02                 - mov eax,[rdx]
;Disrupt_64.dll+61D1A41 - 4C 8D 41 64           - lea r8,[rcx+64]
;Disrupt_64.dll+61D1A45 - 41 89 00              - mov [r8],eax								<<< INTERCEPT HERE		<<<< X WRITE
;Disrupt_64.dll+61D1A48 - 8B 42 04              - mov eax,[rdx+04]
;Disrupt_64.dll+61D1A4B - 41 89 40 04           - mov [r8+04],eax									<<<< Y WRITE
;Disrupt_64.dll+61D1A4F - 8B 42 08              - mov eax,[rdx+08]
;Disrupt_64.dll+61D1A52 - 41 89 40 08           - mov [r8+08],eax									<<<< Z WRITE
;Disrupt_64.dll+61D1A56 - 48 8B 0D 7BB5D5FD     - mov rcx,[Disrupt_64.dll+3F2CFD8]			<<< CONTINUE HERE
;Disrupt_64.dll+61D1A5D - 48 85 C9              - test rcx,rcx
;Disrupt_64.dll+61D1A60 - 74 35                 - je Disrupt_64.dll+61D1A97
;Disrupt_64.dll+61D1A62 - F3 0F10 05 22A134FD   - movss xmm0,[Disrupt_64.dll+351BB8C] 
;Disrupt_64.dll+61D1A6A - F3 0F10 0D C2595CFD   - movss xmm1,[Disrupt_64.dll+3797434] 
;Disrupt_64.dll+61D1A72 - F3 0F10 15 0EFC29FD   - movss xmm2,[Disrupt_64.dll+3471688] 
;Disrupt_64.dll+61D1A7A - 4C 8D 4C 24 30        - lea r9,[rsp+30]
;Disrupt_64.dll+61D1A7F - 48 83 C1 78           - add rcx,78 { 120 }
;Disrupt_64.dll+61D1A83 - 4C 89 C2              - mov rdx,r8
;Disrupt_64.dll+61D1A86 - F3 0F11 44 24 30      - movss [rsp+30],xmm0
;Disrupt_64.dll+61D1A8C - F3 0F11 4C 24 34      - movss [rsp+34],xmm1
;Disrupt_64.dll+61D1A92 - E8 89B02800           - call Disrupt_64.dll+645CB20
;Disrupt_64.dll+61D1A97 - 48 83 C4 28           - add rsp,28 { 40 }
;Disrupt_64.dll+61D1A9B - C3                    - ret 

;v 1.11
;Disrupt_64.dll+62F6F30 - 48 83 EC 28           - sub rsp,28 { 40 }
;Disrupt_64.dll+62F6F34 - 8B 02                 - mov eax,[rdx]
;Disrupt_64.dll+62F6F36 - 4C 8D 41 64           - lea r8,[rcx+64]									
;Disrupt_64.dll+62F6F3A - 41 89 00              - mov [r8],eax								<<< INTERCEPT HERE		<<<< X WRITE
;Disrupt_64.dll+62F6F3D - 8B 42 04              - mov eax,[rdx+04]
;Disrupt_64.dll+62F6F40 - 41 89 40 04           - mov [r8+04],eax									<<<< Y WRITE
;Disrupt_64.dll+62F6F44 - 8B 42 08              - mov eax,[rdx+08]
;Disrupt_64.dll+62F6F47 - 41 89 40 08           - mov [r8+08],eax									<<<< Z WRITE
;Disrupt_64.dll+62F6F4B - 48 8B 0D 564EB6FD     - mov rcx,[Disrupt_64.dll+3E5BDA8]			<<< CONTINUE HERE
;Disrupt_64.dll+62F6F52 - 48 85 C9              - test rcx,rcx
;Disrupt_64.dll+62F6F55 - 74 35                 - je Disrupt_64.dll+62F6F8C
;Disrupt_64.dll+62F6F57 - F3 0F10 05 558416FD   - movss xmm0,[Disrupt_64.dll+345F3B4]
;Disrupt_64.dll+62F6F5F - F3 0F10 0D C52B3DFD   - movss xmm1,[Disrupt_64.dll+36C9B2C]
;Disrupt_64.dll+62F6F67 - F3 0F10 15 49EA0BFD   - movss xmm2,[Disrupt_64.dll+33B59B8]
;Disrupt_64.dll+62F6F6F - 4C 8D 4C 24 30        - lea r9,[rsp+30]
;Disrupt_64.dll+62F6F74 - 48 83 C1 78           - add rcx,78 { 120 }
;Disrupt_64.dll+62F6F78 - 4C 89 C2              - mov rdx,r8
;Disrupt_64.dll+62F6F7B - F3 0F11 44 24 30      - movss [rsp+30],xmm0
;Disrupt_64.dll+62F6F81 - F3 0F11 4C 24 34      - movss [rsp+34],xmm1
;Disrupt_64.dll+62F6F87 - E8 A4D72700           - call Disrupt_64.dll+6574730
;Disrupt_64.dll+62F6F8C - 48 83 C4 28           - add rsp,28 { 40 }
;Disrupt_64.dll+62F6F90 - C3                    - ret 
	; check for our coord target address. This code is used by multiple target structures
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	mov [r8],eax		
	mov eax,[rdx+04h]
	mov [r8+04h],eax	
	mov eax,[rdx+08h]
	mov [r8+08h],eax	
exit:
	jmp qword ptr [_cameraWriteInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWriteInterceptor ENDP


gamespeedStructInterceptor PROC
;v1.13
;Disrupt_64.dll+49BE533 - F2 0F5C 4B 20         - subsd xmm1,[rbx+20]
;Disrupt_64.dll+49BE538 - 74 0F                 - je Disrupt_64.dll+49BE549
;Disrupt_64.dll+49BE53A - F2 0F10 0D FE51ABFE   - movsd xmm1,[Disrupt_64.dll+3473740]
;Disrupt_64.dll+49BE542 - C6 83 9C000000 00     - mov byte ptr [rbx+0000009C],00
;Disrupt_64.dll+49BE549 - 80 BB 9D000000 00     - cmp byte ptr [rbx+0000009D],00
;Disrupt_64.dll+49BE550 - 75 11                 - jne Disrupt_64.dll+49BE563
;Disrupt_64.dll+49BE552 - F2 0F10 93 80000000   - movsd xmm2,[rbx+00000080]						<<< INTERCEPT HERE	<<<< READ GAME SPEED HERE.
;Disrupt_64.dll+49BE55A - 66 0F2F CA            - comisd xmm1,xmm2
;Disrupt_64.dll+49BE55E - 72 03                 - jb Disrupt_64.dll+49BE563
;Disrupt_64.dll+49BE560 - 0F28 CA               - movaps xmm1,xmm2
;Disrupt_64.dll+49BE563 - 80 BB 91000000 00     - cmp byte ptr [rbx+00000091],00				<<< CONTINUE HERE
;Disrupt_64.dll+49BE56A - F2 0F11 43 20         - movsd [rbx+20],xmm0
;Disrupt_64.dll+49BE56F - 74 1A                 - je Disrupt_64.dll+49BE58B

;v1.11
;Disrupt_64.dll+48E03B3 - F2 0F5C 4B 20         - subsd xmm1,[rbx+20]
;Disrupt_64.dll+48E03B8 - 74 0F                 - je Disrupt_64.dll+48E03C9
;Disrupt_64.dll+48E03BA - F2 0F10 0D 4E76ADFE   - movsd xmm1,[Disrupt_64.dll+33B7A10]
;Disrupt_64.dll+48E03C2 - C6 83 9C000000 00     - mov byte ptr [rbx+0000009C],00
;Disrupt_64.dll+48E03C9 - 80 BB 9D000000 00     - cmp byte ptr [rbx+0000009D],00
;Disrupt_64.dll+48E03D0 - 75 11                 - jne Disrupt_64.dll+48E03E3
;Disrupt_64.dll+48E03D2 - F2 0F10 93 80000000   - movsd xmm2,[rbx+00000080]						<<< INTERCEPT HERE	<<<< READ GAME SPEED HERE.
;Disrupt_64.dll+48E03DA - 66 0F2F CA            - comisd xmm1,xmm2
;Disrupt_64.dll+48E03DE - 72 03                 - jb Disrupt_64.dll+48E03E3
;Disrupt_64.dll+48E03E0 - 0F28 CA               - movaps xmm1,xmm2
;Disrupt_64.dll+48E03E3 - 80 BB 91000000 00     - cmp byte ptr [rbx+00000091],00 { 0 }			<<< CONTINUE HERE
;Disrupt_64.dll+48E03EA - F2 0F11 43 20         - movsd [rbx+20],xmm0
;Disrupt_64.dll+48E03EF - 74 1A                 - je Disrupt_64.dll+48E040B
	mov [g_gamespeedStructAddress], rbx	
	movsd xmm2, qword ptr [rbx+00000080h]
	comisd xmm1,xmm2
	jb exit
	movaps xmm1,xmm2
exit:
	jmp qword ptr [_gamespeedStructInterceptorContinue]	; jmp back into the original game code, which is the location after the original statements above.
gamespeedStructInterceptor ENDP

todStructInterceptor PROC
;v1.13
;Disrupt_64.dll+5A1C23C - 4C 8D 4C 24 58        - lea r9,[rsp+58]
;Disrupt_64.dll+5A1C241 - 4C 8D 44 24 48        - lea r8,[rsp+48]
;Disrupt_64.dll+5A1C246 - 48 8B 08              - mov rcx,[rax]					<<< INTERCEPT HERE. TOD READ
;Disrupt_64.dll+5A1C249 - 48 8D 54 24 50        - lea rdx,[rsp+50]
;Disrupt_64.dll+5A1C24E - 48 89 4C 24 20        - mov [rsp+20],rcx
;Disrupt_64.dll+5A1C253 - 48 8D 4C 24 20        - lea rcx,[rsp+20]
;Disrupt_64.dll+5A1C258 - E8 0312B300           - call Disrupt_64.dll+654D460	<< CONTINUE HERE

;v1.11
;Disrupt_64.dll+590DC79 - 4C 8D 4C 24 58        - lea r9,[rsp+58]
;Disrupt_64.dll+590DC7E - 4C 8D 44 24 48        - lea r8,[rsp+48]
;Disrupt_64.dll+590DC83 - 48 8B 08              - mov rcx,[rax]					<<< INTERCEPT HERE. TOD READ
;Disrupt_64.dll+590DC86 - 48 8D 54 24 50        - lea rdx,[rsp+50]
;Disrupt_64.dll+590DC8B - 48 89 4C 24 20        - mov [rsp+20],rcx
;Disrupt_64.dll+590DC90 - 48 8D 4C 24 20        - lea rcx,[rsp+20]
;Disrupt_64.dll+590DC95 - E8 8950D500           - call Disrupt_64.dll+6662D23   << CONTINUE HERE
;
	mov [g_todStructAddress], rax
originalCode:
	mov rcx,[rax]		
	lea rdx,[rsp+50h]
	mov [rsp+20h],rcx
	lea rcx,[rsp+20h]
exit:
	jmp qword ptr [_todStructInterceptorContinue]	; jmp back into the original game code, which is the location after the original statements above.
todStructInterceptor ENDP


hotsampleInterceptor PROC
;Disrupt_64.dll+6F052F0 - 44 8B B3 A0010000     - mov r14d,[rbx+000001A0]			<<< INTERCEPT HERE <<< WIDTH READ OF FRAMEBUFFER (DWORD values)
;Disrupt_64.dll+6F052F7 - 8B AB A4010000        - mov ebp,[rbx+000001A4]			<<< HEIGHT READ OF FRAMEBUFFER. 
;Disrupt_64.dll+6F052FD - 48 8B 4E 08           - mov rcx,[rsi+08]
;Disrupt_64.dll+6F05301 - 48 85 C9              - test rcx,rcx						<<< CONTINUE HERE
;Disrupt_64.dll+6F05304 - 0F84 42010000         - je Disrupt_64.dll+6F0544C
;Disrupt_64.dll+6F0530A - 48 8B 01              - mov rax,[rcx]
	push rax
	mov eax, [g_hotsampleFactor]
	mov r14d, dword ptr [rbx+000001A0h]					; read width
	mov ebp, dword ptr [rbx+000001A4h]						; read height
	cmp byte ptr [g_hotsampleEnabled], 1
	jne originalCode				
	; hotsampling enabled. Multiply both registers with factor, using integer math
	imul r14d, eax
	imul ebp, eax
originalCode:
	mov rcx, [rsi+08h]
exit:
	pop rax
	jmp qword ptr [_hotsampleInterceptorContinue]	; jmp back into the original game code, which is the location after the original statements above.
hotsampleInterceptor ENDP


END