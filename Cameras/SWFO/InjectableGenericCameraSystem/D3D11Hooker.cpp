#include "stdafx.h"
#include <d3d11.h> 
#include "D3D11Hooker.h"
#include "Console.h"
#include "MinHook.h"
#include "Globals.h"
#include "imgui.h"
#include "Defaults.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "OverlayControl.h"
#include "OverlayConsole.h"
#include "Input.h"
#include <atomic>
#include <thread>

#pragma comment(lib, "d3d11.lib")

namespace IGCS::D3D11Hooker
{
	#define DXGI_PRESENT_INDEX			8
	#define DXGI_RESIZEBUFFERS_INDEX	13

	//--------------------------------------------------------------------------------------------------------------------------------
	// Forward declarations
	void createRenderTarget(IDXGISwapChain* pSwapChain);
	void cleanupRenderTarget();
	std::vector<uint8_t> capture_frame(IDXGISwapChain* pSwapChain);

	//--------------------------------------------------------------------------------------------------------------------------------
	// Typedefs of functions to hook
	typedef HRESULT(__stdcall *DXGIPresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	typedef HRESULT(__stdcall *DXGIResizeBuffersHook) (IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);

	static ID3D11Device* _device = nullptr;
	static ID3D11DeviceContext* _context = nullptr;
	static ID3D11RenderTargetView* _mainRenderTargetView = nullptr;

	//--------------------------------------------------------------------------------------------------------------------------------
	// Pointers to the original hooked functions
	static DXGIPresentHook hookedDXGIPresent = nullptr;
	static DXGIResizeBuffersHook hookedDXGIResizeBuffers = nullptr;

	static bool _tmpSwapChainInitialized = false;
	static atomic_bool _imGuiInitializing = false;
	static atomic_bool _initializeDeviceAndContext = true;
	static atomic_bool _imGuiDeviceObjectsCreated = false;
	static atomic_bool _presentInProgress = false;

	HRESULT __stdcall detourDXGIResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags)
	{
		Globals::instance().getScreenshotController().reset();		// kill off any in-progress screenshot process
		_imGuiInitializing = true;
		ImGui_ImplDX11_InvalidateDeviceObjects();
		cleanupRenderTarget();
		OverlayConsole::instance().logDebug("Resizing buffers to %d x %d", width, height);
		HRESULT toReturn = hookedDXGIResizeBuffers(pSwapChain, bufferCount, width, height, newFormat, swapChainFlags);
		OverlayConsole::instance().logDebug("Resized buffers to %d x %d", width, height);
		createRenderTarget(pSwapChain);
		_imGuiDeviceObjectsCreated = ImGui_ImplDX11_CreateDeviceObjects();
		_imGuiInitializing = false;
		return toReturn;
	}


	HRESULT __stdcall detourDXGIPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		if (_presentInProgress)
		{
			return S_OK;
		}
		_presentInProgress = true;
		bool validFrame = false;
		UINT flagsToPass = Flags;
		ScreenshotController& screenshotController = Globals::instance().getScreenshotController();
		if (_tmpSwapChainInitialized)
		{
			if (!(Flags & DXGI_PRESENT_TEST) && !_imGuiInitializing)
			{
				if (_initializeDeviceAndContext)
				{
					if (FAILED(pSwapChain->GetDevice(__uuidof(_device), (void**)&_device)))
					{
						IGCS::Console::WriteError("Failed to get device from hooked swapchain");
					}
					else
					{
						OverlayConsole::instance().logDebug("DX11 Device: %p", (void*)_device);
						_device->GetImmediateContext(&_context);
					}
					if (nullptr == _context)
					{
						IGCS::Console::WriteError("Failed to get device context from hooked swapchain");
					}
					else
					{
						OverlayConsole::instance().logDebug("DX11 Context: %p", (void*)_context);
					}
					createRenderTarget(pSwapChain);
					ImGui_ImplDX11_Init(_device, _context);
					if (!_imGuiDeviceObjectsCreated)
					{
						_imGuiDeviceObjectsCreated = ImGui_ImplDX11_CreateDeviceObjects();
					}
					_initializeDeviceAndContext = false;
				}
				validFrame = true;
				// render our own stuff
				OverlayControl::renderOverlay();
				_context->OMSetRenderTargets(1, &_mainRenderTargetView, NULL);
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
				Input::resetKeyStates();
				Input::resetMouseState();
			}
		}
		bool grabFrame = false;
		if (validFrame && screenshotController.shouldTakeShot())
		{
			// make sure the Present call doesn't synchronize, so it's waiting for the VBL and doesn't unbind the backbuffer. 
			// Doesn't work in this game... 
			//flagsToPass |= DXGI_PRESENT_DO_NOT_SEQUENCE;
			grabFrame = true;
		}
		HRESULT toReturn = hookedDXGIPresent(pSwapChain, SyncInterval, flagsToPass);
		// if we have to grab the frame, do it now.
		if (grabFrame)
		{
			screenshotController.storeGrabbedShot(capture_frame(pSwapChain));
		}
		screenshotController.presentCalled();
		_presentInProgress = false;
		return toReturn;
	}


	void initializeHook()
	{
		_tmpSwapChainInitialized = false;
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = IGCS::Globals::instance().mainWindowHandle();
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		ID3D11Device *pTmpDevice = NULL;
		ID3D11DeviceContext *pTmpContext = NULL;
		IDXGISwapChain* pTmpSwapChain;
		if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &pTmpSwapChain, &pTmpDevice, NULL, &pTmpContext)))
		{
			IGCS::Console::WriteError("Failed to create directX device and swapchain!");
			return;
		}

		__int64* pSwapChainVtable = NULL;
		pSwapChainVtable = (__int64*)pTmpSwapChain;
		pSwapChainVtable = (__int64*)pSwapChainVtable[0];

		OverlayConsole::instance().logDebug("Present Address: %p", (void*)(__int64*)pSwapChainVtable[DXGI_PRESENT_INDEX]);

		if (MH_CreateHook((LPBYTE)pSwapChainVtable[DXGI_PRESENT_INDEX], &detourDXGIPresent, reinterpret_cast<LPVOID*>(&hookedDXGIPresent)) != MH_OK)
		{
			IGCS::Console::WriteError("Hooking Present failed!");
		}
		if (MH_EnableHook((LPBYTE)pSwapChainVtable[DXGI_PRESENT_INDEX]) != MH_OK)
		{
			IGCS::Console::WriteError("Enabling of Present hook failed!");
		}

		OverlayConsole::instance().logDebug("ResizeBuffers Address: %p", (__int64*)pSwapChainVtable[DXGI_RESIZEBUFFERS_INDEX]);

		if (MH_CreateHook((LPBYTE)pSwapChainVtable[DXGI_RESIZEBUFFERS_INDEX], &detourDXGIResizeBuffers, reinterpret_cast<LPVOID*>(&hookedDXGIResizeBuffers)) != MH_OK)
		{
			IGCS::Console::WriteError("Hooking ResizeBuffers failed!");
		}
		if (MH_EnableHook((LPBYTE)pSwapChainVtable[DXGI_RESIZEBUFFERS_INDEX]) != MH_OK)
		{
			IGCS::Console::WriteError("Enabling of ResizeBuffers hook failed!");
		}

		pTmpDevice->Release();
		pTmpContext->Release();
		pTmpSwapChain->Release();
		_tmpSwapChainInitialized = true;

		OverlayConsole::instance().logDebug("DX11 hooks set");
	}


	void createRenderTarget(IDXGISwapChain* pSwapChain)
	{
		if (nullptr == _device)
		{
			// not yet initialized, skip
			return;
		}
		DXGI_SWAP_CHAIN_DESC sd;
		pSwapChain->GetDesc(&sd);
		ID3D11Texture2D* pBackBuffer;
		D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
		ZeroMemory(&render_target_view_desc, sizeof(render_target_view_desc));
		render_target_view_desc.Format = sd.BufferDesc.Format;
		render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		_device->CreateRenderTargetView(pBackBuffer, &render_target_view_desc, &_mainRenderTargetView);
		// get the width/height of the back buffer. 
		D3D11_TEXTURE2D_DESC StagingDesc;
		pBackBuffer->GetDesc(&StagingDesc);
		Globals::instance().getScreenshotController().setBufferSize(StagingDesc.Width, StagingDesc.Height);
		pBackBuffer->Release();
	}


	void cleanupRenderTarget()
	{
		if (nullptr != _mainRenderTargetView)
		{
			_mainRenderTargetView->Release();
			_mainRenderTargetView = nullptr;
		}
	}


	std::vector<uint8_t> capture_frame(IDXGISwapChain* pSwapChain)
	{
		OverlayConsole::instance().logDebug("capture_frame()");

		D3D11_TEXTURE2D_DESC StagingDesc;
		ID3D11Texture2D* pBackBuffer = NULL;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
		pBackBuffer->GetDesc(&StagingDesc);
		StagingDesc.Usage = D3D11_USAGE_STAGING;
		StagingDesc.BindFlags = 0;
		StagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		ID3D11Texture2D* pBackBufferStaging = NULL;
		HRESULT hr = _device->CreateTexture2D(&StagingDesc, NULL, &pBackBufferStaging);
		_context->CopyResource(pBackBufferStaging, pBackBuffer);
		D3D11_MAPPED_SUBRESOURCE mapped;
		hr = _context->Map(pBackBufferStaging, 0, D3D11_MAP_READ, 0, &mapped);
		if (FAILED(hr))
		{
			IGCS::Console::WriteError("Failed to map staging resource with screenshot capture!");
			std::vector<uint8_t> failed;
			return failed;
		}
		
		std::vector<uint8_t> fbdata(StagingDesc.Width * StagingDesc.Height * 4);
		uint8_t* buffer = fbdata.data();
		auto mapped_data = static_cast<BYTE*>(mapped.pData);
		const UINT pitch = StagingDesc.Width * 4;
		for (UINT y = 0; y < StagingDesc.Height; y++)
		{
			// From Reshade.
			if (StagingDesc.Format == DXGI_FORMAT_R10G10B10A2_UNORM || StagingDesc.Format == DXGI_FORMAT_R10G10B10A2_UINT)
			{
				for (uint32_t x = 0; x < pitch; x += 4)
				{
					const uint32_t rgba = *reinterpret_cast<const uint32_t*>(mapped_data + x);
					// Divide by 4 to get 10-bit range (0-1023) into 8-bit range (0-255)
					buffer[x + 0] = ((rgba & 0x3FF) / 4) & 0xFF;
					buffer[x + 1] = (((rgba & 0xFFC00) >> 10) / 4) & 0xFF;
					buffer[x + 2] = (((rgba & 0x3FF00000) >> 20) / 4) & 0xFF;
					buffer[x + 3] = 0xFF;
				}
			}
			else
			{
				memcpy(buffer, mapped_data, min(pitch, static_cast<UINT>(mapped.RowPitch)));

				for (UINT x = 0; x < pitch; x += 4)
				{
					buffer[x + 3] = 0xFF;

					if (StagingDesc.Format == DXGI_FORMAT_B8G8R8A8_UNORM || StagingDesc.Format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB ||
						StagingDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM || StagingDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB ||
						StagingDesc.Format == DXGI_FORMAT_B8G8R8A8_TYPELESS || StagingDesc.Format == DXGI_FORMAT_B8G8R8X8_TYPELESS)
					{
						std::swap(buffer[x + 0], buffer[x + 2]);
					}
				}
			}
			buffer += pitch;
			mapped_data += mapped.RowPitch;
		}
		_context->Unmap(pBackBufferStaging, 0);
		pBackBufferStaging->Release();
		pBackBuffer->Release();
		return fbdata;
	}
}