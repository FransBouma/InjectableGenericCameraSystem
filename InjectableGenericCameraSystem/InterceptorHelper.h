#pragma once

void DisableFoVWrite(LPBYTE hostImageAddress);
void SetCameraStructInterceptorHook(LPBYTE hostImageAddress);
void SetCameraWriteInterceptorHooks(LPBYTE hostImageAddress);