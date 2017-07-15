#include "stdafx.h"
#include <Windows.h> 
#include <d3d11.h> 
#include "UniversalD3D11Hook.h"
#include "Console.h"
#include "MinHook.h"
#include "Globals.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"

#pragma comment(lib, "d3d11.lib")


typedef HRESULT(__stdcall *D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef void(__stdcall *D3D11DrawIndexedHook) (ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
typedef void(__stdcall *D3D11ClearRenderTargetViewHook) (ID3D11DeviceContext* pContext, ID3D11RenderTargetView *pRenderTargetView, const FLOAT ColorRGBA[4]);

ID3D11Device *pDevice = NULL;
ID3D11DeviceContext *pContext = NULL;

__int64* pSwapChainVtable = NULL;
__int64* pDeviceContextVTable = NULL;

D3D11PresentHook phookD3D11Present = NULL;
D3D11DrawIndexedHook phookD3D11DrawIndexed = NULL;
D3D11ClearRenderTargetViewHook phookD3D11ClearRenderTargetView = NULL;

bool firstTime = true;
void* detourBuffer = NULL;
bool showTestWindow = true;

HRESULT __stdcall hookD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (firstTime)
	{
		if (FAILED(pSwapChain->GetDevice(__uuidof(pDevice), (void**)&pDevice)))
		{
			IGCS::Console::WriteError("Failed to get device from hooked swapchain");
		}
		else
		{
			pDevice->GetImmediateContext(&pContext);
		}
		if(nullptr == pContext)
		{
			IGCS::Console::WriteError("Failed to get device context from hooked swapchain");
		}
		firstTime = false;
		IGCS::Console::WriteLine("Hello from hookD3DPresent");
		ImGui_ImplDX11_Init(IGCS::Globals::instance().mainWindowHandle(), pDevice, pContext);
	}
	ImGui_ImplDX11_NewFrame();
	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);     // Normally user code doesn't need/want to call it because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
	ImGui::ShowTestWindow(&showTestWindow);
	ImGui::Render();
	return phookD3D11Present(pSwapChain, SyncInterval, Flags);
}

void __stdcall hookD3D11DrawIndexed(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	return phookD3D11DrawIndexed(pContext, IndexCount, StartIndexLocation, BaseVertexLocation);
}

void __stdcall hookD3D11ClearRenderTargetView(ID3D11DeviceContext* pContext, ID3D11RenderTargetView *pRenderTargetView, const FLOAT ColorRGBA[4])
{
	return phookD3D11ClearRenderTargetView(pContext, pRenderTargetView, ColorRGBA);
}

void InitializeHook()
{
	IGCS::Console::WriteLine("InitializeHook start");
	HWND hWnd = IGCS::Globals::instance().mainWindowHandle();
	IDXGISwapChain* pSwapChain;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1
		, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, NULL, &pContext)))
	{
		IGCS::Console::WriteError("Failed to create directX device and swapchain!");
		return;
	}

	pSwapChainVtable = (__int64*)pSwapChain;
	pSwapChainVtable = (__int64*)pSwapChainVtable[0];
	pDeviceContextVTable = (__int64*)pContext;
	pDeviceContextVTable = (__int64*)pDeviceContextVTable[0];

	cout << "Present Address : " << hex << (void*)(__int64*)pSwapChainVtable[8] << endl;

	if (MH_CreateHook((LPBYTE)pSwapChainVtable[8], &hookD3D11Present, reinterpret_cast<LPVOID*>(&phookD3D11Present)) != MH_OK)
	{
		IGCS::Console::WriteError("Hooking Present failed!");
	}
	if (MH_EnableHook((LPBYTE)pSwapChainVtable[8]) != MH_OK)
	{
		IGCS::Console::WriteError("Enabling of hook failed!");
	}
	pDevice->Release();
	pContext->Release();
	pSwapChain->Release();
	IGCS::Console::WriteLine("InitializeHook end");
}
