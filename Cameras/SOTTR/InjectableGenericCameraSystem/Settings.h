#pragma once
#include "stdafx.h"
#include "Gamepad.h"
#include "Utils.h"
#include "GameConstants.h"
#include "Defaults.h"
#include <map>
#include "ActionData.h"

namespace IGCS
{
	struct Settings
	{
		bool invertY;
		float fastMovementMultiplier;
		float slowMovementMultiplier;
		float movementUpMultiplier;
		float movementSpeed;
		float rotationSpeed;
		float fovChangeSpeed;
		int cameraControlDevice;		// 0==keyboard/mouse, 1 == gamepad, 2 == both, see Defaults.h
		bool allowCameraMovementWhenMenuIsUp;
		// screenshot settings
		int numberOfFramesToWaitBetweenSteps;
		float distanceBetweenLightfieldShots;
		int numberOfShotsToTake;
		int typeOfScreenshot;
		float totalPanoAngleDegrees;
		float overlapPercentagePerPanoShot;
		char screenshotFolder[_MAX_PATH + 1] = { 0 };
		
		void setValueFromMessage(BYTE payload[], DWORD payloadLength)
		{
			// byte 1 is the id of the setting. Bytes 2 and further contain the data for the setting.
			if(payloadLength<3)
			{
				return;
			}
			switch(static_cast<SettingType>(payload[1]))
			{
			case SettingType::FastMovementMultiplier:
				fastMovementMultiplier = Utils::floatFromBytes(payload, payloadLength, 2);
				break;
			case SettingType::SlowMovementMultiplier:
				slowMovementMultiplier = Utils::floatFromBytes(payload, payloadLength, 2);
				break;
			case SettingType::UpMovementMultiplier:
				movementUpMultiplier= Utils::floatFromBytes(payload, payloadLength, 2);
				break;
			case SettingType::MovementSpeed:
				movementSpeed= Utils::floatFromBytes(payload, payloadLength, 2);
				break;
			case SettingType::CameraControlDevice:
				cameraControlDevice = Utils::intFromBytes(payload, payloadLength, 2);
				break;
			case SettingType::RotationSpeed:
				rotationSpeed = Utils::floatFromBytes(payload, payloadLength, 2);
				break;
			case SettingType::InvertYLookDirection:
				invertY = payload[2] == (BYTE)1 ? true : false;
				break;
			case SettingType::FoVZoomSpeed:
				fovChangeSpeed= Utils::floatFromBytes(payload, payloadLength, 2);
				break;
			case SettingType::ShotOutputFolder:
				strncpy_s(screenshotFolder, Utils::stringFromBytes(payload, payloadLength, 2).c_str(), sizeof(screenshotFolder) - 1);
				break;
			case SettingType::ShotFramesToWait:
				numberOfFramesToWaitBetweenSteps = Utils::intFromBytes(payload, payloadLength, 2);
				break;
			case SettingType::ShotType:
				typeOfScreenshot = Utils::intFromBytes(payload, payloadLength, 2);
				break;
			case SettingType::PanoTotalFoV:
				totalPanoAngleDegrees= Utils::floatFromBytes(payload, payloadLength, 2);
				break;
			case SettingType::PanoOverlapPercentage:
				overlapPercentagePerPanoShot= Utils::floatFromBytes(payload, payloadLength, 2);
				break;
			case SettingType::LightfieldDistance:
				distanceBetweenLightfieldShots = Utils::floatFromBytes(payload, payloadLength, 2);
				break;
			case SettingType::LightfieldShotCount:
				numberOfShotsToTake = Utils::intFromBytes(payload, payloadLength, 2);
				break;
			}
		}
		

		void init(bool persistedOnly)
		{
			invertY = CONTROLLER_Y_INVERT;
			fastMovementMultiplier = FASTER_MULTIPLIER;
			slowMovementMultiplier = SLOWER_MULTIPLIER;
			movementUpMultiplier = DEFAULT_UP_MOVEMENT_MULTIPLIER;
			movementSpeed = DEFAULT_MOVEMENT_SPEED;
			rotationSpeed = DEFAULT_ROTATION_SPEED;
			fovChangeSpeed = DEFAULT_FOV_SPEED;
			cameraControlDevice = DEVICE_ID_ALL;
			allowCameraMovementWhenMenuIsUp = false;
			// Screenshot settings
			numberOfFramesToWaitBetweenSteps = 5;
			distanceBetweenLightfieldShots = 1.0f;
			numberOfShotsToTake = 45;
			typeOfScreenshot = (int)ScreenshotType::Lightfield;
			totalPanoAngleDegrees = 110.0f;
			overlapPercentagePerPanoShot = 80.0f;
			strncpy_s(screenshotFolder, "c:\\", sizeof(screenshotFolder)-1);
		}
	};
}