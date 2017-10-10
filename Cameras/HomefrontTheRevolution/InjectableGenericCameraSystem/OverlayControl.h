#pragma once

#include "stdafx.h"
#include <d3d11.h>

namespace IGCS::OverlayControl
{
	void initImGui(ID3D11Device* device, ID3D11DeviceContext* context);
	void toggleOverlay();
	void renderOverlay();
	bool isMainMenuVisible();
	void addNotification(std::string notificationText);
}

