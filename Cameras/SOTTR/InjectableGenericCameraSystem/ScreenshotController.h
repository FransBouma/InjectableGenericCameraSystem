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
#pragma once
#include "stdafx.h"
#include <string>
#include <vector>
#include <condition_variable>
#include <mutex>
#include "Camera.h"
#include "Defaults.h"

namespace IGCS
{
	enum class ScreenshotControllerState : short
	{
		Off,
		Grabbing,
		SavingShots,
	};

	// Simple controller class which 
	class ScreenshotController
	{
	public:
		ScreenshotController();
		~ScreenshotController();

		void configure(std::string rootFolder, int numberOfFramesToWaitBetweenSteps, float movementSpeed, float rotationSpeed);
		void startSingleShot();
		void startHorizontalPanoramaShot(Camera camera, float totalFoVInDegrees, int amountOfShots, float currentFoVInDegrees);
		void startLightfieldShot(Camera camera, float distancePerStep, int amountOfShots, bool isTestRun);
		void storeGrabbedShot(std::vector<uint8_t>);
		void setBufferSize(int width, int height);
		ScreenshotControllerState getState() { return _state; }
		void reset();
		bool shouldTakeShot();		// returns true if a shot should be taken, false otherwise. 
		void presentCalled();

	private:
		void waitForShots();
		void saveGrabbedShots();
		void saveShotToFile(std::string destinationFolder, std::vector<uint8_t> data, int frameNumber);
		std::string createScreenshotFolder();
		void moveCameraForLightfield(int direction, bool end);
		void modifyCamera();

		float _totalFoVInDegrees = 0.0f;
		float _currentFoVInDegrees = 0.0f;
		float _distancePerStep = 0.0f;
		float _movementSpeed = 0.0f;
		float _rotationSpeed = 0.0f;
		int _amountOfShotsToTake = 0;
		int _amountOfColumns = 0;
		int _amountOfRows = 0;
		int _convolutionFrameCounter = 0;		// counts down to 0 from _amountOfFramesToWaitBetweenSteps
		int _shotCounter = 0;
		int _numberOfFramesToWaitBetweenSteps = 1;
		int _framebufferWidth = 0;
		int _framebufferHeight = 0;
		ScreenshotType _typeOfShot = ScreenshotType::Lightfield;
		ScreenshotControllerState _state = ScreenshotControllerState::Off;
		ScreenshotFiletype _filetype = ScreenshotFiletype::Jpeg;
		Camera _camera;				// use local copy of the camera, passed in by the start*shot methods, passed by value. This frees us from caching the old state when manipulating the camera.
		bool _isTestRun = false;

		std::string _rootFolder;
		std::vector<std::vector<uint8_t>> _grabbedFrames;

		// Used together to make sure the main thread in System doesn't busy-wait and waits till the grabbing process has been completed.
		std::mutex _waitCompletionMutex;
		std::condition_variable _waitCompletionHandle;
	};
}

