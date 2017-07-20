#include "stdafx.h"
#include "OverlayControl.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "OverlayConsole.h"

namespace IGCS::OverlayControl
{
	void renderOverlay()
	{
		if (!_showOverlay)
		{
			return;
		}
		ImGui_ImplDX11_NewFrame();
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);     // Normally user code doesn't need/want to call it because positions are saved in .ini file anyway. 
		IGCS::OverlayConsole::instance().draw("IGCS Console", &_showOverlay);
		ImGui::Render();

	}


	void toggleOverlay()
	{
		_showOverlay = !_showOverlay;
	}


	bool isOverlayVisible()
	{
		return _showOverlay;
	}
}

