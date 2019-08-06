#pragma once

#include "stdafx.h"
#include <d3d11.h> 

namespace IGCS::D3D11Hooker
{
	void initializeHook();

	void takeScreenshot(char* filename, int framesToGrab_);
	std::vector<uint8_t> capture_frame(IDXGISwapChain* pSwapChain);
	void saveToFile(std::vector<uint8_t> data, char* base_filename, int framenum);
	void saveAllFiles();
	bool isDoneSavingImages();
	void syncFramesToGrab(int ftgs);
	int framesRemaining();
}