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
PUBLIC timeDilationInterceptor
PUBLIC dofEnableInterceptor 
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: BYTE
EXTERN g_cameraStructAddress: qword
EXTERN g_dofStructAddress : qword
EXTERN g_timeDilationAddress : qword;
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWrite1InterceptionContinue: qword
EXTERN _timeDilationInterceptionContinue: qword
EXTERN _dofEnableInterceptionContinue: qword
.data

;---------------------------------------------------------------
; Scratch pad
;---------------------------------------------------------------
.code


cameraStructInterceptor PROC
;000000014ACF1AB0 | F2:0F1081 84050000            | movsd xmm0,qword ptr ds:[rcx+584]    <<< Camera in RCX			<< INTERCEPT HERE
;000000014ACF1AB8 | F2:0F1102                     | movsd qword ptr ds:[rdx],xmm0        
;000000014ACF1ABC | 8B81 8C050000                 | mov eax,dword ptr ds:[rcx+58C]       
;000000014ACF1AC2 | 8942 08                       | mov dword ptr ds:[rdx+8],eax         
;000000014ACF1AC5 | F2:0F1081 90050000            | movsd xmm0,qword ptr ds:[rcx+590]    
;000000014ACF1ACD | F241:0F1100                   | movsd qword ptr ds:[r8],xmm0         
;000000014ACF1AD2 | 8B81 98050000                 | mov eax,dword ptr ds:[rcx+598]       
;000000014ACF1AD8 | 41:8940 08                    | mov dword ptr ds:[r8+8],eax          
;000000014ACF1ADC | C3                            | ret										<< CONTINUE HERE
	mov [g_cameraStructAddress], rcx
originalCode:
	movsd xmm0,qword ptr [rcx+584h]
	movsd qword ptr [rdx],xmm0    
	mov eax,dword ptr [rcx+58Ch]   
	mov dword ptr [rdx+8h],eax     
	movsd xmm0,qword ptr [rcx+590h]
	movsd qword ptr [r8],xmm0     
	mov eax,dword ptr [rcx+598h]   
	mov dword ptr [r8+8h],eax      
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP


cameraWrite1Interceptor PROC
;000000014AD1C585 | F2:0F114C24 40                | movsd qword ptr ss:[rsp+40],xmm1       
;000000014AD1C58B | 8B88 94000000                 | mov ecx,dword ptr [rax+94]          
;000000014AD1C591 | 8B87 C8050000                 | mov eax,dword ptr [rdi+5C8]         
;000000014AD1C597 | 0F1187 84050000               | movups xmmword ptr [rdi+584],xmm0     << WRITE coords, pitch	<< INTERCEPT HERE
;000000014AD1C59E | 898F 80050000                 | mov dword ptr [rdi+580],ecx         
;000000014AD1C5A4 | 48:89F9                       | mov rcx,rdi                            
;000000014AD1C5A7 | F2:0F118F 94050000            | movsd qword ptr [rdi+594],xmm1        << WRITE yaw,roll
;000000014AD1C5AF | 8987 9C050000                 | mov dword ptr [rdi+59C],eax           << WRITE Fov
;000000014AD1C5B5 | E8 864A1800                   | call injustice2_dump.14AEA1040        << CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movups xmmword ptr [rdi+584h],xmm0 
	mov dword ptr [rdi+580h],ecx       
	movsd qword ptr [rdi+594h],xmm1    
	mov dword ptr [rdi+59Ch],eax
exit:
	mov rcx, rdi
	jmp qword ptr [_cameraWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite1Interceptor ENDP


timeDilationInterceptor PROC
;000000014ABDAAFC | F3:0F11B1 88000000 | movss dword ptr [rcx+88],xmm6      	<< write of time dilation		<< INTERCEPT HERE
;000000014ABDAB04 | 0F287424 30        | movaps xmm6,xmmword ptr ss:[rsp+30]   
;000000014ABDAB09 | 48:83C4 40         | add rsp,40                            
;000000014ABDAB0D | 5B                 | pop rbx								<< CONTINUE HERE                   
;000000014ABDAB0E | C3                 | ret                                   
	mov [g_timeDilationAddress], rcx
originalCode:
	movss dword ptr [rcx+88h],xmm6      
	movaps xmm6,xmmword ptr [rsp+30h]
	add rsp,40h
exit:
	jmp qword ptr [_timeDilationInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
timeDilationInterceptor ENDP


dofEnableInterceptor PROC
;000000014AB68D5B | 8843 68       | mov byte ptr [rbx+68],al     	<< If 0, dof is OFF, 2 is ON		<< INTERCEPT HERE
;000000014AB68D5E | 41:8B46 6C    | mov eax,dword ptr [r14+6C]    
;000000014AB68D62 | 8943 6C       | mov dword ptr [rbx+6C],eax    
;000000014AB68D65 | 41:8B46 70    | mov eax,dword ptr [r14+70]    
;000000014AB68D69 | 8943 70       | mov dword ptr [rbx+70],eax		<< CONTINUE HERE
;000000014AB68D6C | 41:8B46 74    | mov eax,dword ptr [r14+74]    
;000000014AB68D70 | 8943 74       | mov dword ptr [rbx+74],eax    
;000000014AB68D73 | 41:8B46 78    | mov eax,dword ptr [r14+78]    
	mov [g_dofStructAddress], rbx
originalCode:
	mov byte ptr [rbx+68h],al   
	mov eax,dword ptr [r14+6Ch] 
	mov dword ptr [rbx+6Ch],eax 
	mov eax,dword ptr [r14+70h] 
exit:
	jmp qword ptr [_dofEnableInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
dofEnableInterceptor ENDP

END