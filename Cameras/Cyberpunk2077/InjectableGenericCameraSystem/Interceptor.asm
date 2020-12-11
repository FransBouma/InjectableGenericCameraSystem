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
PUBLIC pmStructAddressInterceptor
PUBLIC activeCamAddressInterceptor
PUBLIC activeCamWrite1Interceptor
PUBLIC resolutionStructAddressInterceptor

;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_pmStructAddress: qword
EXTERN g_activeCamStructAddress: qword
EXTERN g_resolutionStructAddress: qword

;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _pmStructAddressInterceptionContinue: qword
EXTERN _activeCamAddressInterceptionContinue: qword
EXTERN _activeCamWrite1InterceptionContinue: qword
EXTERN _resolutionStructAddressInterceptionContinue: qword

.data

.code


activeCamAddressInterceptor PROC
;Cyberpunk2077.exe+FED742 - 8B 41 C8              - mov eax,[rcx-38]
;Cyberpunk2077.exe+FED745 - 48 8B F2              - mov rsi,rdx
;Cyberpunk2077.exe+FED748 - 89 42 08              - mov [rdx+08],eax
;Cyberpunk2077.exe+FED74B - 0F10 41 D0            - movups xmm0,[rcx-30]
;Cyberpunk2077.exe+FED74F - 48 8B CB              - mov rcx,rbx
;Cyberpunk2077.exe+FED752 - 0F11 42 10            - movups [rdx+10],xmm0
;Cyberpunk2077.exe+FED756 - 48 8B 03              - mov rax,[rbx]
;Cyberpunk2077.exe+FED759 - FF 90 58020000        - call qword ptr [rax+00000258]		<< INTERCEPT HERE >> Call get fov . RCX contains pointer to active camera.
;Cyberpunk2077.exe+FED75F - F3 0F11 46 20         - movss [rsi+20],xmm0
;Cyberpunk2077.exe+FED764 - 48 8D 54 24 20        - lea rdx,[rsp+20]
;Cyberpunk2077.exe+FED769 - 48 8B 03              - mov rax,[rbx]						<< CONTINUE HERE
;Cyberpunk2077.exe+FED76C - 48 8B CB              - mov rcx,rbx
;Cyberpunk2077.exe+FED76F - FF 90 60020000        - call qword ptr [rax+00000260]
;Cyberpunk2077.exe+FED775 - 4C 8D 46 40           - lea r8,[rsi+40]
;Cyberpunk2077.exe+FED779 - 48 8B CB              - mov rcx,rbx
;Cyberpunk2077.exe+FED77C - 48 8D 56 3C           - lea rdx,[rsi+3C]
;Cyberpunk2077.exe+FED780 - 0F10 00               - movups xmm0,[rax]
;Cyberpunk2077.exe+FED783 - 0F11 46 24            - movups [rsi+24],xmm0
;Cyberpunk2077.exe+FED787 - F2 0F10 48 10         - movsd xmm1,[rax+10]
;Cyberpunk2077.exe+FED78C - F2 0F11 4E 34         - movsd [rsi+34],xmm1
;Cyberpunk2077.exe+FED791 - 48 8B 03              - mov rax,[rbx]
;Cyberpunk2077.exe+FED794 - FF 90 70020000        - call qword ptr [rax+00000270]
;Cyberpunk2077.exe+FED79A - 48 8B 03              - mov rax,[rbx]
;Cyberpunk2077.exe+FED79D - 4C 8D 46 50           - lea r8,[rsi+50]
;Cyberpunk2077.exe+FED7A1 - 48 8D 56 4C           - lea rdx,[rsi+4C]
	mov [g_activeCamStructAddress], rcx
	call qword ptr [rax+00000258h]
	movss dword ptr [rsi+20h],xmm0
	lea rdx,[rsp+20h]
exit:
	jmp qword ptr [_activeCamAddressInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
activeCamAddressInterceptor ENDP


activeCamWrite1Interceptor PROC
; Writes to many destinations but blocking all these writes doesn't have side effects. However blocking all writes regardless whether it's targeting our
; struct will also block writes when the pm isn't enabled. So we'll check if the destination address is our freecam struct. 
;Cyberpunk2077.exe+10B129A - E8 219013FF           - call Cyberpunk2077.exe+1EA2C0
;Cyberpunk2077.exe+10B129F - 0F10 40 10            - movups xmm0,[rax+10]
;Cyberpunk2077.exe+10B12A3 - 8B 08                 - mov ecx,[rax]
;Cyberpunk2077.exe+10B12A5 - 89 4C 24 20           - mov [rsp+20],ecx
;Cyberpunk2077.exe+10B12A9 - 8B 48 04              - mov ecx,[rax+04]
;Cyberpunk2077.exe+10B12AC - 0F29 44 24 30         - movaps [rsp+30],xmm0
;Cyberpunk2077.exe+10B12B1 - 89 4C 24 24           - mov [rsp+24],ecx
;Cyberpunk2077.exe+10B12B5 - 8B 48 08              - mov ecx,[rax+08]
;Cyberpunk2077.exe+10B12B8 - F2 0F10 44 24 20      - movsd xmm0,[rsp+20]
;Cyberpunk2077.exe+10B12BE - F2 0F11 83 E0000000   - movsd [rbx+000000E0],xmm0				<< INTERCEPT HERE << Write coords in packed int32 format
;Cyberpunk2077.exe+10B12C6 - 0F28 44 24 30         - movaps xmm0,[rsp+30]
;Cyberpunk2077.exe+10B12CB - 89 8B E8000000        - mov [rbx+000000E8],ecx
;Cyberpunk2077.exe+10B12D1 - 0F11 83 F0000000      - movups [rbx+000000F0],xmm0				<< Write quaternion
;Cyberpunk2077.exe+10B12D8 - 80 BB B1000000 00     - cmp byte ptr [rbx+000000B1],00 { 0 }	<< CONTINUE HERE
;Cyberpunk2077.exe+10B12DF - 75 0E                 - jne Cyberpunk2077.exe+10B12EF
;Cyberpunk2077.exe+10B12E1 - 80 BB B0000000 00     - cmp byte ptr [rbx+000000B0],00 { 0 }
;Cyberpunk2077.exe+10B12E8 - 75 05                 - jne Cyberpunk2077.exe+10B12EF
;Cyberpunk2077.exe+10B12EA - 40 32 FF              - xor dil,dil
;Cyberpunk2077.exe+10B12ED - EB 03                 - jmp Cyberpunk2077.exe+10B12F2
;Cyberpunk2077.exe+10B12EF - 40 B7 01              - mov dil,01 { 1 }
;Cyberpunk2077.exe+10B12F2 - 44 3B BB E0000000     - cmp r15d,[rbx+000000E0]
;Cyberpunk2077.exe+10B12F9 - 4C 8B BC 24 A0000000  - mov r15,[rsp+000000A0]
;Cyberpunk2077.exe+10B1301 - 75 2F                 - jne Cyberpunk2077.exe+10B1332
	cmp rbx, [g_activeCamStructAddress]
	jne originalCode
	cmp byte ptr [g_cameraEnabled], 1
	jne originalCode
noWrites:
	movaps xmm0, xmmword ptr  [rsp+30h]
	jmp exit
originalCode:
	movsd qword ptr [rbx+000000E0h],xmm0	
	movaps xmm0, xmmword ptr  [rsp+30h]
	mov [rbx+000000E8h],ecx
	movups xmmword ptr [rbx+000000F0h],xmm0
exit:
	jmp qword ptr [_activeCamWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
activeCamWrite1Interceptor ENDP

pmStructAddressInterceptor PROC
;Cyberpunk2077.exe+25C8521 - 0F59 C4               - mulps xmm0,xmm4
;Cyberpunk2077.exe+25C8524 - 0FC6 FF 00            - shufps xmm7,xmm7,00 { 0 }
;Cyberpunk2077.exe+25C8528 - 0F55 F8               - andnps xmm7,xmm0
;Cyberpunk2077.exe+25C852B - 0F11 BE C0010000        - movups [rsi+000001C0],xmm7		<< INTERCEPT HERE << Quaternion storage. RSI contains pm struct.
;Cyberpunk2077.exe+25C8532 - F3 44 0F11 9E D0010000  - movss [rsi+000001D0],xmm11		<< X storage
;Cyberpunk2077.exe+25C853B - F3 44 0F11 96 D4010000  - movss [rsi+000001D4],xmm10		<< CONTINUE HERE << Y
;Cyberpunk2077.exe+25C8544 - F3 44 0F11 8E D8010000  - movss [rsi+000001D8],xmm9		<< Z
;Cyberpunk2077.exe+25C854D - 48 89 9E DC010000     - mov [rsi+000001DC],rbx
;Cyberpunk2077.exe+25C8554 - 0F29 BD 10010000      - movaps [rbp+00000110],xmm7
;Cyberpunk2077.exe+25C855B - 0F28 DF               - movaps xmm3,xmm7
;Cyberpunk2077.exe+25C855E - 89 9E E4010000        - mov [rsi+000001E4],ebx
;Cyberpunk2077.exe+25C8564 - 0F59 DF               - mulps xmm3,xmm7
;Cyberpunk2077.exe+25C8567 - 48 8B CE              - mov rcx,rsi
;Cyberpunk2077.exe+25C856A - 88 9E F4010000        - mov [rsi+000001F4],bl
;Cyberpunk2077.exe+25C8570 - 88 9E A0010000        - mov [rsi+000001A0],bl
;Cyberpunk2077.exe+25C8576 - 89 BE 8C010000        - mov [rsi+0000018C],edi
;Cyberpunk2077.exe+25C857C - C7 45 0C 0000803F     - mov [rbp+0C],3F800000 { 1,00 }
;Cyberpunk2077.exe+25C8583 - 0F28 C3               - movaps xmm0,xmm3
;Cyberpunk2077.exe+25C8586 - 0F28 D3               - movaps xmm2,xmm3
;Cyberpunk2077.exe+25C8589 - 0FC6 C3 FF            - shufps xmm0,xmm3,-01 { 255 }
;Cyberpunk2077.exe+25C858D - 0F28 CB               - movaps xmm1,xmm3
;Cyberpunk2077.exe+25C8590 - 0FC6 D3 AA            - shufps xmm2,xmm3,-56 { 170 }
	mov [g_pmStructAddress], rsi
	movups xmmword ptr [rsi+000001C0h],xmm7
	movss dword ptr [rsi+000001D0h],xmm11
exit:
	jmp qword ptr [_pmStructAddressInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
pmStructAddressInterceptor ENDP


resolutionStructAddressInterceptor PROC
;Cyberpunk2077.exe+26C3F4B - 80 BB BF000000 00     - cmp byte ptr [rbx+000000BF],00 { 0 }
;Cyberpunk2077.exe+26C3F52 - 0F85 92000000         - jne Cyberpunk2077.exe+26C3FEA
;Cyberpunk2077.exe+26C3F58 - 0FB6 83 82000000      - movzx eax,byte ptr [rbx+00000082]
;Cyberpunk2077.exe+26C3F5F - 38 43 3C              - cmp [rbx+3C],al
;Cyberpunk2077.exe+26C3F62 - 0F85 82000000         - jne Cyberpunk2077.exe+26C3FEA
;Cyberpunk2077.exe+26C3F68 - 8B 43 74              - mov eax,[rbx+74]					>> rbx contains resolution struct pointer. 0x74 is WIDTH
;Cyberpunk2077.exe+26C3F6B - 39 43 18              - cmp [rbx+18],eax
;Cyberpunk2077.exe+26C3F6E - 75 7A                 - jne Cyberpunk2077.exe+26C3FEA
;Cyberpunk2077.exe+26C3F70 - 8B 43 78              - mov eax,[rbx+78]					>> HEIGHT 
;Cyberpunk2077.exe+26C3F73 - 39 43 1C              - cmp [rbx+1C],eax
;Cyberpunk2077.exe+26C3F76 - 75 72                 - jne Cyberpunk2077.exe+26C3FEA
;Cyberpunk2077.exe+26C3F78 - 0FB6 83 81000000      - movzx eax,byte ptr [rbx+00000081]
;Cyberpunk2077.exe+26C3F7F - 38 43 28              - cmp [rbx+28],al
;Cyberpunk2077.exe+26C3F82 - 75 66                 - jne Cyberpunk2077.exe+26C3FEA
;Cyberpunk2077.exe+26C3F84 - 3A 8B 80000000        - cmp cl,[rbx+00000080]
;Cyberpunk2077.exe+26C3F8A - 75 5E                 - jne Cyberpunk2077.exe+26C3FEA
;Cyberpunk2077.exe+26C3F8C - 8B 43 7C              - mov eax,[rbx+7C]
;Cyberpunk2077.exe+26C3F8F - 39 43 68              - cmp [rbx+68],eax
;Cyberpunk2077.exe+26C3F92 - 75 56                 - jne Cyberpunk2077.exe+26C3FEA
;Cyberpunk2077.exe+26C3F94 - 0FB6 43 3E            - movzx eax,byte ptr [rbx+3E]
;Cyberpunk2077.exe+26C3F98 - 38 83 83000000        - cmp [rbx+00000083],al
;Cyberpunk2077.exe+26C3F9E - 75 4A                 - jne Cyberpunk2077.exe+26C3FEA
;Cyberpunk2077.exe+26C3FA0 - 0FB6 83 9C000000      - movzx eax,byte ptr [rbx+0000009C]
;Cyberpunk2077.exe+26C3FA7 - 38 83 C0000000        - cmp [rbx+000000C0],al
;Cyberpunk2077.exe+26C3FAD - 75 3B                 - jne Cyberpunk2077.exe+26C3FEA
;// however this isn't hookable due to all the jne's. So we hook a bit higher in the function:
;Cyberpunk2077.exe+26C3E5D - 8B 81 84000000        - mov eax,[rcx+00000084]			<< INTERCEPT HERE
;Cyberpunk2077.exe+26C3E63 - 89 41 44              - mov [rcx+44],eax
;Cyberpunk2077.exe+26C3E66 - 8B 81 88000000        - mov eax,[rcx+00000088]
;Cyberpunk2077.exe+26C3E6C - 89 41 40              - mov [rcx+40],eax
;Cyberpunk2077.exe+26C3E6F - 8B 81 8C000000        - mov eax,[rcx+0000008C]			<< CONTINUE HERE
;Cyberpunk2077.exe+26C3E75 - 89 41 48              - mov [rcx+48],eax
;Cyberpunk2077.exe+26C3E78 - 8B 81 90000000        - mov eax,[rcx+00000090]
;Cyberpunk2077.exe+26C3E7E - 89 41 4C              - mov [rcx+4C],eax
;Cyberpunk2077.exe+26C3E81 - 8B 81 98000000        - mov eax,[rcx+00000098]
;Cyberpunk2077.exe+26C3E87 - 89 41 54              - mov [rcx+54],eax
;Cyberpunk2077.exe+26C3E8A - 8B 81 94000000        - mov eax,[rcx+00000094]
;Cyberpunk2077.exe+26C3E90 - 89 41 50              - mov [rcx+50],eax
;Cyberpunk2077.exe+26C3E93 - 0FB6 49 20            - movzx ecx,byte ptr [rcx+20]
;Cyberpunk2077.exe+26C3E97 - 80 F9 03              - cmp cl,03 { 3 }
;Cyberpunk2077.exe+26C3E9A - 0F85 AB000000         - jne Cyberpunk2077.exe+26C3F4B
;Cyberpunk2077.exe+26C3EA0 - 8B 43 74              - mov eax,[rbx+74]
;Cyberpunk2077.exe+26C3EA3 - 39 43 18              - cmp [rbx+18],eax
;Cyberpunk2077.exe+26C3EA6 - 75 73                 - jne Cyberpunk2077.exe+26C3F1B
;Cyberpunk2077.exe+26C3EA8 - 8B 43 78              - mov eax,[rbx+78]
;Cyberpunk2077.exe+26C3EAB - 39 43 1C              - cmp [rbx+1C],eax
;Cyberpunk2077.exe+26C3EAE - 75 6B                 - jne Cyberpunk2077.exe+26C3F1B
	mov [g_resolutionStructAddress], rbx
	mov eax,[rcx+00000084h]
	mov [rcx+44h],eax
	mov eax,[rcx+00000088h]
	mov [rcx+40h],eax
exit:
	jmp qword ptr [_resolutionStructAddressInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
resolutionStructAddressInterceptor ENDP

END