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
PUBLIC cameraStructInterceptor
PUBLIC cameraWrite1Interceptor
PUBLIC cameraWrite2Interceptor
PUBLIC cameraWrite3Interceptor
PUBLIC fogWriteInterceptor
PUBLIC todWriteInterceptor
PUBLIC gamespeedReadInterceptor

;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_noHeadBob: byte
EXTERN g_cameraStructAddress: qword
EXTERN g_todStructAddress: qword
EXTERN g_gamespeedStructAddress: qword
EXTERN g_fogStructAddress: qword

;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWrite1InterceptionContinue: qword
EXTERN _cameraWrite2InterceptionContinue: qword
EXTERN _cameraWrite3InterceptionContinue: qword
EXTERN _todWriteInterceptionContinue: qword
EXTERN _gamespeedReadInterceptionContinue: qword
EXTERN _fogWriteInterceptionContinue: qword

.data

.code
cameraStructInterceptor PROC
;Engine.dll+365363 - 74 2D                 - je Engine.dll+365392
;Engine.dll+365365 - 48 8B CF              - mov rcx,rdi
;Engine.dll+365368 - E8 A903CCFF           - call Engine.Spider::danAnimationManager::findAnimationSet+C3
;Engine.dll+36536D - 4C 8D 87 F0000000     - lea r8,[rdi+000000F0]
;Engine.dll+365374 - 48 8B CB              - mov rcx,rbx
;Engine.dll+365377 - 48 8D 54 24 60        - lea rdx,[rsp+60]
;Engine.dll+36537C - E8 008ACBFF           - call Engine.CommonCore::spCircuitTLVElement::getCircuitName+127
;Engine.dll+365381 - 48 8B 9C 24 B0000000  - mov rbx,[rsp+000000B0]
;Engine.dll+365389 - 48 81 C4 A0000000     - add rsp,000000A0 { 160 }
;Engine.dll+365390 - 5F                    - pop rdi
;Engine.dll+365391 - C3                    - ret 
;Engine.dll+365392 - 66 0F7F 83 B0000000   - movdqa [rbx+000000B0],xmm0			<< row 1 << INTERCEPT HERE
;Engine.dll+36539A - 66 0F7F 8B C0000000   - movdqa [rbx+000000C0],xmm1			<< row 2
;Engine.dll+3653A2 - 66 0F7F 93 D0000000   - movdqa [rbx+000000D0],xmm2			<< row 3
;Engine.dll+3653AA - 66 0F7F 9B E0000000   - movdqa [rbx+000000E0],xmm3			<< row 4
;Engine.dll+3653B2 - 48 8B 9C 24 B0000000  - mov rbx,[rsp+000000B0]						 << CONTINUE HERE
;Engine.dll+3653BA - 48 81 C4 A0000000     - add rsp,000000A0 { 160 }
;Engine.dll+3653C1 - 5F                    - pop rdi
;Engine.dll+3653C2 - C3                    - ret 
	mov [g_cameraStructAddress], rbx
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movdqa xmmword ptr [rbx+000000B0h],xmm0
	movdqa xmmword ptr [rbx+000000C0h],xmm1
	movdqa xmmword ptr [rbx+000000D0h],xmm2
	movdqa xmmword ptr [rbx+000000E0h],xmm3
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP


cameraWrite1Interceptor PROC
;Engine.dll+2F213E - EB 2F                 - jmp Engine.dll+2F216F
;Engine.dll+2F2140 - 0F28 02               - movaps xmm0,[rdx]
;Engine.dll+2F2143 - 0F28 4A 10            - movaps xmm1,[rdx+10]
;Engine.dll+2F2147 - 0F28 52 20            - movaps xmm2,[rdx+20]
;Engine.dll+2F214B - 0F28 5A 30            - movaps xmm3,[rdx+30]
;Engine.dll+2F214F - 66 0F7F 81 B0000000   - movdqa [rcx+000000B0],xmm0			<< row 1 << INTERCEPT HERE
;Engine.dll+2F2157 - 66 0F7F 89 C0000000   - movdqa [rcx+000000C0],xmm1			<< row 2
;Engine.dll+2F215F - 66 0F7F 91 D0000000   - movdqa [rcx+000000D0],xmm2			<< row 3
;Engine.dll+2F2167 - 66 0F7F 99 E0000000   - movdqa [rcx+000000E0],xmm3			<< row 4
;Engine.dll+2F216F - 0F28 07               - movaps xmm0,[rdi]							 << CONTINUE HERE
;Engine.dll+2F2172 - 0F28 4F 10            - movaps xmm1,[rdi+10]
;Engine.dll+2F2176 - 0F28 57 20            - movaps xmm2,[rdi+20]
;Engine.dll+2F217A - 0F28 5F 30            - movaps xmm3,[rdi+30]
;Engine.dll+2F217E - 66 0F7F 83 F0000000   - movdqa [rbx+000000F0],xmm0
;Engine.dll+2F2186 - 66 0F7F 8B 00010000   - movdqa [rbx+00000100],xmm1
;Engine.dll+2F218E - 66 0F7F 93 10010000   - movdqa [rbx+00000110],xmm2
;Engine.dll+2F2196 - 66 0F7F 9B 20010000   - movdqa [rbx+00000120],xmm3
;Engine.dll+2F219E - 48 8B 9C 24 30010000  - mov rbx,[rsp+00000130]
;Engine.dll+2F21A6 - 48 81 C4 20010000     - add rsp,00000120 { 288 }
;Engine.dll+2F21AD - 5F                    - pop rdi
;Engine.dll+2F21AE - C3                    - ret 
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movdqa xmmword ptr [rcx+000000B0h],xmm0
	movdqa xmmword ptr [rcx+000000C0h],xmm1
	movdqa xmmword ptr [rcx+000000D0h],xmm2
	movdqa xmmword ptr [rcx+000000E0h],xmm3
exit:
	jmp qword ptr [_cameraWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite1Interceptor ENDP


cameraWrite2Interceptor PROC
;Engine.dll+7121A7 - 0FC6 F0 C4            - shufps xmm6,xmm0,-3C { 196 }
;Engine.dll+7121AB - 0F28 C4               - movaps xmm0,xmm4
;Engine.dll+7121AE - 0F29 B7 B0000000      - movaps [rdi+000000B0],xmm6		<< WRITE row 1		<< INTERCEPT HERE
;Engine.dll+7121B5 - 0FC6 87 C0000000 FA   - shufps xmm0,[rdi+000000C0],-06 { 250 }
;Engine.dll+7121BD - 41 0F28 73 F0         - movaps xmm6,[r11-10]
;Engine.dll+7121C2 - 0FC6 E0 C4            - shufps xmm4,xmm0,-3C { 196 }
;Engine.dll+7121C6 - 0F28 C5               - movaps xmm0,xmm5
;Engine.dll+7121C9 - 0F29 A7 C0000000      - movaps [rdi+000000C0],xmm4		<< WRITE row 2
;Engine.dll+7121D0 - 0FC6 87 D0000000 FA   - shufps xmm0,[rdi+000000D0],-06 { 250 }
;Engine.dll+7121D8 - 0FC6 E8 C4            - shufps xmm5,xmm0,-3C { 196 }
;Engine.dll+7121DC - 0F29 AF D0000000      - movaps [rdi+000000D0],xmm5		<< WRITE row 3
;Engine.dll+7121E3 - 49 8B 7B 18           - mov rdi,[r11+18]									<< CONTINUE HERE
;Engine.dll+7121E7 - 49 8B E3              - mov rsp,r11
;Engine.dll+7121EA - 5D                    - pop rbp
;Engine.dll+7121EB - C3                    - ret 
	cmp byte ptr [g_cameraEnabled], 1
	je noWrites
	cmp byte ptr [g_noHeadBob], 1
	jne originalCode
noWrites:
	shufps xmm0,xmmword ptr [rdi+000000C0h],-06h 
	movaps xmm6,xmmword ptr [r11-10h]
	shufps xmm4,xmm0,-3Ch
	movaps xmm0,xmm5
	shufps xmm0,xmmword ptr [rdi+000000D0h],-06h 
	shufps xmm5,xmm0,-3Ch
	jmp exit
originalCode:
	movaps xmmword ptr [rdi+000000B0h],xmm6		
	shufps xmm0,xmmword ptr [rdi+000000C0h],-06h 
	movaps xmm6,xmmword ptr [r11-10h]
	shufps xmm4,xmm0,-3Ch
	movaps xmm0,xmm5
	movaps xmmword ptr [rdi+000000C0h],xmm4		
	shufps xmm0,xmmword ptr [rdi+000000D0h],-06h 
	shufps xmm5,xmm0,-3Ch
	movaps xmmword ptr [rdi+000000D0h],xmm5		
exit:
	jmp qword ptr [_cameraWrite2InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite2Interceptor ENDP


cameraWrite3Interceptor PROC
;Engine.dll+383400 - 48 8B 41 10           - mov rax,[rcx+10]
;Engine.dll+383404 - 48 85 C0              - test rax,rax
;Engine.dll+383407 - 74 38                 - je Engine.dll+383441
;Engine.dll+383409 - 0F28 02               - movaps xmm0,[rdx]
;Engine.dll+38340C - 0F28 4A 10            - movaps xmm1,[rdx+10]
;Engine.dll+383410 - 0F28 52 20            - movaps xmm2,[rdx+20]
;Engine.dll+383414 - 0F28 5A 30            - movaps xmm3,[rdx+30]
;Engine.dll+383418 - 66 0F7F 80 B0000000   - movdqa [rax+000000B0],xmm0			<< row 1 << INTERCEPT HERE
;Engine.dll+383420 - 66 0F7F 88 C0000000   - movdqa [rax+000000C0],xmm1			<< row 2
;Engine.dll+383428 - 66 0F7F 90 D0000000   - movdqa [rax+000000D0],xmm2			<< row 3
;Engine.dll+383430 - 66 0F7F 98 E0000000   - movdqa [rax+000000E0],xmm3			<< row 4
;Engine.dll+383438 - 48 8B 49 10           - mov rcx,[rcx+10]							 << CONTINUE HERE
;Engine.dll+38343C - E9 D522CAFF           - jmp Engine.Spider::danAnimationManager::findAnimationSet+C3
;Engine.dll+383441 - C3                    - ret 
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movdqa xmmword ptr [rax+000000B0h],xmm0
	movdqa xmmword ptr [rax+000000C0h],xmm1
	movdqa xmmword ptr [rax+000000D0h],xmm2
	movdqa xmmword ptr [rax+000000E0h],xmm3
exit:
	jmp qword ptr [_cameraWrite3InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite3Interceptor ENDP


fogWriteInterceptor PROC
;Engine.dll+377E91 - 66 0F7F 83 60010000   - movdqa [rbx+00000160],xmm0						<< FOG Color		<< INTERCEPT HERE
;Engine.dll+377E99 - C6 83 A3020000 01     - mov byte ptr [rbx+000002A3],01 { 1 }
;Engine.dll+377EA0 - 8B 47 40              - mov eax,[rdi+40]
;Engine.dll+377EA3 - 89 83 70010000        - mov [rbx+00000170],eax
;Engine.dll+377EA9 - C6 83 A4020000 01     - mov byte ptr [rbx+000002A4],01 { 1 }
;Engine.dll+377EB0 - 8B 47 44              - mov eax,[rdi+44]
;Engine.dll+377EB3 - 89 83 74010000        - mov [rbx+00000174],eax							<< FOG Factor 2
;Engine.dll+377EB9 - C6 83 A5020000 01     - mov byte ptr [rbx+000002A5],01 { 1 }        
;Engine.dll+377EC0 - 8B 47 48              - mov eax,[rdi+48]                            
;Engine.dll+377EC3 - 89 83 80010000        - mov [rbx+00000180],eax                      	<< FOG Factor 1
;Engine.dll+377EC9 - C6 83 A6020000 01     - mov byte ptr [rbx+000002A6],01 { 1 }        
;Engine.dll+377ED0 - 8B 47 4C              - mov eax,[rdi+4C]                            
;Engine.dll+377ED3 - C6 83 A3020000 01     - mov byte ptr [rbx+000002A3],01 { 1 }        
;Engine.dll+377EDA - 89 83 78010000        - mov [rbx+00000178],eax							<< FOG Factor 3
;Engine.dll+377EE0 - 8B 47 50              - mov eax,[rdi+50]
;Engine.dll+377EE3 - C6 83 A3020000 01     - mov byte ptr [rbx+000002A3],01 { 1 }
;Engine.dll+377EEA - 89 83 7C010000        - mov [rbx+0000017C],eax                      	<< FOG Blend factor
;Engine.dll+377EF0 - 8B 47 54              - mov eax,[rdi+54]								<< CONTINUE HERE
;Engine.dll+377EF3 - C6 83 A3020000 01     - mov byte ptr [rbx+000002A3],01 { 1 }
;Engine.dll+377EFA - 89 83 A8010000        - mov [rbx+000001A8],eax							<< Ring around the sun factor 1
;Engine.dll+377F00 - 8B 47 58              - mov eax,[rdi+58]
;Engine.dll+377F03 - C6 83 A3020000 01     - mov byte ptr [rbx+000002A3],01 { 1 }
;Engine.dll+377F0A - 89 83 AC010000        - mov [rbx+000001AC],eax							<< Ring around the sun factor 2
;Engine.dll+377F10 - 8B 47 5C              - mov eax,[rdi+5C]
	mov [g_fogStructAddress], rbx
	cmp byte ptr [g_cameraEnabled], 1
	jne originalCode
noWrites:
	mov byte ptr [rbx+000002A3h],01h
	mov eax,[rdi+40h]
	mov [rbx+00000170h],eax
	mov byte ptr [rbx+000002A4h],01h
	mov eax,[rdi+44h]
	mov byte ptr [rbx+000002A5h],01h
	mov eax,[rdi+48h]              
	mov byte ptr [rbx+000002A6h],01h
	mov eax,[rdi+4Ch]              
	mov byte ptr [rbx+000002A3h],01h
	mov eax,[rdi+50h]
	mov byte ptr [rbx+000002A3h],01h
	jmp exit
originalCode:
	movdqa xmmword ptr [rbx+00000160h],xmm0
	mov byte ptr [rbx+000002A3h],01h
	mov eax,[rdi+40h]
	mov [rbx+00000170h],eax
	mov byte ptr [rbx+000002A4h],01h
	mov eax,[rdi+44h]
	mov [rbx+00000174h],eax			
	mov byte ptr [rbx+000002A5h],01h
	mov eax,[rdi+48h]              
	mov [rbx+00000180h],eax        
	mov byte ptr [rbx+000002A6h],01h
	mov eax,[rdi+4Ch]              
	mov byte ptr [rbx+000002A3h],01h
	mov [rbx+00000178h],eax			
	mov eax,[rdi+50h]
	mov byte ptr [rbx+000002A3h],01h
	mov [rbx+0000017Ch],eax
exit:
	jmp qword ptr [_fogWriteInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
fogWriteInterceptor ENDP


todWriteInterceptor PROC
;Game.dll+5C1885 - 72 19                 - jb Game.dll+5C18A0
;Game.dll+5C1887 - 8B 43 28              - mov eax,[rbx+28]
;Game.dll+5C188A - 66 0F1F 44 00 00      - nop [rax+rax+00]
;Game.dll+5C1890 - F3 41 0F58 C2         - addss xmm0,xmm10
;Game.dll+5C1895 - FF C0                 - inc eax
;Game.dll+5C1897 - 41 0F2F C0            - comiss xmm0,xmm8
;Game.dll+5C189B - 73 F3                 - jae Game.dll+5C1890
;Game.dll+5C189D - 89 43 28              - mov [rbx+28],eax
;Game.dll+5C18A0 - F3 0F10 4B 34         - movss xmm1,[rbx+34]				<< First address of non-jmp targets. 
;Game.dll+5C18A5 - 41 0F2F C9            - comiss xmm1,xmm9
;Game.dll+5C18A9 - 44 0F28 54 24 50      - movaps xmm10,[rsp+50]			<< INTERCEPT HERE
;Game.dll+5C18AF - 44 0F28 44 24 70      - movaps xmm8,[rsp+70]
;Game.dll+5C18B5 - F3 0F11 43 2C         - movss [rbx+2C],xmm0				<< WRITE ToD. No need to block it as it is read above.
;Game.dll+5C18BA - 76 43                 - jna Game.dll+5C18FF				<< CONTINUE
;Game.dll+5C18BC - F3 0F58 CA            - addss xmm1,xmm2
;Game.dll+5C18C0 - 44 89 6B 34           - mov [rbx+34],r13d
;Game.dll+5C18C4 - C7 44 24 20 05000000  - mov [rsp+20],00000005 { 5 }
;Game.dll+5C18CC - C7 44 24 24 15000000  - mov [rsp+24],00000015 { 21 }
	mov [g_todStructAddress], rbx
	movaps xmm10, xmmword ptr [rsp+50h]
	movaps xmm8, xmmword ptr [rsp+70h]
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rbx+2Ch],xmm0
exit:
	jmp qword ptr [_todWriteInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
todWriteInterceptor ENDP


gamespeedReadInterceptor PROC
;Engine.dll+4E5E1F - EB 03                 - jmp Engine.dll+4E5E24
;Engine.dll+4E5E21 - 0F57 C9               - xorps xmm1,xmm1
;Engine.dll+4E5E24 - F3 0F10 47 24         - movss xmm0,[rdi+24]				<< INTERCEPT HERE << Read gamespeed. 1 is normal, 0 is pause.
;Engine.dll+4E5E29 - F3 0F59 47 20         - mulss xmm0,[rdi+20]
;Engine.dll+4E5E2E - F3 0F59 C1            - mulss xmm0,xmm1
;Engine.dll+4E5E32 - F3 0F5D 05 26A74500   - minss xmm0,[Engine.Spider::danPooledAnimTreeDesc::`vftable'+1AE8] << CONTINUE HERE
;Engine.dll+4E5E3A - F3 0F11 83 90000000   - movss [rbx+00000090],xmm0
;Engine.dll+4E5E42 - 48 8B 5C 24 40        - mov rbx,[rsp+40]
;Engine.dll+4E5E47 - 0F28 7C 24 20         - movaps xmm7,[rsp+20]
;Engine.dll+4E5E4C - 48 83 C4 30           - add rsp,30 { 48 }
;Engine.dll+4E5E50 - 5F                    - pop rdi
;Engine.dll+4E5E51 - C3                    - ret 
	mov [g_gamespeedStructAddress], rdi
	movss xmm0,dword ptr [rdi+24h]
	mulss xmm0,dword ptr [rdi+20h]
	mulss xmm0,xmm1
exit:
	jmp qword ptr [_gamespeedReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
gamespeedReadInterceptor ENDP

END