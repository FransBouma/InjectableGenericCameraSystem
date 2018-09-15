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
PUBLIC cameraWrite2Interceptor
PUBLIC cameraWrite3Interceptor
PUBLIC cameraWrite4Interceptor

;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_cameraStructAddress: qword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWrite1InterceptionContinue: qword
EXTERN _cameraWrite2InterceptionContinue: qword
EXTERN _cameraWrite3InterceptionContinue: qword
EXTERN _cameraWrite4InterceptionContinue: qword

.data

;---------------------------------------------------------------
; Scratch pad

.code


cameraStructInterceptor PROC
; copy of matrix to separate one. We'll leave the writes in tact and pre-sub 60 from the value in rbx to make sure we can reliably test for the address later.
;SOTTR.exe+BB1A147 - 89 83 80FFFFFF        - mov [rbx-00000080],eax						<< INTERCEPT HERE 
;SOTTR.exe+BB1A14D - 8B 83 7CFFFFFF        - mov eax,[rbx-00000084]						<< FOV READ
;SOTTR.exe+BB1A153 - 89 83 84FFFFFF        - mov [rbx-0000007C],eax						
;SOTTR.exe+BB1A159 - 0F28 83 A0FFFFFF      - movaps xmm0,[rbx-00000060]					<< CONTINUE HERE << MAT1 READ
;SOTTR.exe+BB1A160 - 66 0F7F 83 E0FFFFFF   - movdqa [rbx-00000020],xmm0					
;SOTTR.exe+BB1A168 - 0F28 8B B0FFFFFF      - movaps xmm1,[rbx-00000050]					<< MAT2 READ
;SOTTR.exe+BB1A16F - 66 0F7F 8B F0FFFFFF   - movdqa [rbx-00000010],xmm1
;SOTTR.exe+BB1A177 - 0F28 83 C0FFFFFF      - movaps xmm0,[rbx-00000040]					<< MAT3 READ
;SOTTR.exe+BB1A17E - 66 0F7F 03            - movdqa [rbx],xmm0
;SOTTR.exe+BB1A182 - 0F28 8B D0FFFFFF      - movaps xmm1,[rbx-00000030]					<< MAT4, coords READ
;SOTTR.exe+BB1A189 - 66 0F7F 4B 10         - movdqa [rbx+10],xmm1
;SOTTR.exe+BB1A18E - 44 38 A3 DD010000     - cmp [rbx+000001DD],r12l
	push rbx
	sub rbx, 60h
	mov [g_cameraStructAddress], rbx
	pop rbx
originalCode:
	mov [rbx-00000080h],eax
	mov eax,[rbx-00000084h]
	mov [rbx-0000007Ch],eax
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP


cameraWrite1Interceptor PROC
;SOTTR.exe+BB1A1A1 - 74 09                 - je SOTTR.exe+BB1A1AC
;SOTTR.exe+BB1A1A3 - 44 39 A0 900F0000     - cmp [rax+00000F90],r12d
;SOTTR.exe+BB1A1AA - 75 13                 - jne SOTTR.exe+BB1A1BF
;SOTTR.exe+BB1A1AC - 48 8B 93 88030000     - mov rdx,[rbx+00000388]
;SOTTR.exe+BB1A1B3 - 48 8D 8B E0010000     - lea rcx,[rbx+000001E0]
;SOTTR.exe+BB1A1BA - E8 818965F4           - call SOTTR.exe+172B40
;SOTTR.exe+BB1A1BF - 0F28 83 E0010000      - movaps xmm0,[rbx+000001E0]					<< JUMP END POINT
;SOTTR.exe+BB1A1C6 - 89 F8                 - mov eax,edi
;SOTTR.exe+BB1A1C8 - 66 0F7F 83 A0FFFFFF   - movdqa [rbx-00000060],xmm0					<< INTERCEPT HERE << MAT1
;SOTTR.exe+BB1A1D0 - 0F28 8B F0010000      - movaps xmm1,[rbx+000001F0]
;SOTTR.exe+BB1A1D7 - 66 0F7F 8B B0FFFFFF   - movdqa [rbx-00000050],xmm1					<< MAT2
;SOTTR.exe+BB1A1DF - 0F28 83 00020000      - movaps xmm0,[rbx+00000200]
;SOTTR.exe+BB1A1E6 - 66 0F7F 83 C0FFFFFF   - movdqa [rbx-00000040],xmm0					<< MAT3
;SOTTR.exe+BB1A1EE - 0F28 8B 10020000      - movaps xmm1,[rbx+00000210]
;SOTTR.exe+BB1A1F5 - 66 0F7F 8B D0FFFFFF   - movdqa [rbx-00000030],xmm1					<< MAT4, coords
;SOTTR.exe+BB1A1FD - 83 FF FF              - cmp edi,-01 { 255 }						<< CONTINUE HERE
;SOTTR.exe+BB1A200 - 7F 06                 - jg SOTTR.exe+BB1A208
	cmp byte ptr [g_cameraEnabled], 1
	jne originalCode
	push rbx
	sub rbx, 60h
	cmp qword ptr rbx, [g_cameraStructAddress]
	pop rbx
	jne originalCode
	jmp exit
originalCode:
	movdqa xmmword ptr [rbx-00000060h],xmm0
	movaps xmm1,xmmword ptr [rbx+000001F0h]
	movdqa xmmword ptr [rbx-00000050h],xmm1
	movaps xmm0,xmmword ptr [rbx+00000200h]
	movdqa xmmword ptr [rbx-00000040h],xmm0
	movaps xmm1,xmmword ptr [rbx+00000210h]
	movdqa xmmword ptr [rbx-00000030h],xmm1
exit:
	jmp qword ptr [_cameraWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite1Interceptor ENDP


cameraWrite2Interceptor PROC
;SOTTR.exe+6DF4A2F - 0FC6 C0 00            - shufps xmm0,xmm000 { 0 }
;SOTTR.exe+6DF4A33 - 0F59 D8               - mulps xmm3,xmm0
;SOTTR.exe+6DF4A36 - 0F28 C4               - movaps xmm0,xmm4
;SOTTR.exe+6DF4A39 - 0FC6 C1 FE            - shufps xmm0,xmm1-02 { 254 }
;SOTTR.exe+6DF4A3D - 0FC6 E0 84            - shufps xmm4,xmm0-7C { 132 }
;SOTTR.exe+6DF4A41 - 41 0F29 67 60         - movaps [r15+60],xmm4				<< INTERCEPT HERE << MAT1 WRITE
;SOTTR.exe+6DF4A46 - 0F28 C3               - movaps xmm0,xmm3
;SOTTR.exe+6DF4A49 - 0FC6 C1 FE            - shufps xmm0,xmm1-02 { 254 }
;SOTTR.exe+6DF4A4D - 0FC6 D8 84            - shufps xmm3,xmm0-7C { 132 }
;SOTTR.exe+6DF4A51 - 0F28 C5               - movaps xmm0,xmm5
;SOTTR.exe+6DF4A54 - 41 0F29 5F 70         - movaps [r15+70],xmm3				<< MAT2 WRITE
;SOTTR.exe+6DF4A59 - 0FC6 C1 FE            - shufps xmm0,xmm1-02 { 254 }		<< CONTINUE HERE
;SOTTR.exe+6DF4A5D - 0FC6 E8 84            - shufps xmm5,xmm0-7C { 132 }
;SOTTR.exe+6DF4A61 - 0F28 C2               - movaps xmm0,xmm2
	cmp byte ptr [g_cameraEnabled], 1
	jne originalCode
	push r15
	add r15, 60h
	cmp qword ptr r15, [g_cameraStructAddress]
	pop r15
	jne originalCode
nowrites:
	movaps xmm0,xmm3
	shufps xmm0,xmm1,-02h
	shufps xmm3,xmm0,-7Ch
	movaps xmm0,xmm5
	jmp exit
originalCode:
	movaps xmmword ptr [r15+60h],xmm4		
	movaps xmm0,xmm3
	shufps xmm0,xmm1,-02h
	shufps xmm3,xmm0,-7Ch
	movaps xmm0,xmm5
	movaps xmmword ptr [r15+70h],xmm3
exit:
	jmp qword ptr [_cameraWrite2InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite2Interceptor ENDP


cameraWrite3Interceptor PROC
;SOTTR.exe+6DF4A64 - 0FC6 05 44E11FFA FE   - shufps xmm0,[SOTTR.exe+FF2BB0]-02 { 254 }
;SOTTR.exe+6DF4A6C - 41 0F29 AF 80000000   - movaps [r15+00000080],xmm5				<< INTERCEPT HERE. MAT3 WRITE
;SOTTR.exe+6DF4A74 - 0FC6 D0 84            - shufps xmm2,xmm0-7C { 132 }
;SOTTR.exe+6DF4A78 - 41 0F29 97 90000000   - movaps [r15+00000090],xmm2				<< MAT4 WRITE, coords.
;SOTTR.exe+6DF4A80 - 48 8B 86 B8FC0100     - mov rax,[rsi+0001FCB8]					<< CONTINUE HERE
;SOTTR.exe+6DF4A87 - C6 86 64020000 01     - mov byte ptr [rsi+00000264],01 { 1 }
;SOTTR.exe+6DF4A8E - 48 85 C0              - test rax,rax
;SOTTR.exe+6DF4A91 - 74 06                 - je SOTTR.exe+6DF4A99
	cmp byte ptr [g_cameraEnabled], 1
	jne originalCode
	push r15
	add r15, 60h
	cmp qword ptr r15, [g_cameraStructAddress]
	pop r15
	jne originalCode
nowrites:
	shufps xmm2,xmm0, -7Ch
	jmp exit
originalCode:
	movaps xmmword ptr [r15+00000080h],xmm5	
	shufps xmm2,xmm0, -7Ch
	movaps xmmword ptr [r15+00000090h],xmm2	
exit:
	jmp qword ptr [_cameraWrite3InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite3Interceptor ENDP


cameraWrite4Interceptor PROC
;SOTTR.exe+BB1A25C - E8 8F72E8F4           - call SOTTR.exe+9A14F0
;SOTTR.exe+BB1A261 - 0F57 D2               - xorps xmm2,xmm2
;SOTTR.exe+BB1A264 - F3 0F11 83 8CFFFFFF   - movss [rbx-00000074],xmm0
;SOTTR.exe+BB1A26C - 0F5C 93 B0FFFFFF      - subps xmm2,[rbx-00000050]
;SOTTR.exe+BB1A273 - 0F28 C2               - movaps xmm0,xmm2
;SOTTR.exe+BB1A276 - 0FC6 83 B0FFFFFF FE   - shufps xmm0,[rbx-00000050]-02 { 254 }		<< INTERCEPT HERE
;SOTTR.exe+BB1A27E - 0FC6 D0 84            - shufps xmm2,xmm0-7C { 132 }
;SOTTR.exe+BB1A282 - 0F29 93 B0FFFFFF      - movaps [rbx-00000050],xmm2					<< MAT2 WRITE
;SOTTR.exe+BB1A289 - E8 92E743F5           - call SOTTR.exe+F58A20						<< CONTINUE HERE
;SOTTR.exe+BB1A28E - 48 89 C1              - mov rcx,rax
;SOTTR.exe+BB1A291 - 48 8B 10              - mov rdx,[rax]
	; statements nowrites and writes both have to do
	shufps xmm0,[rbx-00000050h], -02h
	shufps xmm2,xmm0, -7Ch
	; camera check
	cmp byte ptr [g_cameraEnabled], 1
	jne originalCode
	push rbx
	sub rbx, 60h
	cmp qword ptr rbx, [g_cameraStructAddress]
	pop rbx
	je exit
originalCode:
	movaps xmmword ptr [rbx-00000050h],xmm2		
exit:
	jmp qword ptr [_cameraWrite4InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite4Interceptor ENDP



END