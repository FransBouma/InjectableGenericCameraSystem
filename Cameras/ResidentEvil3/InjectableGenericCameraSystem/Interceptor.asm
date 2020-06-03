;////////////////////////////////////////////////////////////////////////////////////////////////////////
;// Part of Injectable Generic Camera System
;// Copyright(c) 2020, Frans Bouma
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
PUBLIC cameraWrite5Interceptor
PUBLIC cameraWrite6Interceptor
PUBLIC timestopReadInterceptor
PUBLIC resolutionScaleReadInterceptor
PUBLIC displayTypeInterceptor
PUBLIC dofSelectorWriteInterceptor
;---------------------------------------------------------------

;---------------------------------------------------------------
; Externs which are used and set by the system. Read / write these
; values in asm to communicate with the system
EXTERN g_cameraEnabled: byte
EXTERN g_cameraStructAddress: qword
EXTERN g_resolutionScaleAddress: qword
EXTERN g_timestopStructAddress: qword
EXTERN g_displayTypeStructAddress: qword
EXTERN g_dofStructAddress: qword

;---------------------------------------------------------------

;---------------------------------------------------------------
; Own externs, defined in InterceptorHelper.cpp
EXTERN _cameraStructInterceptionContinue: qword
EXTERN _cameraWrite1InterceptionContinue: qword
EXTERN _cameraWrite2InterceptionContinue: qword
EXTERN _cameraWrite3InterceptionContinue: qword
EXTERN _cameraWrite4InterceptionContinue: qword
EXTERN _cameraWrite5InterceptionContinue: qword
EXTERN _cameraWrite6InterceptionContinue: qword
EXTERN _timestopReadInterceptionContinue: qword
EXTERN _resolutionScaleReadInterceptionContinue: qword
EXTERN _displayTypeInterceptionContinue: qword
EXTERN _dofSelectorWriteInterceptionContinue: qword

.data

.code

cameraStructInterceptor PROC
; intercepts camera and also blocks FOV write 1
;re3demo.exe+160A72B9 - 44 0F29 8C 24 10010000  - movaps [rsp+00000110],xmm9
;re3demo.exe+160A72C2 - F3 44 0F10 48 34      - movss xmm9,[rax+34]
;re3demo.exe+160A72C8 - 48 85 C0              - test rax,rax
;re3demo.exe+160A72CB - 75 13                 - jne re3demo.exe+160A72E0
;re3demo.exe+160A72CD - 45 31 C0              - xor r8d,r8d
;re3demo.exe+160A72D0 - 8D 50 38              - lea edx,[rax+38]
;re3demo.exe+160A72D3 - 48 89 D9              - mov rcx,rbx
;re3demo.exe+160A72D6 - E8 F5E21EEC           - call re3demo.exe+22955D0
;re3demo.exe+160A72DB - E9 B50C0000           - jmp re3demo.exe+160A7F95
;re3demo.exe+160A72E0 - 8B 40 38              - mov eax,[rax+38]				<< INTERCEPT HERE
;re3demo.exe+160A72E3 - 89 82 B4000000        - mov [rdx+000000B4],eax				<< Write FOV
;re3demo.exe+160A72E9 - 48 83 79 18 00        - cmp qword ptr [rcx+18],00
;re3demo.exe+160A72EE - 0F85 A10C0000         - jne re3demo.exe+160A7F95		<< CONTINUE HERE
;re3demo.exe+160A72F4 - 48 8B 8A C8000000     - mov rcx,[rdx+000000C8]
;re3demo.exe+160A72FB - 44 0F29 84 24 20010000  - movaps [rsp+00000120],xmm8
;re3demo.exe+160A7304 - 48 85 C9              - test rcx,rcx
;re3demo.exe+160A7307 - 74 26                 - je re3demo.exe+160A732F
;re3demo.exe+160A7309 - F3 0F10 8D 90000000   - movss xmm1,[rbp+00000090]
;re3demo.exe+160A7311 - 48 83 C1 10           - add rcx,10 { 16 }
;re3demo.exe+160A7315 - E8 166920EC           - call re3demo.exe+22ADC30
;re3demo.exe+160A731A - 48 8B 43 50           - mov rax,[rbx+50]
	mov [g_cameraStructAddress], rdx
	mov eax,[rax+38h]
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	mov [rdx+000000B4h],eax
exit:
	cmp qword ptr [rcx+18h],00h
	jmp qword ptr [_cameraStructInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraStructInterceptor ENDP

cameraWrite1Interceptor PROC
; fov write 2
;re3demo.exe+160A76AE - 48 83 78 18 00        - cmp qword ptr [rax+18],00
;re3demo.exe+160A76B3 - 0F85 CB080000         - jne re3demo.exe+160A7F84
;re3demo.exe+160A76B9 - 48 8B 86 B8000000     - mov rax,[rsi+000000B8]
;re3demo.exe+160A76C0 - 48 85 C0              - test rax,rax
;re3demo.exe+160A76C3 - 0F84 DE020000         - je re3demo.exe+160A79A7
;re3demo.exe+160A76C9 - F3 0F10 78 30         - movss xmm7,[rax+30]
;re3demo.exe+160A76CE - F3 44 0F10 48 34      - movss xmm9,[rax+34]				<< INTERCEPT HERE
;re3demo.exe+160A76D4 - 8B 40 38              - mov eax,[rax+38]   				
;re3demo.exe+160A76D7 - 89 87 B4000000        - mov [rdi+000000B4],eax			<< WRITE FOV
;re3demo.exe+160A76DD - 48 8B 43 50           - mov rax,[rbx+50]   				<< CONTINUE HERE
;re3demo.exe+160A76E1 - 48 83 78 18 00        - cmp qword ptr [rax+18],00
;re3demo.exe+160A76E6 - 0F85 98080000         - jne re3demo.exe+160A7F84
;re3demo.exe+160A76EC - F3 0F10 86 A0000000   - movss xmm0,[rsi+000000A0]
;re3demo.exe+160A76F4 - F3 0F10 8E A4000000   - movss xmm1,[rsi+000000A4]
;re3demo.exe+160A76FC - F3 0F10 96 A8000000   - movss xmm2,[rsi+000000A8]
;re3demo.exe+160A7704 - F3 0F11 87 90000000   - movss [rdi+00000090],xmm0
;re3demo.exe+160A770C - F3 0F11 8F 94000000   - movss [rdi+00000094],xmm1
; Update June 2020
; 000000014065A24C | F3:0F1070 30         | movss xmm6,dword ptr [rax+30]  
; 000000014065A251 | F3 0F 10 78 34       | movss xmm7,dword ptr [rax+34]  	<< INTERCEPT HERE
; 000000014065A256 | 8B 40 38             | mov eax,dword ptr [rax+38]     	
; 000000014065A259 | 89 86 B4000000       | mov dword ptr [rsi+B4],eax     	<< WRITE FOV
; 000000014065A25F | 48:8B47 50           | mov rax,qword ptr [rdi+50]     	<< CONTINUE HERE
; 000000014065A263 | 48:8378 18 00        | cmp qword ptr [rax+18],0       
; 000000014065A268 | 0F85 6A080000        | jne re3_dump.14065AAD8            
; 000000014065A26E | F3:0F1083 A0000000   | movss xmm0,dword ptr [rbx+A0]  
; 000000014065A276 | F3:0F108B A4000000   | movss xmm1,dword ptr [rbx+A4]  
	movss xmm7, dword ptr [rax+34h]		
	mov eax,[rax+38h]   		
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	mov [rsi+000000B4h],eax	
exit:
	jmp qword ptr [_cameraWrite1InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite1Interceptor ENDP

cameraWrite2Interceptor PROC
; fov write 3
;re3demo.exe+160A7A42 - F2 0F59 D1            - mulsd xmm2,xmm1
;re3demo.exe+160A7A46 - 66 0F5A C2            - cvtpd2ps xmm0,xmm2
;re3demo.exe+160A7A4A - E8 75E479EE           - call re3demo.exe+4845EC4
;re3demo.exe+160A7A4F - F3 0F59 05 5D1C82EE   - mulss xmm0,[re3demo.exe+48C96B4]
;re3demo.exe+160A7A57 - F3 0F5A C0            - cvtss2sd xmm0,xmm0
;re3demo.exe+160A7A5B - F2 0F59 05 85440DEF   - mulsd xmm0,[re3demo.exe+517BEE8]
;re3demo.exe+160A7A63 - F2 0F5A C0            - cvtsd2ss xmm0,xmm0						<< INTERCEPT HERE
;re3demo.exe+160A7A67 - F3 0F11 87 B4000000   - movss [rdi+000000B4],xmm0				<< WRITE FOV
;re3demo.exe+160A7A6F - 48 8B 43 50           - mov rax,[rbx+50]
;re3demo.exe+160A7A73 - 48 8B 48 18           - mov rcx,[rax+18]						<< CONTINUE HERE
;re3demo.exe+160A7A77 - 48 85 C9              - test rcx,rcx
;re3demo.exe+160A7A7A - 0F85 CE040000         - jne re3demo.exe+160A7F4E
;re3demo.exe+160A7A80 - 48 85 C9              - test rcx,rcx
;re3demo.exe+160A7A83 - 0F85 C5040000         - jne re3demo.exe+160A7F4E
;// Update June 2020:
;000000014065A5D3 | E8 5CE01204          | call re3_dump.144788634              
;000000014065A5D8 | F3 0F5905 14F11A04   | mulss xmm0,dword ptr [1448096F4]  
;000000014065A5E0 | F3 0F5AC0            | cvtss2sd xmm0,xmm0                   
;000000014065A5E4 | F2 0F5905 142BA604   | mulsd xmm0,qword ptr [1450BD100]  
;000000014065A5EC | F2 0F 5A C0          | cvtsd2ss xmm0,xmm0                   	<< INTERCEPT HERE
;000000014065A5F0 | F3 0F1186 B4000000   | movss dword ptr [rsi+B4],xmm0     		<< WRITE FOV
;000000014065A5F8 | 48 8B47 50           | mov rax,qword ptr [rdi+50]        
;000000014065A5FC | 48 8B48 18           | mov rcx,qword ptr [rax+18]        		<< CONTINUE HERE
;000000014065A600 | 48 85C9              | test rcx,rcx                         
;000000014065A603 | 0F85 90040000        | jne re3_dump.14065AA99               
;000000014065A609 | 48:85C9              | test rcx,rcx                         
	cvtsd2ss xmm0,xmm0		
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rsi+000000B4h],xmm0
exit:
	mov rax,[rdi+50h]
	jmp qword ptr [_cameraWrite2InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite2Interceptor ENDP

cameraWrite3Interceptor PROC
; coords write
;re3demo.exe+160A780D - 48 83 78 18 00        - cmp qword ptr [rax+18],00 { 0 }
;re3demo.exe+160A7812 - 0F85 48070000         - jne re3demo.exe+160A7F60
;re3demo.exe+160A7818 - F3 0F10 44 24 50      - movss xmm0,[rsp+50]
;re3demo.exe+160A781E - F3 0F10 4C 24 54      - movss xmm1,[rsp+54]
;re3demo.exe+160A7824 - F3 0F10 54 24 58      - movss xmm2,[rsp+58]
;re3demo.exe+160A782A - F3 0F11 87 80000000   - movss [rdi+00000080],xmm0			<< INTERCEPT HERE <<< Write x
;re3demo.exe+160A7832 - F3 0F11 8F 84000000   - movss [rdi+00000084],xmm1			<<< Write y
;re3demo.exe+160A783A - F3 0F11 97 88000000   - movss [rdi+00000088],xmm2			<<< Write z
;re3demo.exe+160A7842 - 48 8B 43 50           - mov rax,[rbx+50]					<< CONTINUE HERE
;re3demo.exe+160A7846 - 48 83 78 18 00        - cmp qword ptr [rax+18],00 { 0 }
;re3demo.exe+160A784B - 0F85 0F070000         - jne re3demo.exe+160A7F60
;re3demo.exe+160A7851 - F3 0F10 74 24 40      - movss xmm6,[rsp+40]
;re3demo.exe+160A7857 - F3 44 0F10 44 24 44   - movss xmm8,[rsp+44]
;re3demo.exe+160A785E - 44 0F29 94 24 00010000  - movaps [rsp+00000100],xmm10
;re3demo.exe+160A7867 - F3 44 0F10 54 24 48   - movss xmm10,[rsp+48]
;re3demo.exe+160A786E - F3 0F11 B7 A0000000   - movss [rdi+000000A0],xmm6
;// Update June 2020
;000000014065A39A | F3:0F104424 50      | movss xmm0,dword ptr [rsp+50]  
;000000014065A3A0 | F3:0F104C24 54      | movss xmm1,dword ptr [rsp+54]  
;000000014065A3A6 | F3:0F105424 58      | movss xmm2,dword ptr [rsp+58]  
;000000014065A3AC | F3:0F1186 80000000  | movss dword ptr [rsi+80],xmm0  	<< INTERCEPT HERE <<< Write x
;000000014065A3B4 | F3:0F118E 84000000  | movss dword ptr [rsi+84],xmm1  	<<< Write y
;000000014065A3BC | F3:0F1196 88000000  | movss dword ptr [rsi+88],xmm2  	<<< Write z
;000000014065A3C4 | 48:8B47 50          | mov rax,qword ptr [rdi+50]     	<< CONTINUE HERE
;000000014065A3C8 | 48:8378 18 00       | cmp qword ptr [rax+18],0       
;000000014065A3CD | 0F85 E1060000       | jne re3_dump.14065AAB4            
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rsi+00000080h],xmm0
	movss dword ptr [rsi+00000084h],xmm1
	movss dword ptr [rsi+00000088h],xmm2
exit:
	jmp qword ptr [_cameraWrite3InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite3Interceptor ENDP

cameraWrite4Interceptor PROC
; coords write 2 
;re3demo.exe+160A7646 - 0F10 46 70            - movups xmm0,[rsi+70]
;re3demo.exe+160A764A - 0F11 47 70            - movups [rdi+70],xmm0
;re3demo.exe+160A764E - 48 8B 43 50           - mov rax,[rbx+50]
;re3demo.exe+160A7652 - 48 83 78 18 00        - cmp qword ptr [rax+18],00 { 0 }
;re3demo.exe+160A7657 - 0F85 27090000         - jne re3demo.exe+160A7F84
;re3demo.exe+160A765D - F3 0F10 86 80000000   - movss xmm0,[rsi+00000080]
;re3demo.exe+160A7665 - F3 0F10 8E 84000000   - movss xmm1,[rsi+00000084]
;re3demo.exe+160A766D - F3 0F10 96 88000000   - movss xmm2,[rsi+00000088]
;re3demo.exe+160A7675 - F3 0F11 87 80000000   - movss [rdi+00000080],xmm0			<< INTERCEPT HERE	<< X
;re3demo.exe+160A767D - F3 0F11 8F 84000000   - movss [rdi+00000084],xmm1				<< Y
;re3demo.exe+160A7685 - F3 0F11 97 88000000   - movss [rdi+00000088],xmm2				<< Z
;re3demo.exe+160A768D - 48 8B 43 50           - mov rax,[rbx+50]					<< CONTINUE HERE
;re3demo.exe+160A7691 - 48 83 78 18 00        - cmp qword ptr [rax+18],00 { 0 }
;re3demo.exe+160A7696 - 0F85 E8080000         - jne re3demo.exe+160A7F84
;re3demo.exe+160A769C - 0F10 86 90000000      - movups xmm0,[rsi+00000090]
;// Update June 2020
;re3.exe+65A1D1 - 48 8B 47 50           - mov rax,[rdi+50]
;re3.exe+65A1D5 - 48 83 78 18 00        - cmp qword ptr [rax+18],00 { 0 }
;re3.exe+65A1DA - 0F85 F8080000         - jne re3.exe+65AAD8
;re3.exe+65A1E0 - F3 0F10 83 80000000   - movss xmm0,[rbx+00000080]
;re3.exe+65A1E8 - F3 0F10 8B 84000000   - movss xmm1,[rbx+00000084]
;re3.exe+65A1F0 - F3 0F10 93 88000000   - movss xmm2,[rbx+00000088]
;re3.exe+65A1F8 - F3 0F11 86 80000000   - movss [rsi+00000080],xmm0			<< INTERCEPT HERE	<< X
;re3.exe+65A200 - F3 0F11 8E 84000000   - movss [rsi+00000084],xmm1				<< Y
;re3.exe+65A208 - F3 0F11 96 88000000   - movss [rsi+00000088],xmm2				<< Z
;re3.exe+65A210 - 48 8B 47 50           - mov rax,[rdi+50]					<< CONTINUE HERE
;re3.exe+65A214 - 48 83 78 18 00        - cmp qword ptr [rax+18],00 { 0 }
;re3.exe+65A219 - 0F85 B9080000         - jne re3.exe+65AAD8
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movss dword ptr [rsi+00000080h],xmm0
	movss dword ptr [rsi+00000084h],xmm1
	movss dword ptr [rsi+00000088h],xmm2
exit:
	jmp qword ptr [_cameraWrite4InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite4Interceptor ENDP

cameraWrite5Interceptor PROC
; quaternion write
;re3demo.exe+160A768D - 48 8B 43 50           - mov rax,[rbx+50]
;re3demo.exe+160A7691 - 48 83 78 18 00        - cmp qword ptr [rax+18],00 { 0 }
;re3demo.exe+160A7696 - 0F85 E8080000         - jne re3demo.exe+160A7F84
;re3demo.exe+160A769C - 0F10 86 90000000      - movups xmm0,[rsi+00000090]			<< INTERCEPT HERE
;re3demo.exe+160A76A3 - 0F11 87 A0000000      - movups [rdi+000000A0],xmm0			<< WRITE Quaternion
;re3demo.exe+160A76AA - 48 8B 43 50           - mov rax,[rbx+50]					<< CONTINUE HERE
;re3demo.exe+160A76AE - 48 83 78 18 00        - cmp qword ptr [rax+18],00 { 0 }
;re3demo.exe+160A76B3 - 0F85 CB080000         - jne re3demo.exe+160A7F84
;re3demo.exe+160A76B9 - 48 8B 86 B8000000     - mov rax,[rsi+000000B8]
;re3demo.exe+160A76C0 - 48 85 C0              - test rax,rax
;re3demo.exe+160A76C3 - 0F84 DE020000         - je re3demo.exe+160A79A7
;re3demo.exe+160A76C9 - F3 0F10 78 30         - movss xmm7,[rax+30]
;re3demo.exe+160A76CE - F3 44 0F10 48 34      - movss xmm9,[rax+34]
;re3demo.exe+160A76D4 - 8B 40 38              - mov eax,[rax+38]
;re3demo.exe+160A76D7 - 89 87 B4000000        - mov [rdi+000000B4],eax
;// Update June 2020
;000000014065A210 | 48:8B47 50        | mov rax,qword ptr [rdi+50]        
;000000014065A214 | 48:8378 18 00     | cmp qword ptr [rax+18],0          
;000000014065A219 | 0F85 B9080000     | jne re3_dump.14065AAD8               
;000000014065A21F | 0F1083 90000000   | movups xmm0,xmmword ptr [rbx+90]  	<< INTERCEPT HERE
;000000014065A226 | 0F1186 A0000000   | movups xmmword ptr [rsi+A0],xmm0  	<< WRITE Quaternion
;000000014065A22D | 48:8B47 50        | mov rax,qword ptr [rdi+50]        	<< CONTINUE HERE
;000000014065A231 | 48:8378 18 00     | cmp qword ptr [rax+18],0          
;000000014065A236 | 0F85 9C080000     | jne re3_dump.14065AAD8               
;000000014065A23C | 48:8B83 B8000000  | mov rax,qword ptr [rbx+B8]        
;000000014065A243 | 48:85C0           | test rax,rax                         
;000000014065A246 | 0F84 35040000     | je re3_dump.14065A681                
;000000014065A24C | F3:0F1070 30      | movss xmm6,dword ptr [rax+30]     
;000000014065A251 | F3:0F1078 34      | movss xmm7,dword ptr [rax+34]     
;000000014065A256 | 8B40 38           | mov eax,dword ptr [rax+38]        
	movups xmm0, xmmword ptr [rbx+00000090h]
	cmp byte ptr [g_cameraEnabled], 1
	je exit
originalCode:
	movups xmmword ptr [rsi+000000A0h],xmm0
exit:
	jmp qword ptr [_cameraWrite5InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite5Interceptor ENDP

cameraWrite6Interceptor PROC
; quaternion write 2. Strictly not necessary but stops a camera move when the camera is enabled
;re3demo.exe+160A7842 - 48 8B 43 50             - mov rax,[rbx+50]
;re3demo.exe+160A7846 - 48 83 78 18 00          - cmp qword ptr [rax+18],00 { 0 }
;re3demo.exe+160A784B - 0F85 0F070000           - jne re3demo.exe+160A7F60
;re3demo.exe+160A7851 - F3 0F10 74 24 40        - movss xmm6,[rsp+40]
;re3demo.exe+160A7857 - F3 44 0F10 44 24 44     - movss xmm8,[rsp+44]
;re3demo.exe+160A785E - 44 0F29 94 24 00010000  - movaps [rsp+00000100],xmm10
;re3demo.exe+160A7867 - F3 44 0F10 54 24 48     - movss xmm10,[rsp+48]
;re3demo.exe+160A786E - F3 0F11 B7 A0000000     - movss [rdi+000000A0],xmm6			<< INTERCEPT HERE << Write Qx
;re3demo.exe+160A7876 - F3 44 0F11 87 A4000000  - movss [rdi+000000A4],xmm8			<< Write Qy
;re3demo.exe+160A787F - F3 44 0F11 97 A8000000  - movss [rdi+000000A8],xmm10		<< Write Qz
;re3demo.exe+160A7888 - 44 0F29 9C 24 F0000000  - movaps [rsp+000000F0],xmm11
;re3demo.exe+160A7891 - F3 44 0F10 5C 24 4C     - movss xmm11,[rsp+4C]
;re3demo.exe+160A7898 - F3 44 0F11 9F AC000000  - movss [rdi+000000AC],xmm11		<< Write Qw
;re3demo.exe+160A78A1 - 48 8B 43 50             - mov rax,[rbx+50]					<< CONTINUE HERE
;re3demo.exe+160A78A5 - 48 83 78 18 00          - cmp qword ptr [rax+18],00 { 0 }
;re3demo.exe+160A78AA - 0F85 9E060000           - jne re3demo.exe+160A7F4E
;// Update June 2020
;000000014065A3C4 | 48:8B47 50              | mov rax,qword ptr [rdi+50]           
;000000014065A3C8 | 48:8378 18 00           | cmp qword ptr [rax+18],0             
;000000014065A3CD | 0F85 E1060000           | jne re3_dump.14065AAB4                  
;000000014065A3D3 | F344:0F104424 40        | movss xmm8,dword ptr [rsp+40]        
;000000014065A3DA | 44:0F298C24 10010000    | movaps xmmword ptr [rsp+110],xmm9    
;000000014065A3E3 | F344:0F104C24 44        | movss xmm9,dword ptr [rsp+44]        
;000000014065A3EA | 44:0F299424 00010000    | movaps xmmword ptr [rsp+100],xmm10   
;000000014065A3F3 | F344:0F105424 48        | movss xmm10,dword ptr [rsp+48]       
;000000014065A3FA | F344:0F1186 A0000000    | movss dword ptr [rsi+A0],xmm8        	<< INTERCEPT HERE << Write Qx
;000000014065A403 | F344:0F118E A4000000    | movss dword ptr [rsi+A4],xmm9        	<< Write Qy
;000000014065A40C | F344:0F1196 A8000000    | movss dword ptr [rsi+A8],xmm10       	<< Write Qz
;000000014065A415 | 44:0F299C24 F0000000    | movaps xmmword ptr [rsp+F0],xmm11    
;000000014065A41E | F344:0F105C24 4C        | movss xmm11,dword ptr [rsp+4C]       
;000000014065A425 | F344:0F119E AC000000    | movss dword ptr [rsi+AC],xmm11       	<< Write Qw
;000000014065A42E | 48:8B47 50              | mov rax,qword ptr [rdi+50]           	<< CONTINUE HERE
;000000014065A432 | 48:8378 18 00           | cmp qword ptr [rax+18],0             
;000000014065A437 | 0F85 5C060000           | jne re3_dump.14065AA99                  
;000000014065A43D | 4C:8D4D 80              | lea r9,qword ptr [rbp-80]            
	cmp byte ptr [g_cameraEnabled], 1
	jne originalCode
noWrites:
	movaps xmmword ptr [rsp+0F0h],xmm11
	movss xmm11,dword ptr [rsp+4Ch]   
	jmp exit
originalCode:
	movss dword ptr [rsi+0A0h],xmm8    
	movss dword ptr [rsi+0A4h],xmm9    
	movss dword ptr [rsi+0A8h],xmm10   
	movaps xmmword ptr [rsp+0F0h],xmm11
	movss xmm11,dword ptr [rsp+4Ch]   
	movss dword ptr [rsi+0ACh],xmm11   
exit:
	jmp qword ptr [_cameraWrite6InterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
cameraWrite6Interceptor ENDP

resolutionScaleReadInterceptor PROC
;re3demo.exe+1ABE49E3 - 48 8B 05 56CD11EE     - mov rax,[re3demo.exe+8D01740] { (178C53C0) }
;re3demo.exe+1ABE49EA - 48 8D 4C 24 30        - lea rcx,[rsp+30]
;re3demo.exe+1ABE49EF - F3 41 0F10 46 40      - movss xmm0,[r14+40]
;re3demo.exe+1ABE49F5 - C7 44 24 30 CDCCCC3D  - mov [rsp+30],3DCCCCCD { (0) }
;re3demo.exe+1ABE49FD - F3 0F59 80 B4140000   - mulss xmm0,[rax+000014B4]			<< INTERCEPT HERE << Address of resolution scaling 1.0 is 100% 2.0 is 200%
;re3demo.exe+1ABE4A05 - 48 8D 44 24 2C        - lea rax,[rsp+2C]
;re3demo.exe+1ABE4A0A - 0F2F C7               - comiss xmm0,xmm7
;re3demo.exe+1ABE4A0D - F3 0F11 44 24 2C      - movss [rsp+2C],xmm0					<< CONTINUE HERE
;re3demo.exe+1ABE4A13 - 48 0F46 C1            - cmovbe rax,rcx
;re3demo.exe+1ABE4A17 - F3 0F10 08            - movss xmm1,[rax]
;re3demo.exe+1ABE4A1B - 0F2F F1               - comiss xmm6,xmm1
;re3demo.exe+1ABE4A1E - 77 03                 - ja re3demo.exe+1ABE4A23
;re3demo.exe+1ABE4A20 - 0F28 CE               - movaps xmm1,xmm6
;// Update June 2020
;00000001428D4D98 | 0F44D1              | cmove edx,ecx                        
;00000001428D4D9B | 49:8BCC             | mov rcx,r12                          
;00000001428D4D9E | E8 8D87FCFF         | call re3_dump.14289D530              
;00000001428D4DA3 | 48:8B05 E6653406    | mov rax,qword ptr [148C1B390]     
;00000001428D4DAA | F341:0F1046 40      | movss xmm0,dword ptr [r14+40]     	<< INTERCEPT HERE << Address of resolution scaling 1.0 is 100% 2.0 is 200%
;00000001428D4DB0 | C74424 30 CDCCCC3D  | mov dword ptr [rsp+30],3DCCCCCD   
;00000001428D4DB8 | F3:0F5980 B4140000  | mulss xmm0,dword ptr [rax+14B4]   
;00000001428D4DC0 | 0F2FC7              | comiss xmm0,xmm7                     	<< CONTINUE HERE
;00000001428D4DC3 | F3:0F114424 2C      | movss dword ptr [rsp+2C],xmm0     
;00000001428D4DC9 | 76 0F               | jbe re3_dump.1428D4DDA               
;00000001428D4DCB | 0F2FF0              | comiss xmm6,xmm0                     
;00000001428D4DCE | 48:8D4424 2C        | lea rax,qword ptr [rsp+2C]        
;00000001428D4DD3 | 77 0A               | ja re3_dump.1428D4DDF                
;00000001428D4DD5 | 0F28CE              | movaps xmm1,xmm6                     
originalCode:
	mov [g_resolutionScaleAddress], rax
	movss xmm0,dword ptr [r14+40h]   
	mov dword ptr [rsp+30h],3DCCCCCDh
	mulss xmm0,dword ptr [rax+14B4h] 
exit:
	jmp qword ptr [_resolutionScaleReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
resolutionScaleReadInterceptor ENDP


timestopReadInterceptor PROC
;re3demo.exe+18B02102 - 48 89 FE              - mov rsi,rdi
;re3demo.exe+18B02105 - 44 38 B3 C1030000     - cmp [rbx+000003C1],r14l
;re3demo.exe+18B0210C - 74 0A                 - je re3demo.exe+18B02118
;re3demo.exe+18B0210E - C7 83 84030000 0000803F - mov [rbx+00000384],3F800000 { (0) }
;re3demo.exe+18B02118 - F3 0F10 8B A0030000   - movss xmm1,[rbx+000003A0]
;re3demo.exe+18B02120 - F3 0F10 83 84030000   - movss xmm0,[rbx+00000384]
;re3demo.exe+18B02128 - 0F2F C1               - comiss xmm0,xmm1
;re3demo.exe+18B0212B - 76 08                 - jna re3demo.exe+18B02135
;re3demo.exe+18B0212D - F3 0F11 8B 84030000   - movss [rbx+00000384],xmm1
;re3demo.exe+18B02135 - F3 0F10 8B 84030000   - movss xmm1,[rbx+00000384]
;re3demo.exe+18B0213D - F3 0F10 83 84030000   - movss xmm0,[rbx+00000384]
;re3demo.exe+18B02145 - F3 0F59 8B 80030000   - mulss xmm1,[rbx+00000380]		<< INTERCEPT HERE << read time dilation. Set to 0.0 for pause, 1.0 for continue.
;re3demo.exe+18B0214D - 0F5A C0               - cvtps2pd xmm0,xmm0
;re3demo.exe+18B02150 - F3 0F11 8B 84030000   - movss [rbx+00000384],xmm1
;re3demo.exe+18B02158 - F2 41 0F59 C1         - mulsd xmm0,xmm9					<< CONTINUE HERE
;re3demo.exe+18B0215D - F2 48 0F2C C0         - cvttsd2si rax,xmm0
;re3demo.exe+18B02162 - 0F5A C1               - cvtps2pd xmm0,xmm1
;re3demo.exe+18B02165 - 48 01 C6              - add rsi,rax
;re3demo.exe+18B02168 - F2 41 0F5E C0         - divsd xmm0,xmm8
;re3demo.exe+18B0216D - 66 0F5A C0            - cvtpd2ps xmm0,xmm0
	mov [g_timestopStructAddress], rbx
	mulss xmm1, dword ptr [rbx+00000380h]
	cvtps2pd xmm0,xmm0
	movss dword ptr [rbx+00000384h],xmm1
exit:
	jmp qword ptr [_timestopReadInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
timestopReadInterceptor ENDP


displayTypeInterceptor PROC
;re3demo.exe+18B9F894 - 45 0F57 D2            - xorps xmm10,xmm10
;re3demo.exe+18B9F898 - 44 0F2F 53 48         - comiss xmm10,[rbx+48]
;re3demo.exe+18B9F89D - 72 06                 - jb re3demo.exe+18B9F8A5
;re3demo.exe+18B9F89F - F3 44 0F11 43 48      - movss [rbx+48],xmm8
;re3demo.exe+18B9F8A5 - 44 0F2F 53 4C         - comiss xmm10,[rbx+4C]
;re3demo.exe+18B9F8AA - 72 06                 - jb re3demo.exe+18B9F8B2
;re3demo.exe+18B9F8AC - F3 44 0F11 4B 4C      - movss [rbx+4C],xmm9
;re3demo.exe+18B9F8B2 - 48 8D 45 10           - lea rax,[rbp+10]					<< JMP landing address
;re3demo.exe+18B9F8B6 - 48 89 45 18           - mov [rbp+18],rax
;re3demo.exe+18B9F8BA - 48 8D 3D 3F0746E7     - lea rdi,[re3demo.exe]				<< RIP relative, can't intercept		
;re3demo.exe+18B9F8C1 - 8B 43 74              - mov eax,[rbx+74]					<<<<<<< Read camera type. Set to 0 for FIT.
;re3demo.exe+18B9F8C4 - 83 F8 0E              - cmp eax,0E { 14 }
;re3demo.exe+18B9F8C7 - 0F84 F8000000         - je re3demo.exe+18B9F9C5
;re3demo.exe+18B9F8CD - 83 F8 11              - cmp eax,11 { 17 }
;re3demo.exe+18B9F8D0 - 0F8D EF000000         - jnl re3demo.exe+18B9F9C5
;re3demo.exe+18B9F8D6 - FF C8                 - dec eax
;re3demo.exe+18B9F8D8 - 83 F8 0F              - cmp eax,0F { 15 }
;re3demo.exe+18B9F8DB - 0F87 40010000         - ja re3demo.exe+18B9FA21
;re3demo.exe+18B9F8E1 - 48 98                 - cdqe 
;re3demo.exe+18B9F8E3 - 8B 8C 87 E03F2A02     - mov ecx,[rdi+rax*4+022A3FE0]
;re3demo.exe+18B9F8EA - 48 01 F9              - add rcx,rdi
;
; We'll intercept a bit higher in the function as a RIP value is in the way, the RBX register is the same value:
;re3demo.exe+18B9F80A - 48 89 CB              - mov rbx,rcx
;re3demo.exe+18B9F80D - 48 8B 49 10           - mov rcx,[rcx+10]
;re3demo.exe+18B9F811 - F3 0F10 0D 031E2CF0   - movss xmm1,[re3demo.exe+8E6161C] { (0,00) }
;re3demo.exe+18B9F819 - 44 0F29 44 24 60      - movaps [rsp+60],xmm8		<< INTERCEPT HERE
;re3demo.exe+18B9F81F - 44 0F29 4C 24 50      - movaps [rsp+50],xmm9
;re3demo.exe+18B9F825 - F3 0F11 45 10         - movss [rbp+10],xmm0
;re3demo.exe+18B9F82A - F3 0F11 4D 14         - movss [rbp+14],xmm1			<< CONTINUE HERE
;re3demo.exe+18B9F82F - 44 0F29 54 24 40      - movaps [rsp+40],xmm10
;re3demo.exe+18B9F835 - 48 85 C9              - test rcx,rcx
;re3demo.exe+18B9F838 - 74 20                 - je re3demo.exe+18B9F85A
	mov [g_displayTypeStructAddress], rbx
	movaps xmmword ptr [rsp+60h],xmm8	
	movaps xmmword ptr [rsp+50h],xmm9
	movss dword ptr [rbp+10h],xmm0
exit:
	jmp qword ptr [_displayTypeInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
displayTypeInterceptor ENDP


dofSelectorWriteInterceptor PROC
; use the whole block.
;re3demo.exe+1AEF4AB0 - 89 51 4C              - mov [rcx+4C],edx						<< INTERCEPT HERE << Set to 1 for low/no dof. default 0
;re3demo.exe+1AEF4AB3 - 85 D2                 - test edx,edx
;re3demo.exe+1AEF4AB5 - 74 0E                 - je re3demo.exe+1AEF4AC5
;re3demo.exe+1AEF4AB7 - 83 EA 01              - sub edx,01 { 1 }
;re3demo.exe+1AEF4ABA - 74 09                 - je re3demo.exe+1AEF4AC5
;re3demo.exe+1AEF4ABC - 83 FA 01              - cmp edx,01 { 1 }
;re3demo.exe+1AEF4ABF - 75 08                 - jne re3demo.exe+1AEF4AC9
;re3demo.exe+1AEF4AC1 - 88 51 50              - mov [rcx+50],dl
;re3demo.exe+1AEF4AC4 - C3                    - ret 
;re3demo.exe+1AEF4AC5 - C6 41 50 00           - mov byte ptr [rcx+50],00 { 0 }
;re3demo.exe+1AEF4AC9 - C3                    - ret										<< CONTINUE HERE
	mov [g_dofStructAddress], rcx
	cmp byte ptr [g_cameraEnabled], 1
	je noWrite
	mov [rcx+4Ch],edx				
noWrite:
	test edx,edx
	je setToZero
	sub edx,01h
	je setToZero
	cmp edx,01h
	jne exit
	mov [rcx+50h],dl
	jmp exit
setToZero:
	mov byte ptr [rcx+50h],00
exit:
	jmp qword ptr [_dofSelectorWriteInterceptionContinue]	; jmp back into the original game code, which is the location after the original statements above.
dofSelectorWriteInterceptor ENDP

END