////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2019, Frans Bouma
// All rights reserved.
// https://github.com/FransBouma/InjectableGenericCameraSystem
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
//
//  * Redistributions of source code must retain the above copyright notice, this
//	  list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and / or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ScreenshotController.h"
#include "Utils.h"
#include "Defaults.h"
#include "OverlayConsole.h"
#include "OverlayControl.h"
#include <direct.h>
#include "CameraManipulator.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

namespace IGCS
{
	ScreenshotController::ScreenshotController() : _camera{ Camera() }
	{
	}

	ScreenshotController::~ScreenshotController()
	{}


	void ScreenshotController::configure(string rootFolder, int numberOfFramesToWaitBetweenSteps, float movementSpeed, float rotationSpeed)
	{
		if (_state != ScreenshotControllerState::Off)
		{
			// Initialize can't be called when a screenhot is in progress. ignore
			return;
		}
		_rootFolder = rootFolder;
		_numberOfFramesToWaitBetweenSteps = numberOfFramesToWaitBetweenSteps;
		_movementSpeed = movementSpeed;
		_rotationSpeed = rotationSpeed;
	}


	bool ScreenshotController::shouldTakeShot()
	{
		if (_convolutionFrameCounter > 0)
		{
			// always false
			return false;
		}
		return _state == ScreenshotControllerState::Grabbing;
	}


	void ScreenshotController::presentCalled()
	{
		if (_convolutionFrameCounter > 0)
		{
			_convolutionFrameCounter--;
		}
	}


	void ScreenshotController::setBufferSize(int width, int height)
	{
		if (_state != ScreenshotControllerState::Off)
		{
			// ignore
			return;
		}
		_framebufferHeight = height;
		_framebufferWidth = width;
	}


	void ScreenshotController::startSingleShot()
	{
		OverlayConsole::instance().logDebug("strtSingleShot start.");
		reset();
		_typeOfShot = ScreenshotType::SingleShot;
		_state = ScreenshotControllerState::Grabbing;
		// we'll wait now till all the shots are taken. 
		waitForShots();
		OverlayControl::addNotification("Single screenshot taken. Writing to disk...");
		saveGrabbedShots();
		OverlayControl::addNotification("Single screenshot done.");
		// done
	}

	void ScreenshotController::startHorizontalPanoramaShot(Camera camera, float totalFoVInDegrees, int amountOfShots, float currentFoVInDegrees)
	{
		reset();
		_camera = camera;
		_totalFoVInDegrees = totalFoVInDegrees;
		_amountOfShotsToTake = amountOfShots;
		_currentFoVInDegrees = currentFoVInDegrees;
		_typeOfShot = ScreenshotType::HorizontalPanorama;
	}


	void ScreenshotController::startLightfieldShot(Camera camera, float distancePerStep, int amountOfShots, bool isTestRun)
	{
		OverlayConsole::instance().logDebug("startLightfield shot start. isTestRun: %d", isTestRun);
		reset();
		_isTestRun = isTestRun;
		_camera = camera;
		_distancePerStep = distancePerStep;
		_amountOfShotsToTake = amountOfShots;
		_typeOfShot = ScreenshotType::Lightfield;
		// move to start
		moveCameraForLightfield(-1, true);
		// set convolution counter to its initial value
		_convolutionFrameCounter = _numberOfFramesToWaitBetweenSteps;
		_state = ScreenshotControllerState::Grabbing;
		// we'll wait now till all the shots are taken. 
		waitForShots();
		OverlayControl::addNotification("All Lightfield shots taken. Writing shots to disk...");
		saveGrabbedShots();
		OverlayControl::addNotification("Lightfield done.");
		// done
	}


	void ScreenshotController::storeGrabbedShot(std::vector<uint8_t> grabbedShot)
	{
		if (grabbedShot.size() <= 0)
		{
			// failed
			return;
		}
		_grabbedFrames.push_back(grabbedShot);
		_shotCounter++;
		if (_shotCounter > _amountOfShotsToTake)
		{
			// we're done. Move to the next state, which is saving shots. 
			unique_lock<mutex> lock(_waitCompletionMutex);
			_state = ScreenshotControllerState::SavingShots;
			// tell the waiting thread to wake up so the system can proceed as normal.
			_waitCompletionHandle.notify_all();
		}
		else
		{
			modifyCamera();
			_convolutionFrameCounter = _numberOfFramesToWaitBetweenSteps;
		}
	}


	void ScreenshotController::saveGrabbedShots()
	{
		if (_grabbedFrames.size() <= 0)
		{
			return;
		}
		if (!_isTestRun)
		{
			string destinationFolder = createScreenshotFolder();
			int frameNumber = 0;
			for (std::vector<uint8_t> frame : _grabbedFrames)
			{
				saveShotToFile(destinationFolder, frame, frameNumber);
				frameNumber++;
			}
		}
		// done
		_grabbedFrames.clear();
		_state = ScreenshotControllerState::Off;
	}


	void ScreenshotController::saveShotToFile(std::string destinationFolder, std::vector<uint8_t> data, int frameNumber)
	{
		bool saveSuccessful = false;
		string filename = "";
		switch (_filetype)
		{
		case ScreenshotFiletype::Bmp:
			filename = Utils::formatString("%s\\%d.bmp", destinationFolder.c_str(), frameNumber);
			saveSuccessful = stbi_write_bmp(filename.c_str(), _framebufferWidth, _framebufferHeight, 4, data.data()) != 0;
			break;
		case ScreenshotFiletype::Jpeg:
			filename = Utils::formatString("%s\\%d.jpg", destinationFolder.c_str(), frameNumber);
			saveSuccessful = stbi_write_jpg(filename.c_str(), _framebufferWidth, _framebufferHeight, 4, data.data(), IGCS_JPG_SCREENSHOT_QUALITY) != 0;
			break;
		case ScreenshotFiletype::Png:
			filename = Utils::formatString("%s\\%d.png", destinationFolder.c_str(), frameNumber);
			saveSuccessful = stbi_write_png(filename.c_str(), _framebufferWidth, _framebufferHeight, 8, data.data(), 4 * _framebufferWidth) != 0;
			break;
		}
		if (saveSuccessful)
		{
			OverlayConsole::instance().logDebug("Successfully wrote screenshot of dimensions %dx%d to... %s", _framebufferWidth, _framebufferHeight, filename.c_str());
		}
		else
		{
			OverlayConsole::instance().logDebug("Failed to write screenshot of dimensions %dx%d to... %s", _framebufferWidth, _framebufferHeight, filename.c_str());
		}
	}


	string ScreenshotController::createScreenshotFolder()
	{
		time_t t = time(nullptr);
		tm tm;
		localtime_s(&tm, &t);
		string optionalBackslash = (_rootFolder.ends_with('\\')) ? "" : "\\";
		string folderName = Utils::formatString("%s%s%.4d-%.2d-%.2d-%.2d-%.2d-%.2d", _rootFolder.c_str(), optionalBackslash.c_str(), (tm.tm_year + 1900), (tm.tm_mon + 1), tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec);
		mkdir(folderName.c_str());
		return folderName;
	}


	void ScreenshotController::waitForShots()
	{
		unique_lock<mutex> lock(_waitCompletionMutex);
		while (_state != ScreenshotControllerState::SavingShots)
		{
			_waitCompletionHandle.wait(lock);
		}
		// state has changed, we're notified so we're all goed to save the shots. 
	}


	void ScreenshotController::modifyCamera()
	{
		// based on the type of the shot, we'll either rotate or move.
		switch (_typeOfShot)
		{
		case ScreenshotType::HorizontalPanorama:
			// TODO: IMPLEMENT
			break;
		case ScreenshotType::Lightfield:
			moveCameraForLightfield(1, false);
			break;
		case ScreenshotType::SingleShot:
			// nothing
			break;
		}
	}


	void ScreenshotController::moveCameraForLightfield(int direction, bool end)
	{
		_camera.resetMovement();
		float dist = direction * _distancePerStep;
		if (end)
		{
			dist *= 0.5f * _amountOfShotsToTake;
		}
		float stepSize = dist / _movementSpeed; // scale to be independent of camera movement speed
		_camera.moveRight(stepSize);
		GameSpecific::CameraManipulator::updateCameraDataInGameData(_camera);
	}


	void ScreenshotController::reset()
	{
		// don't reset framebuffer width/height, numberOfFramesToWaitBetweenSteps, movementSpeed, 
		// rotationSpeed, rootFolder as those are set through configure!
		_typeOfShot = ScreenshotType::Lightfield;
		_state = ScreenshotControllerState::Off;
		_totalFoVInDegrees = 0.0f;
		_currentFoVInDegrees = 0.0f;
		_distancePerStep = 0.0f;
		_amountOfShotsToTake = 0;
		_amountOfColumns = 0;
		_amountOfRows = 0;
		_convolutionFrameCounter = 0;
		_shotCounter = 0;
		_isTestRun = false;

		_grabbedFrames.clear();
	}
}
