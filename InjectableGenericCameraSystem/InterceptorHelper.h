#pragma once

bool PerformAOBScans(LPBYTE hostImageAddress);
void DisableFoVWrite(LPBYTE hostImageAddress);
void SetCameraStructInterceptorHook(LPBYTE hostImageAddress);
void SetCameraWriteInterceptorHooks(LPBYTE hostImageAddress);
void SetTimestopInterceptorHooks(LPBYTE hostImageAddress);