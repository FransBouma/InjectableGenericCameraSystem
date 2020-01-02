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
		int multiShotNumberOfFramesToWaitBetweenSteps;
		float lightFieldDistanceBetweenShots;
		int multiShotNumberOfShotsToTake;
		int typeOfScreenshot;
		float panoramaTotalAngleDegrees;
		float panoramaOverlapPercentagePerShot;
		char screenshotFolder[_MAX_PATH + 1] = { 0 };
		
		void setValueFromMessage(uint8_t payload[], DWORD payloadLength)
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
				invertY = payload[2] == (uint8_t)1 ? true : false;
				break;
			case SettingType::FoVZoomSpeed:
				fovChangeSpeed= Utils::floatFromBytes(payload, payloadLength, 2);
				break;
			case SettingType::ShotOutputFolder:
				strncpy_s(screenshotFolder, Utils::stringFromBytes(payload, payloadLength, 2).c_str(), sizeof(screenshotFolder) - 1);
				break;
			case SettingType::ShotFramesToWait:
				multiShotNumberOfFramesToWaitBetweenSteps = Utils::intFromBytes(payload, payloadLength, 2);
				break;
			case SettingType::ShotType:
				typeOfScreenshot = Utils::intFromBytes(payload, payloadLength, 2);
				break;
			case SettingType::PanoTotalFoV:
				panoramaTotalAngleDegrees= Utils::floatFromBytes(payload, payloadLength, 2);
				break;
			case SettingType::PanoOverlapPercentage:
				panoramaOverlapPercentagePerShot= Utils::floatFromBytes(payload, payloadLength, 2);
				break;
			case SettingType::LightfieldDistance:
				lightFieldDistanceBetweenShots = Utils::floatFromBytes(payload, payloadLength, 2);
				break;
			case SettingType::LightfieldShotCount:
				multiShotNumberOfShotsToTake = Utils::intFromBytes(payload, payloadLength, 2);
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
			multiShotNumberOfFramesToWaitBetweenSteps = 5;
			lightFieldDistanceBetweenShots = 1.0f;
			multiShotNumberOfShotsToTake = 45;
			typeOfScreenshot = (int)ScreenshotType::Lightfield;
			panoramaTotalAngleDegrees = 110.0f;
			panoramaOverlapPercentagePerShot = 80.0f;
			strncpy_s(screenshotFolder, "c:\\", sizeof(screenshotFolder)-1);
		}
	};
}