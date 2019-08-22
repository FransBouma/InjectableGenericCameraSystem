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
#include "stb_image_write.h"
#include "stb_image_resize.h"
#include "Defaults.h"
#include "OverlayConsole.h"
#include "OverlayControl.h"
#include <direct.h>

using namespace std;

namespace IGCS
{
	ScreenshotController::ScreenshotController()
	{
	}

	ScreenshotController::~ScreenshotController()
	{}


	void ScreenshotController::initialize(string rootFolder, int amountOfFramesToWaitbetweenSteps, float movementSpeed, float rotationSpeed)
	{
		if (_state != ScreenshotControllerState::Off)
		{
			// Initialize can't be called when a screenhot is in progress. ignore
			return;
		}
		_rootFolder = rootFolder;
		_amountOfFramesToWaitbetweenSteps = amountOfFramesToWaitbetweenSteps;
		_movementSpeed = movementSpeed;
		_rotationSpeed = rotationSpeed;
	}


	bool ScreenshotController::shouldTakeShot()
	{
		return _state == ScreenshotControllerState::Grabbing;
	}


	void ScreenshotController::presentCalled()
	{
// TODO: add convolution counter handling
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
		reset();
		_typeOfShot = ScreenshotType::SingleShot;
		_state = ScreenshotControllerState::Grabbing;
		// we'll wait now till all the shots are taken. 
		waitForShots();
		saveGrabbedShots();
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


	void ScreenshotController::startLightfieldShot(Camera camera, float distancePerStep, int amountOfShots)
	{
		reset();
		_camera = camera;
		_distancePerStep = distancePerStep;
		_amountOfShotsToTake = amountOfShots;
		_typeOfShot = ScreenshotType::Lightfield;
		_state = ScreenshotControllerState::Grabbing;
		// we'll wait now till all the shots are taken. 
		waitForShots();
		saveGrabbedShots();
		// done
	}


	void ScreenshotController::startTiledGridShot(Camera camera, int amountOfColumns, int amountOfRows, float currentFoVInDegrees)
	{
		reset();
		_camera = camera;
		_amountOfColumns = amountOfColumns;
		_amountOfRows = amountOfRows;
		_currentFoVInDegrees = currentFoVInDegrees;
	}


	void ScreenshotController::storeGrabbedShot(std::vector<uint8_t> grabbedShot)
	{
		if (grabbedShot.capacity <= 0)
		{
			// failed
			return;
		}
		_grabbedFrames.push_back(grabbedShot);
// TODO: Add camera move
		_shotCounter++;
		if (_shotCounter > _amountOfShotsToTake)
		{
			// we're done. Move to the next state, which is saving shots. 
			_state = ScreenshotControllerState::SavingShots;
			// tell the waiting thread to wake up so the system can proceed as normal.
			_waitCompletionHandle.notify_all();
		}
	}


	void ScreenshotController::saveGrabbedShots()
	{
		if (_grabbedFrames.size() <= 0)
		{
			return;
		}
		string destinationFolder = createScreenshotFolder();
		int frameNumber = 0;
		for (std::vector<uint8_t> frame : _grabbedFrames) 
		{
			saveShotToFile(destinationFolder, frame, frameNumber);
			frameNumber++;
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
				filename = Utils::formatString("%s\\%d.bmp", destinationFolder, frameNumber);
				saveSuccessful = stbi_write_bmp(filename.c_str(), _framebufferWidth, _framebufferHeight, 0, data.data()) != 0;
				break;
			case ScreenshotFiletype::Jpeg:
				filename = Utils::formatString("%s\\%d.jpg", destinationFolder, frameNumber);
				saveSuccessful = stbi_write_jpg(filename.c_str(), _framebufferWidth, _framebufferHeight, 0, data.data(), IGCS_JPG_SCREENSHOT_QUALITY) != 0;
				break;
			case ScreenshotFiletype::Png:
				filename = Utils::formatString("%s\\%d.png", destinationFolder, frameNumber);
				saveSuccessful = stbi_write_png(filename.c_str(), _framebufferWidth, _framebufferHeight, 8, data.data(), 4 * _framebufferWidth) != 0;
				break;
		}
		if (saveSuccessful)
		{
			OverlayConsole::instance().logDebug("Successfully wrote screenshot of dimensions %dx%d to... %s", _framebufferWidth, _framebufferHeight, filename);
		}
		else 
		{
			OverlayConsole::instance().logDebug("Failed to write screenshot of dimensions %dx%d to... %s", _framebufferWidth, _framebufferHeight, filename);
		}
	}


	string ScreenshotController::createScreenshotFolder()
	{
		time_t t = time(nullptr);
		tm tm;
		localtime_s(&tm, &t);
		int year = tm.tm_year + 1900;
		int month = tm.tm_mon + 1;
		int day = tm.tm_mday;
		int hour = tm.tm_hour * 3600 + tm.tm_min * 60 + tm.tm_sec;
		hour = hour / 3600;
		int minute = (hour - (hour * 3600)) / 60;
		int seconds = hour - (hour * 3600) - (minute * 60);
		string optionalBackslash = (_rootFolder.ends_with('\\')) ? "" : "\\";
		string folderName = Utils::formatString("%s%s%.4d-%.2d-%.2d-%.2d-%.2d-%.2d", _rootFolder, optionalBackslash, year, month, day, hour, minute, seconds);
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


	void ScreenshotController::reset()
	{
		_typeOfShot = ScreenshotType::Undefined;
		_state = ScreenshotControllerState::Off;
		_totalFoVInDegrees = 0.0f;
		_currentFoVInDegrees = 0.0f;
		_distancePerStep = 0.0f;
		_amountOfShotsToTake = 0;
		_amountOfColumns = 0;
		_amountOfRows = 0;
		_convolutionFrameCounter = 0;
		_shotCounter = 0;
		_amountOfFramesToWaitbetweenSteps = 0;
		_movementSpeed = 0.0f;
		_rotationSpeed = 0.0f;

		_grabbedFrames.clear();

		// don't reset framebuffer width/height!
	}


	void ScreenshotController::moveLightfield(int direction, bool end)
	{
		moveLightfield(direction, end, true);
	}


	void ScreenshotController::moveLightfield(int direction, bool end, bool log)
	{
		_camera.resetMovement();
		float dist = direction * _distancePerStep;
		if (end) 
		{
			if (log) 
			{
				//OverlayConsole::instance().logLine((direction > 0) ? "Move to end." : "Move to start.");
				OverlayControl::addNotification((direction > 0) ? "Move to end." : "Move to start.");
			}
			dist *= 0.5f * _amountOfShotsToTake;
			_camera.moveRight(dist / _movementSpeed); // scale to be independent of camera movement speed
			return;
		}
		if (log) 
		{
			//OverlayConsole::instance().logLine((direction > 0) ? "Move to next." : "Move to previous.");
			OverlayControl::addNotification((direction > 0) ? "Move to next." : "Move to previous.");
		}
		_camera.moveRight(dist / _movementSpeed); // scale to be independent of camera movement speed
	}

}
