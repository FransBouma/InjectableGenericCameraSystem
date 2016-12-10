#pragma once
#include "stdafx.h"

using namespace DirectX;

void WriteNewCameraValuesToGameData(XMVECTOR newLookQuaternion, XMFLOAT3 newCoords);
void WaitForCameraStructAddresses();
void RestoreOriginalCameraValues();
void CacheOriginalCameraValues();
void ResetFoV();
void ChangeFoV(float amount);
XMFLOAT3 GetCurrentCameraCoords();
void SetTimeStopValue(LPBYTE hostImageAddress, byte newValue);