#pragma once

void NopRange(__int64 startAddress, int length);
void SetHook(__int64 hostImageAddress, __int64 startOffset, __int64 continueOffset, __int64* interceptionContinue, void* asmFunction);
