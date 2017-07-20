#pragma once

namespace IGCS::OverlayControl
{
	static bool _showOverlay = true;

	void toggleOverlay();
	void renderOverlay();
	bool isOverlayVisible();
}

