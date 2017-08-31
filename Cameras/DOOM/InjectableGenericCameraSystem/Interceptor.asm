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
PUBLIC fovInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraStructAddress: qword
EXTERN g_cameraEnabled: byte
EXTERN g_fovStructAddress: qword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _hostBaseAddress: qword
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _fovInterceptorContinue: qword

;---------------------------------------------------------------
; Scratch pad
;
; Timestop is tricky. They likely use some sort of function but couldn't find it. The timestop I found through the menu stops the engine environment and animations
; but not the movement of the nps, so they'll slide along over the rails they're on. Not usable. 
;DOOMx64.exe+1403D30 - 40 53                 - push rbx
;DOOMx64.exe+1403D32 - 41 56                 - push r14
;DOOMx64.exe+1403D34 - 48 83 EC 28           - sub rsp,28 { 40 }
;DOOMx64.exe+1403D38 - 83 3D 912D2103 00     - cmp dword ptr [DOOMx64.exe+4616AD0],00 		// read timestop for photomode, which we'll use
;DOOMx64.exe+1403D3F - 44 0FB6 F2            - movzx r14d,dl
;DOOMx64.exe+1403D43 - 48 8B D9              - mov rbx,rcx
;DOOMx64.exe+1403D46 - 74 0D                 - je DOOMx64.exe+1403D55
;DOOMx64.exe+1403D48 - 83 3D F12C2103 00     - cmp dword ptr [DOOMx64.exe+4616A40],00 
;DOOMx64.exe+1403D4F - 0F84 42010000         - je DOOMx64.exe+1403E97
;DOOMx64.exe+1403D55 - 48 89 7C 24 50        - mov [rsp+50],rdi
;DOOMx64.exe+1403D5A - 4C 89 7C 24 20        - mov [rsp+20],r15
;DOOMx64.exe+1403D5F - 45 33 FF              - xor r15d,r15d
;DOOMx64.exe+1403D62 - 4C 89 79 50           - mov [rcx+50],r15
;DOOMx64.exe+1403D66 - 44 89 79 4C           - mov [rcx+4C],r15d
;DOOMx64.exe+1403D6A - 44 88 79 58           - mov [rcx+58],r15l
;
; For reference, this is the timestop through the menu, but not usable:
;DOOMx64.exe+118670 - 80 B9 2B010000 00     - cmp byte ptr [rcx+0000012B],00			<<<< INTERCEPT HERE. Read timestop flag
;DOOMx64.exe+118677 - 75 28                 - jne DOOMx64.exe+1186A1
;DOOMx64.exe+118679 - 48 8B 51 10           - mov rdx,[rcx+10]
;DOOMx64.exe+11867D - 8B 42 0C              - mov eax,[rdx+0C]							
;DOOMx64.exe+118680 - 89 42 10              - mov [rdx+10],eax
;DOOMx64.exe+118683 - 48 8B 51 10           - mov rdx,[rcx+10]
;DOOMx64.exe+118687 - 8B 42 04              - mov eax,[rdx+04]
;DOOMx64.exe+11868A - 01 42 0C              - add [rdx+0C],eax
;DOOMx64.exe+11868D - 48 8B 51 10           - mov rdx,[rcx+10]
;DOOMx64.exe+118691 - 8B 42 30              - mov eax,[rdx+30]
;DOOMx64.exe+118694 - 89 42 34              - mov [rdx+34],eax
;DOOMx64.exe+118697 - 48 8B 49 10           - mov rcx,[rcx+10]
;DOOMx64.exe+11869B - 8B 41 28              - mov eax,[rcx+28]
;DOOMx64.exe+11869E - 01 41 30              - add [rcx+30],eax
;DOOMx64.exe+1186A1 - F3 C3                 - repe ret									<<<< CONTINUE HERE. Whole function needed due to jne above.
;
; in-game DOF:
;DOOMx64.exe+AE1819 - 83 3D 90594E03 00     - cmp dword ptr [DOOMx64.exe+3FC71B0],00			<<< DOF ENABLE/DISABLE
;DOOMx64.exe+AE1820 - 74 50                 - je DOOMx64.exe+AE1872
;DOOMx64.exe+AE1822 - 83 3D 175A4E03 00     - cmp dword ptr [DOOMx64.exe+3FC7240],00			<<< Unclear what this does.
;DOOMx64.exe+AE1829 - 0F95 C0               - setne al
;DOOMx64.exe+AE182C - 88 87 3C264B00        - mov [rdi+004B263C],al
;DOOMx64.exe+AE1832 - F3 0F10 05 9A5A4E03   - movss xmm0,[DOOMx64.exe+3FC72D4] { [00000000] }
;DOOMx64.exe+AE183A - F3 0F11 87 40264B00   - movss [rdi+004B2640],xmm0
;DOOMx64.exe+AE1842 - F3 0F10 0D 1A5B4E03   - movss xmm1,[DOOMx64.exe+3FC7364] { [512.00] }		<<< FAR PLANE
;DOOMx64.exe+AE184A - F3 0F11 8F 44264B00   - movss [rdi+004B2644],xmm1
;DOOMx64.exe+AE1852 - F3 0F10 05 9A5B4E03   - movss xmm0,[DOOMx64.exe+3FC73F4] { [300.00] }	    <<< NEAR PLANE
;DOOMx64.exe+AE185A - F3 0F11 87 48264B00   - movss [rdi+004B2648],xmm0
;DOOMx64.exe+AE1862 - F3 0F10 0D 1A5C4E03   - movss xmm1,[DOOMx64.exe+3FC7484] { [1.00] }		<<< BLUR STRENGTH
;DOOMx64.exe+AE186A - F3 0F11 8F 4C264B00   - movss [rdi+004B264C],xmm1
;DOOMx64.exe+AE1872 - 48 8B 05 47503603     - mov rax,[DOOMx64.exe+3E468C0] { [FB184C9800] }
;
; HUD toggle
;DOOMx64.exe+93FAD8 - 48 8B 88 B0000000     - mov rcx,[rax+000000B0]
;DOOMx64.exe+93FADF - 83 79 30 00           - cmp dword ptr [rcx+30],00 { 0 }			<<< HUD Toggle read. 1 == hud visible. not all elements are hidden.
;DOOMx64.exe+93FAE3 - 0F84 970F0000         - je DOOMx64.exe+940A80
;DOOMx64.exe+93FAE9 - 41 8B 45 08           - mov eax,[r13+08]
;DOOMx64.exe+93FAED - 4C 89 B4 24 E0080000  - mov [rsp+000008E0],r14
;DOOMx64.exe+93FAF5 - 41 0B 45 00           - or eax,[r13+00]
;DOOMx64.exe+93FAF9 - 41 0B 45 04           - or eax,[r13+04]
;DOOMx64.exe+93FAFD - A9 FFFFFF7F           - test eax,7FFFFFFF { 2147483647 }
;DOOMx64.exe+93FB02 - 0F84 5A140000         - je DOOMx64.exe+940F62
;DOOMx64.exe+93FB08 - 4C 8B B5 48080000     - mov r14,[rbp+00000848]
;DOOMx64.exe+93FB0F - 41 8B 06              - mov eax,[r14]
;DOOMx64.exe+93FB12 - 41 0B 46 08           - or eax,[r14+08]
;
;---------------------------------------------------------------
.code

cameraAddressInterceptor PROC
;DOOMx64.exe+930DF8 - F3 0F10 4C 24 40      - movss xmm1,[rsp+40]
;DOOMx64.exe+930DFE - F3 0F11 B7 C0000000   - movss [rdi+000000C0],xmm6								<< INTERCEPT HERE
;DOOMx64.exe+930E06 - F3 0F11 BF C4000000   - movss [rdi+000000C4],xmm7
;DOOMx64.exe+930E0E - F3 44 0F11 87 C8000000  - movss [rdi+000000C8],xmm8
;DOOMx64.exe+930E17 - F3 44 0F11 8F CC000000  - movss [rdi+000000CC],xmm9
;DOOMx64.exe+930E20 - F3 44 0F11 A7 D8000000  - movss [rdi+000000D8],xmm12
;DOOMx64.exe+930E29 - F3 44 0F11 97 D0000000  - movss [rdi+000000D0],xmm10
;DOOMx64.exe+930E32 - F3 44 0F11 9F D4000000  - movss [rdi+000000D4],xmm11
;DOOMx64.exe+930E3B - F3 44 0F11 AF DC000000  - movss [rdi+000000DC],xmm13
;DOOMx64.exe+930E44 - F3 44 0F11 B7 E0000000  - movss [rdi+000000E0],xmm14
;DOOMx64.exe+930E4D - F3 44 0F11 BF E4000000  - movss [rdi+000000E4],xmm15
;DOOMx64.exe+930E56 - F3 0F11 87 E8000000   - movss [rdi+000000E8],xmm0
;DOOMx64.exe+930E5E - F3 0F11 8F EC000000   - movss [rdi+000000EC],xmm1
;DOOMx64.exe+930E66 - F3 0F10 05 9A694C03   - movss xmm0,[DOOMx64.exe+3DF7808] { [00000000] }		<< CONTINUE HERE
;DOOMx64.exe+930E6E - 48 8B CF              - mov rcx,rdi
	; Game jmps to this location due to the hook set in C function SetCameraStructInterceptorHook
	; The game uses one block of code which is only used to write the new coords + camera matrix. We can simply combine the address interception and 
	; write disable in one. 
	mov [g_cameraStructAddress], rdi
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rdi+000000C0h],xmm6
	movss dword ptr [rdi+000000C4h],xmm7
	movss dword ptr [rdi+000000C8h],xmm8
	movss dword ptr [rdi+000000CCh],xmm9
	movss dword ptr [rdi+000000D8h],xmm12
	movss dword ptr [rdi+000000D0h],xmm10
	movss dword ptr [rdi+000000D4h],xmm11
	movss dword ptr [rdi+000000DCh],xmm13
	movss dword ptr [rdi+000000E0h],xmm14
	movss dword ptr [rdi+000000E4h],xmm15
	movss dword ptr [rdi+000000E8h],xmm0
	movss dword ptr [rdi+000000ECh],xmm1
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraAddressInterceptor ENDP


fovInterceptor PROC
; Game jmps to this location due to the hook set in C function setFoVInterceptorHook
; rdi contains the base address of the struct which contains the pointer to the fov value
; rdi + 0000CE74h is the fov struct address, which contains the fov at offset 10h. We have to write our
; fov to that address. Reset it with the value hostImageAddress + 3DA8064h as that's the setting's fov address.
;DOOMx64.exe+916467 - 48 8B 0D 621F3D03     - mov rcx,[DOOMx64.exe+3CE83D0]						
;DOOMx64.exe+91646E - 0F29 7C 24 60         - movaps [rsp+60],xmm7								<<< INTERCEPT HERE
;DOOMx64.exe+916473 - 44 0F29 44 24 50      - movaps [rsp+50],xmm8								
;DOOMx64.exe+916479 - 48 8D 94 24 90000000  - lea rdx,[rsp+00000090]
;DOOMx64.exe+916481 - 41 B8 01000000        - mov r8d,00000001 { 1 }							<<< CONTINUE HERE
;DOOMx64.exe+916487 - 48 8B 01              - mov rax,[rcx]
;DOOMx64.exe+91648A - 44 0F29 4C 24 40      - movaps [rsp+40],xmm9
;DOOMx64.exe+916490 - 44 0F29 54 24 30      - movaps [rsp+30],xmm10
;DOOMx64.exe+916496 - 44 0F29 5C 24 20      - movaps [rsp+20],xmm11
;DOOMx64.exe+91649C - FF 90 10080000        - call qword ptr [rax+00000810]
;DOOMx64.exe+9164A2 - 48 8D 8F 74CE0000     - lea rcx,[rdi+0000CE74]
;DOOMx64.exe+9164A9 - 48 8B D0              - mov rdx,rax
	push rax
	lea	rax, [rdi+000CE74h]
	mov [g_fovStructAddress], rax
	pop rax
originalCode:
	movaps [rsp+60h],xmm7	
	movaps [rsp+50h],xmm8	
	lea rdx,[rsp+00000090h]
exit:
	jmp qword ptr [_fovInterceptorContinue]
fovInterceptor ENDP

END