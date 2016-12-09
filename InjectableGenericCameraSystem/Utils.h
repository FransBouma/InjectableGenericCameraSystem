#pragma once

void NopRange(LPBYTE startAddress, int length);
void SetHook(LPBYTE hostImageAddress, DWORD startOffset, DWORD continueOffset, LPBYTE* interceptionContinue, void* asmFunction);
DWORD AOBFindSignature(LPVOID baseAddress, DWORD imageSize, PCHAR signature, PCHAR mask);