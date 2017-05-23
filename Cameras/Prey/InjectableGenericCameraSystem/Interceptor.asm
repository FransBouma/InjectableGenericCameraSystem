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
PUBLIC fovReadInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_cameraStructAddress: qword
EXTERN g_fovConstructAddress: qword
EXTERN g_timestopStructAddress: qword
;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWrite1InterceptionContinue: qword
EXTERN _cameraWrite2InterceptionContinue: qword
EXTERN _cameraWrite3InterceptionContinue: qword
EXTERN _fovReadInterceptionContinue: qword
EXTERN _timestopInterceptionContinue: qword
.data

;---------------------------------------------------------------
; Scratch pad
; player intercept. Is matrix (3x4, where the the coords are the lower row, write like this:
;matrixInMemory[0] = rotationMatrix._11;
;matrixInMemory[1] = rotationMatrix._21;
;matrixInMemory[2] = rotationMatrix._31;
;matrixInMemory[3] = newCoords.x;
;matrixInMemory[4] = rotationMatrix._12;
;matrixInMemory[5] = rotationMatrix._22;
;matrixInMemory[6] = rotationMatrix._32;
;matrixInMemory[7] = newCoords.y;
;matrixInMemory[8] = rotationMatrix._13;
;matrixInMemory[9] = rotationMatrix._23;
;matrixInMemory[10] = rotationMatrix._33;
;matrixInMemory[11] = newCoords.z;
;
;Prey.AK::Monitor::PostCode+24A6D9 - F3 45 0F11 7C 24 0C   - movss [r12+0C],xmm15			// write x			<< INTERCEPT HERE
;Prey.AK::Monitor::PostCode+24A6E0 - F3 45 0F11 74 24 1C   - movss [r12+1C],xmm14			// write y
;Prey.AK::Monitor::PostCode+24A6E7 - F3 0F59 C1            - mulss xmm0,xmm1
;Prey.AK::Monitor::PostCode+24A6EB - F3 0F59 E9            - mulss xmm5,xmm1
;Prey.AK::Monitor::PostCode+24A6EF - F3 44 0F59 E3         - mulss xmm12,xmm3
;Prey.AK::Monitor::PostCode+24A6F4 - 41 0F28 CB            - movaps xmm1,xmm11
;Prey.AK::Monitor::PostCode+24A6F8 - 41 0F28 F2            - movaps xmm6,xmm10
;Prey.AK::Monitor::PostCode+24A6FC - F3 0F5C F8            - subss xmm7,xmm0
;Prey.AK::Monitor::PostCode+24A700 - F3 45 0F11 44 24 2C   - movss [r12+2C],xmm8			// write z
;Prey.AK::Monitor::PostCode+24A707 - F3 0F59 F2            - mulss xmm6,xmm2									<< CONTINUE HERE

; Supersampling read
; patch v1.0.3
;0000000140FC3FF0 | 41 F7 F8                 | idiv r8d                        
;0000000140FC3FF3 | 44 8B C0                 | mov r8d,eax                     
;0000000140FC3FF6 | 8B C1                    | mov eax,ecx                     
;0000000140FC3FF8 | 8B 0D 7A E7 4E 01        | mov ecx,dword ptr ds:[1424B2778]				<< Supersampling read
;0000000140FC3FFE | 99                       | cdq                             
;0000000140FC3FFF | F7 FF                    | idiv edi                        
;0000000140FC4001 | 44 3B C0                 | cmp r8d,eax                     
;0000000140FC4004 | 41 0F 4C C0              | cmovl eax,r8d                   
;0000000140FC4008 | 83 F9 01                 | cmp ecx,1                       
;0000000140FC400B | 7D 07                    | jge prey_dump.140FC4014         
;0000000140FC400D | B8 01 00 00 00           | mov eax,1                       
;0000000140FC4012 | EB 05                    | jmp prey_dump.140FC4019         
;0000000140FC4014 | 3B C8                    | cmp ecx,eax                     
;0000000140FC4016 | 0F 4C C1                 | cmovl eax,ecx                   
;0000000140FC4019 | 39 83 F4 F3 00 00        | cmp dword ptr ds:[rbx+F3F4],eax 
;0000000140FC401F | 74 13                    | je prey_dump.140FC4034          
;0000000140FC4021 | 89 83 F4 F3 00 00        | mov dword ptr ds:[rbx+F3F4],eax 
;0000000140FC4027 | B0 01                    | mov al,1                        
;0000000140FC4029 | 48 8B 5C 24 30           | mov rbx,qword ptr ss:[rsp+30]   
;0000000140FC402E | 48 83 C4 20              | add rsp,20                      
;0000000140FC4032 | 5F                       | pop rdi                         
;0000000140FC4033 | C3                       | ret                             
;0000000140FC4034 | 32 C0                    | xor al,al                       
;0000000140FC4036 | 48 8B 5C 24 30           | mov rbx,qword ptr ss:[rsp+30]   
;0000000140FC403B | 48 83 C4 20              | add rsp,20                      
;0000000140FC403F | 5F                       | pop rdi                         
;0000000140FC4040 | C3                       | ret                             
;
; sys_flash cvar read (for hud toggle)
; v1.0.3
;0000000146F06D30 | 48 89 74 24 10           | mov qword ptr ss:[rsp+10],rsi 
;0000000146F06D35 | 57                       | push rdi                      
;0000000146F06D36 | 48 83 EC 20              | sub rsp,20                    
;0000000146F06D3A | 83 3D 2B 46 48 FB 00     | cmp dword ptr ds:[14238B36C],0				<< sys_flash read
;0000000146F06D41 | 0F B6 F2                 | movzx esi,dl                  
;0000000146F06D44 | 48 89 CF                 | mov rdi,rcx                   
;0000000146F06D47 | 74 2E                    | je prey_dump.146F06D77        
;0000000146F06D49 | 48 8B 81 C8 00 00 00     | mov rax,qword ptr ds:[rcx+C8] 
;0000000146F06D50 | 48 89 5C 24 30           | mov qword ptr ss:[rsp+30],rbx 
;0000000146F06D55 | 48 8B 58 38              | mov rbx,qword ptr ds:[rax+38] 
;0000000146F06D59 | 48 8B 01                 | mov rax,qword ptr ds:[rcx]    
;0000000146F06D5C | FF 10                    | call qword ptr ds:[rax]       
;0000000146F06D5E | 48 8B 03                 | mov rax,qword ptr ds:[rbx]    
;0000000146F06D61 | 48 8D 57 08              | lea rdx,qword ptr ds:[rdi+8]  
;0000000146F06D65 | 44 0F B6 C6              | movzx r8d,sil                 
;0000000146F06D69 | 48 89 D9                 | mov rcx,rbx                   
;0000000146F06D6C | FF 90 68 03 00 00        | call qword ptr ds:[rax+368]   
;0000000146F06D72 | 48 8B 5C 24 30           | mov rbx,qword ptr ss:[rsp+30] 
;0000000146F06D77 | 48 8B 74 24 38           | mov rsi,qword ptr ss:[rsp+38] 
;0000000146F06D7C | 48 83 C4 20              | add rsp,20                    
;0000000146F06D80 | 5F                       | pop rdi                       
;0000000146F06D81 | C3                       | ret                           
;
.code


cameraStructInterceptor PROC
; Struct interceptor is also a write interceptor for coords/quaternion
; patch 1.0.2
;Prey.exe+150E5DF - F3 0F11 4E 0C         - movss [rsi+0C],xmm1		// quaternion						<< intercept here
;Prey.exe+150E5E4 - F3 0F11 56 10         - movss [rsi+10],xmm2
;Prey.exe+150E5E9 - F3 0F11 5E 14         - movss [rsi+14],xmm3
;Prey.exe+150E5EE - F3 0F11 46 18         - movss [rsi+18],xmm0
;Prey.exe+150E5F3 - F3 0F11 3E            - movss [rsi],xmm7   		// coords
;Prey.exe+150E5F7 - F3 0F11 76 04         - movss [rsi+04],xmm6
;Prey.exe+150E5FC - F3 44 0F11 46 08      - movss [rsi+08],xmm8
;Prey.exe+150E602 - E8 8946D5FF           - call Prey.exe+1262C90					<< continue here
; This function intercepts the camera address and also blocks the writes by reading from our own two structs. 
	mov [g_cameraStructAddress], rsi
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	movss dword ptr [rsi+0Ch],xmm1				; quaternion	 (x/y/z/w)
	movss dword ptr [rsi+10h],xmm2
	movss dword ptr [rsi+14h],xmm3
	movss dword ptr [rsi+18h],xmm0
	movss dword ptr [rsi],xmm7					; coords
	movss dword ptr [rsi+04h],xmm6
	movss dword ptr [rsi+08h],xmm8
exit:
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP


cameraWrite1Interceptor PROC
; patch 1.0.2
;Prey.exe+386A10 - 8B 02                 - mov eax,[rdx]						<< INTERCEPT HERE
;Prey.exe+386A12 - 89 01                 - mov [rcx],eax						// coords
;Prey.exe+386A14 - 8B 42 04              - mov eax,[rdx+04]
;Prey.exe+386A17 - 89 41 04              - mov [rcx+04],eax
;Prey.exe+386A1A - 8B 42 08              - mov eax,[rdx+08]
;Prey.exe+386A1D - 89 41 08              - mov [rcx+08],eax
;Prey.exe+386A20 - 8B 42 0C              - mov eax,[rdx+0C]
;Prey.exe+386A23 - 89 41 0C              - mov [rcx+0C],eax						// qX
;Prey.exe+386A26 - 8B 42 10              - mov eax,[rdx+10]
;Prey.exe+386A29 - 89 41 10              - mov [rcx+10],eax
;Prey.exe+386A2C - 8B 42 14              - mov eax,[rdx+14]
;Prey.exe+386A2F - 89 41 14              - mov [rcx+14],eax
;Prey.exe+386A32 - 8B 42 18              - mov eax,[rdx+18]
;Prey.exe+386A35 - 89 41 18              - mov [rcx+18],eax						// qW
;Prey.exe+386A38 - 8B 42 1C              - mov eax,[rdx+1C]
;Prey.exe+386A3B - 89 41 1C              - mov [rcx+1C],eax						<< CONTINUE HERE
;Prey.exe+386A3E - 8B 42 20              - mov eax,[rdx+20]
;Prey.exe+386A41 - 89 41 20              - mov [rcx+20],eax
;Prey.exe+386A44 - 8B 42 24              - mov eax,[rdx+24]
;Prey.exe+386A47 - 89 41 24              - mov [rcx+24],eax
;Prey.exe+386A4A - 8B 42 28              - mov eax,[rdx+28]
;Prey.exe+386A4D - 89 41 28              - mov [rcx+28],eax
;Prey.exe+386A50 - 8B 42 2C              - mov eax,[rdx+2C]
;Prey.exe+386A53 - 89 41 2C              - mov [rcx+2C],eax
;Prey.exe+386A56 - 8B 42 30              - mov eax,[rdx+30]
;Prey.exe+386A59 - 89 41 30              - mov [rcx+30],eax
;Prey.exe+386A5C - 8B 42 34              - mov eax,[rdx+34]
;Prey.exe+386A5F - 89 41 34              - mov [rcx+34],eax
;Prey.exe+386A62 - 0FB6 42 38            - movzx eax,byte ptr [rdx+38]
;Prey.exe+386A66 - 88 41 38              - mov [rcx+38],al
;Prey.exe+386A69 - 0FB6 42 39            - movzx eax,byte ptr [rdx+39]
;Prey.exe+386A6D - 88 41 39              - mov [rcx+39],al
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
originalCode:
	mov eax,[rdx]
	mov [rcx],eax	
	mov eax,[rdx+04h]
	mov [rcx+04h],eax
	mov eax,[rdx+08h]
	mov [rcx+08h],eax
	mov eax,[rdx+0Ch]
	mov [rcx+0Ch],eax
	mov eax,[rdx+10h]
	mov [rcx+10h],eax
	mov eax,[rdx+14h]
	mov [rcx+14h],eax
	mov eax,[rdx+18h]
	mov [rcx+18h],eax
exit:
	jmp qword ptr [_cameraWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite1Interceptor ENDP


cameraWrite2Interceptor PROC
; coord write intercept is broken up in two blocks as it has a relative read in the middle.... WHY is that statement placed at that spot!
;Prey.AK::Monitor::PostCode+24A38A - F3 44 0F58 68 04      - addss xmm13,[rax+04]			<< INTERCEPT HERE
;Prey.AK::Monitor::PostCode+24A390 - F3 44 0F58 20         - addss xmm12,[rax]
;Prey.AK::Monitor::PostCode+24A395 - F3 44 0F58 70 08      - addss xmm14,[rax+08]
;Prey.AK::Monitor::PostCode+24A39B - F3 44 0F11 67 1C      - movss [rdi+1C],xmm12			// write x
;Prey.AK::Monitor::PostCode+24A3A1 - F3 44 0F11 6F 20      - movss [rdi+20],xmm13			// write y
;Prey.AK::Monitor::PostCode+24A3A7 - F3 44 0F10 2D 08DAC401  - movss xmm13,[Prey.CreateAudioInputSource+51B6B8]		<< CONTINUE HERE
	addss xmm13, dword ptr [rax+04h]					; original statement
	addss xmm12, dword ptr [rax]
	addss xmm14, dword ptr [rax+08h]
	cmp byte ptr [g_cameraEnabled], 1					; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je exit												; our own camera is enabled, just skip the writes
	movss dword ptr [rdi+1Ch],xmm12
	movss dword ptr [rdi+20h],xmm13
exit:
	jmp qword ptr [_cameraWrite2InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite2Interceptor ENDP

cameraWrite3Interceptor PROC
;Prey.AK::Monitor::PostCode+24A3B0 - F3 44 0F11 77 24      - movss [rdi+24],xmm14			<< INTERCEPT HERE, write Z
;Prey.AK::Monitor::PostCode+24A3B6 - F3 0F10 8D 88000000   - movss xmm1,[rbp+00000088]
;Prey.AK::Monitor::PostCode+24A3BE - 48 8D 4F 1C           - lea rcx,[rdi+1C]
;Prey.AK::Monitor::PostCode+24A3C2 - E8 79410000           - call Prey.AK::Monitor::PostCode+24E540			<< CONTINUE HERE
	cmp byte ptr [g_cameraEnabled], 1						; check if the user enabled the camera. If so, just skip the write statements, otherwise just execute the original code.
	je noWrite												; our own camera is enabled, just skip the writes
	movss dword ptr [rdi+24h],xmm14		
noWrite:
	movss xmm1, dword ptr [rbp+00000088h]
	lea rcx,[rdi+1Ch]
exit:
	jmp qword ptr [_cameraWrite3InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite3Interceptor ENDP


timestopInterceptor PROC
; start of function which contains timestop read. Contains CryAction instance (with m_pause) in rcx
;Prey.exe+5D6AD0 - 48 89 5C 24 10        - mov [rsp+10],rbx					<<< INTERCEPT HERE
;Prey.exe+5D6AD5 - 48 89 6C 24 18        - mov [rsp+18],rbp
;Prey.exe+5D6ADA - 48 89 74 24 20        - mov [rsp+20],rsi
;Prey.exe+5D6ADF - 57                    - push rdi							<<< CONTINUE HERE
;Prey.exe+5D6AE0 - 41 54                 - push r12
;Prey.exe+5D6AE2 - 41 55                 - push r13
;Prey.exe+5D6AE4 - 41 56                 - push r14
;Prey.exe+5D6AE6 - 41 57                 - push r15
;...
;Prey.exe+5D6C99 - 74 09                 - je Prey.exe+5D6CA4
;Prey.exe+5D6C9B - 44 38 6B 08           - cmp [rbx+08],r13l				<<< TImestop read
;Prey.exe+5D6C9F - 75 03                 - jne Prey.exe+5D6CA4
;Prey.exe+5D6CA1 - 45 8B FD              - mov r15d,r13d
;
	mov [g_timestopStructAddress], rcx
originalCode:
	mov [rsp+10h],rbx
	mov [rsp+18h],rbp
	mov [rsp+20h],rsi
exit:
	jmp qword ptr [_timestopInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
timestopInterceptor ENDP


fovReadInterceptor PROC
;Prey.AK::Monitor::PostCode+11EB4CA - 48 8B 05 4F510E01     - mov rax,[Prey.AmdPowerXpressRequestHighPerformance+860E0] 
;Prey.AK::Monitor::PostCode+11EB4D1 - F3 0F10 53 0C         - movss xmm2,[rbx+0C]										<<<< INTERCEP HERE
;Prey.AK::Monitor::PostCode+11EB4D6 - 0F28 D8               - movaps xmm3,xmm0
;Prey.AK::Monitor::PostCode+11EB4D9 - F3 0F10 48 08         - movss xmm1,[rax+08]							<< Read FOV. 
;Prey.AK::Monitor::PostCode+11EB4DE - 0F2E CA               - ucomiss xmm1,xmm2
;Prey.AK::Monitor::PostCode+11EB4E1 - 75 09                 - jne Prey.AK::Monitor::PostCode+11EB4EC					<<< CONTINUE HERE
;Prey.AK::Monitor::PostCode+11EB4E3 - 0F57 C0               - xorps xmm0,xmm0
;Prey.AK::Monitor::PostCode+11EB4E6 - 48 83 C4 20           - add rsp,20 { 32 }
;Prey.AK::Monitor::PostCode+11EB4EA - 5B                    - pop rbx
;Prey.AK::Monitor::PostCode+11EB4EB - C3                    - ret 
	mov [g_fovConstructAddress], rax
originalCode:
	movss xmm2, dword ptr [rbx+0Ch]
	movaps xmm3,xmm0
	movss xmm1, dword ptr [rax+08h]
	ucomiss xmm1,xmm2
exit:
	jmp qword ptr [_fovReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
fovReadInterceptor ENDP

END