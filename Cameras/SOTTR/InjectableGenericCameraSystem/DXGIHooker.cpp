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
#include <d3d11.h>
#include <d3d12.h> 
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_4.h>
#include "DXGIHooker.h"
#include "MinHook.h"
#include "Globals.h"
#include "Defaults.h"
#include "Input.h"
#include <atomic>
#include <thread>
#include "MessageHandler.h"
#include <VersionHelpers.h>
#include <atlbase.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

namespace IGCS::DXGIHooker
{
	#define DXGI_PRESENT_INDEX			8
	#define DXGI_RESIZEBUFFERS_INDEX	13
	#define DXGI_PRESENT1_INDEX			22

	//--------------------------------------------------------------------------------------------------------------------------------
	// Forward declarations
	std::vector<uint8_t> capture_frame_DX11(IDXGISwapChain* pSwapChain);
	std::vector<uint8_t> capture_frame_DX12(IDXGISwapChain* pSwapChain);
	bool handlePresentCore(IDXGISwapChain* pSwapChain, UINT Flags);

	//--------------------------------------------------------------------------------------------------------------------------------
	// Typedefs of functions to hook
	typedef HRESULT(__stdcall *DXGIPresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	typedef HRESULT(__stdcall *DXGIResizeBuffersHook) (IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
	typedef HRESULT(__stdcall* DXGIPresent1Hook) (IDXGISwapChain1* pSwapChain, UINT SyncInterval, UINT PresentFlags, const DXGI_PRESENT_PARAMETERS* pPresentParameters);

	static ID3D11Device* _deviceDX11 = nullptr;
	static ID3D11DeviceContext* _contextDX11 = nullptr;
	static ID3D12Device* _deviceDX12 = nullptr;
	static ID3D12CommandQueue* _commandQueueDX12 = nullptr;
	
	//--------------------------------------------------------------------------------------------------------------------------------
	// Pointers to the original hooked functions
	static DXGIPresentHook hookedDXGIPresent = nullptr;
	static DXGIResizeBuffersHook hookedDXGIResizeBuffers = nullptr;
	static DXGIPresent1Hook hookedDXGIPresent1 = nullptr;

	static atomic_bool _initializeDeviceAndContext = true;
	static atomic_bool _presentInProgress = false;
	static atomic_bool _direct3D11Available = false;
	static atomic_bool _direct3D12Available = false;
	

	HRESULT __stdcall detourDXGIResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags)
	{
		Globals::instance().getScreenshotController().reset();		// kill off any in-progress screenshot process
		return hookedDXGIResizeBuffers(pSwapChain, bufferCount, width, height, newFormat, swapChainFlags);
	}


	void grabShotIfRequired(IDXGISwapChain* pSwapChain, ScreenshotController& screenshotController, bool validFrameToGrab)
	{
		if (validFrameToGrab && screenshotController.shouldTakeShot())
		{
			vector<uint8_t> screenshotBytes;
			if (_direct3D12Available)
			{
				screenshotBytes = capture_frame_DX12(pSwapChain);
			}
			else
			{
				if (_direct3D11Available)
				{
					screenshotBytes = capture_frame_DX11(pSwapChain);
				}
			}
			if (!screenshotBytes.empty())
			{
				screenshotController.storeGrabbedShot(screenshotBytes);
			}
		}
	}


	HRESULT __stdcall detourDXGIPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		if (_presentInProgress)
		{
			return S_OK;
		}
		const bool validFrameToGrab = handlePresentCore(pSwapChain, Flags);
		const HRESULT toReturn = hookedDXGIPresent(pSwapChain, SyncInterval, Flags);
		// if we have to grab the frame, do it now.
		ScreenshotController& screenshotController = Globals::instance().getScreenshotController();
		grabShotIfRequired(pSwapChain, screenshotController, validFrameToGrab);
		screenshotController.presentCalled();
		_presentInProgress = false;
		return toReturn;
	}

	
	HRESULT __stdcall detourDXGIPresent1(IDXGISwapChain1* pSwapChain, UINT SyncInterval, UINT PresentFlags, const DXGI_PRESENT_PARAMETERS* pPresentParameters)
	{
		if (_presentInProgress)
		{
			return S_OK;
		}
		const bool validFrameToGrab = handlePresentCore(pSwapChain, PresentFlags);
		const HRESULT toReturn = hookedDXGIPresent1(pSwapChain, SyncInterval, PresentFlags, pPresentParameters);
		// if we have to grab the frame, do it now.
		ScreenshotController& screenshotController = Globals::instance().getScreenshotController();
		grabShotIfRequired(pSwapChain, screenshotController, validFrameToGrab);
		screenshotController.presentCalled();
		_presentInProgress = false;
		return toReturn;
	}


	void determineDirect3DObjects(IDXGISwapChain* pSwapChain)
	{
		if(!_initializeDeviceAndContext)
		{
			return;
		}
		
		// First dx12, then dx11
		if (FAILED(pSwapChain->GetDevice(__uuidof(_deviceDX12), (void**)&_deviceDX12)))
		{
			_direct3D12Available = false;
			// check dx11
			if (FAILED(pSwapChain->GetDevice(__uuidof(_deviceDX11), (void**)&_deviceDX11)))
			{
				MessageHandler::logError("Can't init Direct3D 11/12 devices.");
				_direct3D11Available = false;
				return;
			}
			MessageHandler::logDebug("DX11 Device: %p", (void*)_deviceDX11);
			_deviceDX11->GetImmediateContext(&_contextDX11);
			if (nullptr == _contextDX11)
			{
				MessageHandler::logError("Failed to get Direct3D 11 device context from hooked swapchain.");
				_direct3D11Available = false;
				return;
			}
			MessageHandler::logDebug("DX11 Context: %p", (void*)_contextDX11);
			_direct3D11Available = true;
		}
		else
		{
			// dx12
			D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
			commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			commandQueueDesc.NodeMask = 0;
			if (FAILED(_deviceDX12->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&_commandQueueDX12))))
			{
				MessageHandler::logError("Failed to create D3D12CommandQueue");
				_direct3D12Available = false;
			}
			else
			{
				_direct3D12Available = true;
			}
		}
	}

	
	// Performs the handling of Present/Present1 core functionality. 
	// Returns true if the frame is a valid frame to grab, false otherwise. 
	bool handlePresentCore(IDXGISwapChain* pSwapChain, UINT Flags)
	{
		_presentInProgress = true;
		bool validFrameToGrab = false;
		if (!(Flags & DXGI_PRESENT_TEST))
		{
			if (_initializeDeviceAndContext)
			{
				determineDirect3DObjects(pSwapChain);

				_initializeDeviceAndContext = false;
			}
			validFrameToGrab = (_direct3D11Available || _direct3D12Available);
			Input::resetKeyStates();
			Input::resetMouseState();
		}
		return validFrameToGrab;
	}


	void hookDXGIPresent1(IDXGISwapChain* pTmpSwapChain)
	{
		IDXGISwapChain1* pTmpSwapChain1 = nullptr;
		if (SUCCEEDED(pTmpSwapChain->QueryInterface(__uuidof(IDXGISwapChain1), (void**)&pTmpSwapChain1)))
		{
			// SwapChain1 is available, hook present1 as well. 
			__int64* pSwapChain1Vtable = nullptr;
			pSwapChain1Vtable = (__int64*)pTmpSwapChain1;
			pSwapChain1Vtable = (__int64*)pSwapChain1Vtable[0];
			MessageHandler::logDebug("Present1 Address: %p", (void*)(__int64*)pSwapChain1Vtable[DXGI_PRESENT1_INDEX]);
			if (MH_CreateHook((LPBYTE)pSwapChain1Vtable[DXGI_PRESENT1_INDEX], &detourDXGIPresent1, reinterpret_cast<LPVOID*>(&hookedDXGIPresent1)) != MH_OK)
			{
				MessageHandler::logDebug("Hooking Present1 failed!");
			}
			if (MH_EnableHook((LPBYTE)pSwapChain1Vtable[DXGI_PRESENT1_INDEX]) != MH_OK)
			{
				MessageHandler::logDebug("Enabling of Present1 hook failed!");
			}
			if (nullptr != pTmpSwapChain1)
			{
				pTmpSwapChain1->Release();
			}
		}
	}


	// Default msg pump for the tmp window we're creating in initializeDXGIHookUsingDX12()
	LRESULT WINAPI WndProcForDX12TmpWindow(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	
	// Tries to hook DXGI using a DX12 swapchain on a fake window. 
	bool initializeDXGIHookUsingDX12()
	{
		// It's about creating a fake swapchain to hook DXGI. We can't use another method, as DXGI might be reshade so we can't
		// an offset nor an AOB scan for present(1). We also can't use a fake window with a fake swapchain using dx11 in all cases, as that will fail:
		// using DX11 in a separate window might cause overlays like the one from NVidia to inject themselves using DX11 into that window. This causes a problem
		// as they'll crash as they main game window uses DX12. We therefore have to use DX12 in a separate window to obtain the DXGI swapchain.
		if(nullptr!=hookedDXGIPresent)
		{
			// already hooked.
			return true;
		}
		
		MessageHandler::logDebug("Hooking DXGI using DX12...");
		// If we're not on Windows 10, always return false as DX12 isn't available on earlier versions. 
		if(!IsWindows10OrGreater())
		{
			MessageHandler::logDebug("Not on Windows 10...");
			return false;
		}
		 
		// Create tmp window. We initialize the swapchain on this window. As we own this swapchain we won't run into access denied issues if the swapchain is
		// exclusive to the window of the game. 
		WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProcForDX12TmpWindow, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, _T("IGCS TmpWindow"), nullptr };
		::RegisterClassEx(&wc);
		HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("IGCS TmpWindow"), WS_OVERLAPPEDWINDOW, 100, 100, 100, 100, NULL, NULL, wc.hInstance, NULL);

		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

		IDXGIFactory4* pTmpDXGIFactory;
		if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&pTmpDXGIFactory))))
		{
			// Failed
			return false;
		}

		IDXGIAdapter* pTmpAdapter;
		pTmpDXGIFactory->EnumAdapters(0, &pTmpAdapter);

		// create the d3d12 device
		ID3D12Device* pTmpDevice;
		if (FAILED(D3D12CreateDevice(pTmpAdapter, featureLevel, IID_PPV_ARGS(&pTmpDevice))))
		{
			// Failed
			return false;
		}

		// create the command queue from the device
		ID3D12CommandQueue* pTmpCommandQueue;
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
		commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		if (FAILED(pTmpDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&pTmpCommandQueue))))
		{
			// Failed
			return false;
		}

		// THEN create the swapchain, passing the commandqueue
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.BufferCount = 2;
		swapChainDesc.BufferDesc.Width = 0;
		swapChainDesc.BufferDesc.Height = 0;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		IDXGISwapChain* pTmpSwapChain;
		if (FAILED(pTmpDXGIFactory->CreateSwapChain(pTmpCommandQueue, &swapChainDesc, &pTmpSwapChain)))
		{
			// Failed
			return false;
		}

		__int64* pSwapChainVtable = nullptr;
		pSwapChainVtable = (__int64*)pTmpSwapChain;
		pSwapChainVtable = (__int64*)pSwapChainVtable[0];

		MessageHandler::logDebug("Present Address: %p", (void*)(__int64*)pSwapChainVtable[DXGI_PRESENT_INDEX]);

		bool hooksFailed= false;
		if (MH_CreateHook((LPBYTE)pSwapChainVtable[DXGI_PRESENT_INDEX], &detourDXGIPresent, reinterpret_cast<LPVOID*>(&hookedDXGIPresent)) != MH_OK)
		{
			// Failed
			hooksFailed=false;
		}
		if (MH_EnableHook((LPBYTE)pSwapChainVtable[DXGI_PRESENT_INDEX]) != MH_OK)
		{
			// Failed
			hooksFailed=false;
		}
		hookDXGIPresent1(pTmpSwapChain);

		MessageHandler::logDebug("ResizeBuffers Address: %p", (__int64*)pSwapChainVtable[DXGI_RESIZEBUFFERS_INDEX]);

		if (MH_CreateHook((LPBYTE)pSwapChainVtable[DXGI_RESIZEBUFFERS_INDEX], &detourDXGIResizeBuffers, reinterpret_cast<LPVOID*>(&hookedDXGIResizeBuffers)) != MH_OK)
		{
			// Failed
			hooksFailed=false;
		}
		if (MH_EnableHook((LPBYTE)pSwapChainVtable[DXGI_RESIZEBUFFERS_INDEX]) != MH_OK)
		{
			// Failed
			hooksFailed=false;
		}

		pTmpCommandQueue->Release();
		pTmpDevice->Release();
		pTmpSwapChain->Release();

		CloseWindow(hwnd);

		if(hooksFailed)
		{
			MessageHandler::logDebug("One or more hooks failed when hooking DXGI using DX12.");
			return false;
		}
		
		MessageHandler::logDebug("DXGI hooks set (using DX12)");
		return true;
	}
	

	// Tries to hook DXGI using a DX11 swapchain on the active window. 
	bool initializeDXGIHookUsingDX11()
	{
		if (nullptr != hookedDXGIPresent)
		{
			// already hooked.
			return true;
		}
		
		MessageHandler::logDebug("Hooking DXGI using DX11...");
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		HWND windowHandle = Globals::instance().mainWindowHandle();
		MessageHandler::logDebug("HWND of window used for hooking DX11: 0x%x", windowHandle);
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = windowHandle;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		ID3D11Device *pTmpDevice = nullptr;
		ID3D11DeviceContext *pTmpContext = nullptr;
		IDXGISwapChain* pTmpSwapChain;
		if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &pTmpSwapChain, &pTmpDevice, NULL, &pTmpContext)))
		{
			MessageHandler::logError("Failed to create directX device and swapchain!");
			return false;
		}

		__int64* pSwapChainVtable = nullptr;
		pSwapChainVtable = (__int64*)pTmpSwapChain;
		pSwapChainVtable = (__int64*)pSwapChainVtable[0];

		MessageHandler::logDebug("Present Address: %p", (void*)(__int64*)pSwapChainVtable[DXGI_PRESENT_INDEX]);

		if (MH_CreateHook((LPBYTE)pSwapChainVtable[DXGI_PRESENT_INDEX], &detourDXGIPresent, reinterpret_cast<LPVOID*>(&hookedDXGIPresent)) != MH_OK)
		{
			MessageHandler::logError("Hooking Present failed!");
			return false;
		}
		if (MH_EnableHook((LPBYTE)pSwapChainVtable[DXGI_PRESENT_INDEX]) != MH_OK)
		{
			MessageHandler::logError("Enabling of Present hook failed!");
			return false;
		}
		hookDXGIPresent1(pTmpSwapChain);

		MessageHandler::logDebug("ResizeBuffers Address: %p", (__int64*)pSwapChainVtable[DXGI_RESIZEBUFFERS_INDEX]);

		if (MH_CreateHook((LPBYTE)pSwapChainVtable[DXGI_RESIZEBUFFERS_INDEX], &detourDXGIResizeBuffers, reinterpret_cast<LPVOID*>(&hookedDXGIResizeBuffers)) != MH_OK)
		{
			MessageHandler::logError("Hooking ResizeBuffers failed!");
			return false;
		}
		if (MH_EnableHook((LPBYTE)pSwapChainVtable[DXGI_RESIZEBUFFERS_INDEX]) != MH_OK)
		{
			MessageHandler::logError("Enabling of ResizeBuffers hook failed!");
			return false;
		}

		pTmpDevice->Release();
		pTmpContext->Release();
		pTmpSwapChain->Release();

		MessageHandler::logDebug("DXGI hooks set (Using DX11)");
		_direct3D11Available = true;
		return true;
	}


	void copyBackBufferBytes(uint32_t height, uint32_t pitch, uint32_t rowPitch, uint8_t* source, uint8_t* destination, DXGI_FORMAT format)
	{
		// From Reshade.
		for (uint32_t y = 0; y < height; y++)
		{
			if (format == DXGI_FORMAT_R10G10B10A2_UNORM || format == DXGI_FORMAT_R10G10B10A2_UINT)
			{
				for (uint32_t x = 0; x < pitch; x += 4)
				{
					const uint32_t rgba = *reinterpret_cast<const uint32_t*>(source + x);
					// Divide by 4 to get 10-bit range (0-1023) into 8-bit range (0-255)
					destination[x + 0] = ((rgba & 0x3FF) / 4) & 0xFF;
					destination[x + 1] = (((rgba & 0xFFC00) >> 10) / 4) & 0xFF;
					destination[x + 2] = (((rgba & 0x3FF00000) >> 20) / 4) & 0xFF;
					destination[x + 3] = 0xFF;
				}
			}
			else
			{
				memcpy(destination, source, pitch);

				for (uint32_t x = 0; x < pitch; x += 4)
				{
					destination[x + 3] = 0xFF;

					if (format == DXGI_FORMAT_B8G8R8A8_UNORM || format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB ||
						format == DXGI_FORMAT_B8G8R8X8_UNORM || format == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB ||
						format == DXGI_FORMAT_B8G8R8A8_TYPELESS || format == DXGI_FORMAT_B8G8R8X8_TYPELESS)
					{
						std::swap(destination[x + 0], destination[x + 2]);
					}
				}
			}
			destination += pitch;
			source += rowPitch;
		}
	}
	

	std::vector<uint8_t> capture_frame_DX11(IDXGISwapChain* pSwapChain)
	{
		MessageHandler::logDebug("capture_frame_DX11()");

		std::vector<uint8_t> failed;
		D3D11_TEXTURE2D_DESC StagingDesc;
		ID3D11Texture2D* pBackBuffer = nullptr;
		if(FAILED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)))
		{
			MessageHandler::logError("Failed to grab back buffer for Direct3D11 screenshot capture");
			return failed;
		}
		pBackBuffer->GetDesc(&StagingDesc);

		StagingDesc.Usage = D3D11_USAGE_STAGING;
		StagingDesc.BindFlags = 0;
		StagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		CComPtr<ID3D11Texture2D> pBackBufferStaging = nullptr;
		if(FAILED(_deviceDX11->CreateTexture2D(&StagingDesc, nullptr, &pBackBufferStaging)))
		{
			MessageHandler::logError("Failed to create system memory texture for Direct3D11 screenshot capture");
			return failed;
		}
		Globals::instance().getScreenshotController().setBufferSize(StagingDesc.Width, StagingDesc.Height);

		_contextDX11->CopyResource(pBackBufferStaging, pBackBuffer);
		
		D3D11_MAPPED_SUBRESOURCE mapped;
		if (FAILED(_contextDX11->Map(pBackBufferStaging, 0, D3D11_MAP_READ, 0, &mapped)))
		{
			MessageHandler::logError("Failed to map staging resource with screenshot capture!");
			return failed;
		}
		std::vector<uint8_t> fbdata(StagingDesc.Width * StagingDesc.Height * 4);
		uint8_t* buffer = fbdata.data();
		auto mapped_data = static_cast<uint8_t*>(mapped.pData);
		const uint32_t pitch = StagingDesc.Width * 4;
		copyBackBufferBytes(StagingDesc.Height, pitch, mapped.RowPitch, mapped_data, buffer, StagingDesc.Format);
		
		_contextDX11->Unmap(pBackBufferStaging, 0);
		pBackBuffer->Release();
		return fbdata;
	}


	void setTransitionState(ID3D12Resource* resource, D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to, ID3D12GraphicsCommandList* commandList)
	{
		D3D12_RESOURCE_BARRIER transition = { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION };
		transition.Transition.pResource = resource;
		transition.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		transition.Transition.StateBefore = from;
		transition.Transition.StateAfter = to;
		commandList->ResourceBarrier(1, &transition);
	}
	

	std::vector<uint8_t> capture_frame_DX12(IDXGISwapChain* pSwapChain)
	{
		MessageHandler::logDebug("capture_frame_DX12()");

		// Get buffer index
		CComQIPtr<IDXGISwapChain3> pSwapChain3 = pSwapChain;
		const UINT backBufferIdx = pSwapChain3->GetCurrentBackBufferIndex();
		
		// get swapchain desc.
		CComQIPtr<IDXGISwapChain1> pSwapChain1 = pSwapChain;
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
		pSwapChain1->GetDesc1(&swapChainDesc);
		Globals::instance().getScreenshotController().setBufferSize(swapChainDesc.Width, swapChainDesc.Height);
		std::vector<uint8_t> failed;
		
		// From Reshade & DirectXTK12/ScreenGrab
		const uint32_t pitch = swapChainDesc.Width * 4;
		const uint32_t rowPitch = (pitch + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);

		D3D12_RESOURCE_DESC desc = { D3D12_RESOURCE_DIMENSION_BUFFER };
		desc.Width = swapChainDesc.Height * rowPitch;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.SampleDesc = { 1, 0 };
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		D3D12_HEAP_PROPERTIES props = { D3D12_HEAP_TYPE_READBACK };

		CComPtr<ID3D12Resource> intermediate;
		if (FAILED(_deviceDX12->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&intermediate))))
		{
			MessageHandler::logError("Failed to create system memory texture for Direct3D12 screenshot capture");
			return failed;
		}

		// Create a command allocator
		CComPtr<ID3D12CommandAllocator> commandAlloc;
		if (FAILED(_deviceDX12->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAlloc))))
		{
			MessageHandler::logError("Failed to create a command allocator for Direct3D12 screenshot capture");
			return failed;
		}

		// Spin up a new command list
		CComPtr<ID3D12GraphicsCommandList> commandList;
		if (FAILED(_deviceDX12->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAlloc, nullptr, IID_PPV_ARGS(&commandList))))
		{
			MessageHandler::logError("Failed to create a command list for Direct3D12 screenshot capture");
			return failed;
		}

		// Create a fence
		CComPtr<ID3D12Fence> fence;
		if (FAILED(_deviceDX12->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
		{
			MessageHandler::logError("Failed to create a fence for Direct3D12 screenshot capture");
			return failed;
		}

		CComPtr<ID3D12Resource> pBackBuffer = nullptr;
		pSwapChain->GetBuffer(backBufferIdx, __uuidof(ID3D12Resource), (LPVOID*)&pBackBuffer);
		
		setTransitionState(pBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_SOURCE, commandList);
		// setup copy heaps
		D3D12_TEXTURE_COPY_LOCATION srcLocation = { pBackBuffer };
		srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		srcLocation.SubresourceIndex = 0;

		D3D12_TEXTURE_COPY_LOCATION dstLocation = { intermediate };
		dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		dstLocation.PlacedFootprint.Footprint.Width = swapChainDesc.Width;
		dstLocation.PlacedFootprint.Footprint.Height = swapChainDesc.Height;
		dstLocation.PlacedFootprint.Footprint.Depth = 1;
		dstLocation.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		dstLocation.PlacedFootprint.Footprint.RowPitch = rowPitch;

		commandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);
		setTransitionState(pBackBuffer, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PRESENT, commandList);

		// execute the command list
		commandList->Close();
		ID3D12CommandList* const listsToExecute[] = { commandList };
		_commandQueueDX12->ExecuteCommandLists(1, listsToExecute);

		// signal and wait for the fence
		_commandQueueDX12->Signal(fence, 1);
		while(fence->GetCompletedValue()<1)
		{
			SwitchToThread();
		}
		
		// copy the data
		uint8_t* mappedData;
		if(FAILED(intermediate->Map(0, nullptr, reinterpret_cast<void**>(&mappedData))))
		{
			MessageHandler::logError("Failed to map staging resource with screenshot capture!");
			return failed;
		}
		std::vector<uint8_t> fbdata(swapChainDesc.Height * pitch);
		uint8_t* buffer = fbdata.data();
		copyBackBufferBytes(swapChainDesc.Height, pitch, rowPitch, mappedData, buffer, swapChainDesc.Format);
		intermediate->Unmap(0, nullptr);
		return fbdata;
	}
}