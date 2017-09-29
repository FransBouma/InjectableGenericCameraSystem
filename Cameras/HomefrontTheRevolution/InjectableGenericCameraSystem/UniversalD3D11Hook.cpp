#include "stdafx.h"
#include <d3d11.h> 
#include "UniversalD3D11Hook.h"
#include "Console.h"
#include "MinHook.h"
#include "Globals.h"
#include "imgui.h"
#include "Defaults.h"
#include "imgui_impl_dx11.h"
#include "OverlayControl.h"
#include "OverlayConsole.h"

#pragma comment(lib, "d3d11.lib")

namespace IGCS::DX11Hooker
{
	#define DXGI_PRESENT_INDEX				8
	#define DXGI_RESIZEBUFFERS_INDEX		13
	#define DX11_CREATE_DEFERRED_CONTEXT	27
	#define DX11_PS_SET_SHADER				9

	//--------------------------------------------------------------------------------------------------------------------------------
	// Forward declarations
	void createRenderTarget(IDXGISwapChain* pSwapChain);
	void cleanupRenderTarget();
	void initImGui();
	void initImGuiStyle();
	void hookDeviceMethods();
	void hookContextMethods();

	//--------------------------------------------------------------------------------------------------------------------------------
	// Typedefs of functions to hook
	typedef HRESULT(__stdcall *D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	typedef HRESULT(__stdcall *D3D11ResizeBuffersHook) (IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
	typedef HRESULT(__stdcall *D3D11CreateDeferredContextHook) (ID3D11Device* pDevice, UINT contextFlags, ID3D11DeviceContext** ppDeferredContext);
	typedef HRESULT(__stdcall *D3D11PSSetShaderHook) (ID3D11DeviceContext* pDeviceContext, ID3D11PixelShader* pPixelShader, ID3D11ClassInstance *const *ppClassInstances, UINT numClassInstances);

	static ID3D11Device* _device = nullptr;
	static ID3D11DeviceContext* _context = nullptr;
	static ID3D11RenderTargetView* _mainRenderTargetView = nullptr;

	//--------------------------------------------------------------------------------------------------------------------------------
	// Pointers to the original hooked functions
	static D3D11PresentHook hookedD3D11Present = nullptr;
	static D3D11ResizeBuffersHook hookedD3D11ResizeBuffers = nullptr;
	static D3D11CreateDeferredContextHook hookedD3D11CreateDeferredContext = nullptr;
	static D3D11PSSetShaderHook hookedD3D11PSSetShader = nullptr;

	static bool _tmpSwapChainInitialized = false;
	static bool _showWindow = true;
	static bool _imGuiInitializing = false;

	//--------------------------------------------------------------------------------------------------------------------------------
	// Detour methods which are called by the hooked vtable entrances
	HRESULT __stdcall detourD3D11CreateDeferredContext(ID3D11Device* pDevice, UINT contextFlags, ID3D11DeviceContext** ppDeferredContext)
	{
		return hookedD3D11CreateDeferredContext(pDevice, contextFlags, ppDeferredContext);
	}


	HRESULT __stdcall detourD3D11PSSetShader(ID3D11DeviceContext* pDeviceContext, ID3D11PixelShader* pPixelShader, ID3D11ClassInstance *const *ppClassInstances, UINT numClassInstances)
	{
		return hookedD3D11PSSetShader(pDeviceContext, pPixelShader, ppClassInstances, numClassInstances);
	}


	HRESULT __stdcall detourD3D11ResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags)
	{
		_imGuiInitializing = true;
		ImGui_ImplDX11_InvalidateDeviceObjects();
		cleanupRenderTarget();
		HRESULT toReturn = hookedD3D11ResizeBuffers(pSwapChain, bufferCount, width, height, newFormat, swapChainFlags);
		createRenderTarget(pSwapChain);
		ImGui_ImplDX11_CreateDeviceObjects();
		_imGuiInitializing = false;
		return toReturn;
	}


	HRESULT __stdcall detourD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		static bool _initializeDeviceAndContext = true;

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
					initImGui();

					// Uncomment lines below to hook device/context for shader interception
					//hookDeviceMethods();
					//hookContextMethods();

					_initializeDeviceAndContext = false;
				}

				// render our own stuff
				_context->OMSetRenderTargets(1, &_mainRenderTargetView, NULL);
				OverlayControl::renderOverlay();
				ImGui_ImpDX11_ResetKeyStates();
			}
		}
		return hookedD3D11Present(pSwapChain, SyncInterval, Flags);
	}

	//--------------------------------------------------------------------------------------------------------------------------------
	// Hook setting code
	void initializeHook()
	{
		HWND hWnd = IGCS::Globals::instance().mainWindowHandle();
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

		ID3D11Device *pTmpDevice = NULL;
		ID3D11DeviceContext *pTmpContext = NULL;
		IDXGISwapChain* pTmpSwapChain;
		if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1
			, D3D11_SDK_VERSION, &swapChainDesc, &pTmpSwapChain, &pTmpDevice, NULL, &pTmpContext)))
		{
			IGCS::Console::WriteError("Failed to create directX device and swapchain!");
			return;
		}

		__int64* pSwapChainVTable = NULL;
		__int64* pDeviceContextVTable = NULL;
		pSwapChainVTable = (__int64*)pTmpSwapChain;
		pSwapChainVTable = (__int64*)pSwapChainVTable[0];
		pDeviceContextVTable = (__int64*)pTmpContext;
		pDeviceContextVTable = (__int64*)pDeviceContextVTable[0];

		OverlayConsole::instance().logDebug("Present Address: %p", (void*)(__int64*)pSwapChainVTable[DXGI_PRESENT_INDEX]);

		if (MH_CreateHook((LPBYTE)pSwapChainVTable[DXGI_PRESENT_INDEX], &detourD3D11Present, reinterpret_cast<LPVOID*>(&hookedD3D11Present)) != MH_OK)
		{
			IGCS::Console::WriteError("Hooking Present failed!");
		}
		if (MH_EnableHook((LPBYTE)pSwapChainVTable[DXGI_PRESENT_INDEX]) != MH_OK)
		{
			IGCS::Console::WriteError("Enabling of Present hook failed!");
		}

		OverlayConsole::instance().logDebug("ResizeBuffers Address: %p", (__int64*)pSwapChainVTable[DXGI_RESIZEBUFFERS_INDEX]);

		if (MH_CreateHook((LPBYTE)pSwapChainVTable[DXGI_RESIZEBUFFERS_INDEX], &detourD3D11ResizeBuffers, reinterpret_cast<LPVOID*>(&hookedD3D11ResizeBuffers)) != MH_OK)
		{
			IGCS::Console::WriteError("Hooking ResizeBuffers failed!");
		}
		if (MH_EnableHook((LPBYTE)pSwapChainVTable[DXGI_RESIZEBUFFERS_INDEX]) != MH_OK)
		{
			IGCS::Console::WriteError("Enabling of ResizeBuffers hook failed!");
		}

		pTmpDevice->Release();
		pTmpContext->Release();
		pTmpSwapChain->Release();
		_tmpSwapChainInitialized = true;

		OverlayConsole::instance().logDebug("DX11 hooks set");
	}


	void hookDeviceMethods()
	{
		if (nullptr == _device)
		{
			IGCS::Console::WriteError("Can't hook device as it's NULL!");
			return;
		}

		__int64* pDeviceVTable = (__int64*)_device;
		pDeviceVTable = (__int64*)pDeviceVTable[0];

		OverlayConsole::instance().logDebug("CreateDeferredContext Address: %p", (__int64*)pDeviceVTable[DX11_CREATE_DEFERRED_CONTEXT]);

		if (MH_CreateHook((LPBYTE)pDeviceVTable[DX11_CREATE_DEFERRED_CONTEXT], &detourD3D11CreateDeferredContext, reinterpret_cast<LPVOID*>(&hookedD3D11CreateDeferredContext)) != MH_OK)
		{
			IGCS::Console::WriteError("Hooking CreateDeferredContext failed!");
		}
		if (MH_EnableHook((LPBYTE)pDeviceVTable[DX11_CREATE_DEFERRED_CONTEXT]) != MH_OK)
		{
			IGCS::Console::WriteError("Enabling of CreateDeferredContext hook failed!");
		}
	}


	void hookContextMethods()
	{
		if (nullptr == _context)
		{
			IGCS::Console::WriteError("Can't hook context as it's NULL!");
			return;
		}

		__int64* pDeviceContextVTable = (__int64*)_context;
		pDeviceContextVTable = (__int64*)pDeviceContextVTable[0];

		OverlayConsole::instance().logDebug("PSSetShader Address: %p", (__int64*)pDeviceContextVTable[DX11_PS_SET_SHADER]);

		if (MH_CreateHook((LPBYTE)pDeviceContextVTable[DX11_PS_SET_SHADER], &detourD3D11PSSetShader, reinterpret_cast<LPVOID*>(&hookedD3D11PSSetShader)) != MH_OK)
		{
			IGCS::Console::WriteError("Hooking PSSetShader failed!");
		}
		if (MH_EnableHook((LPBYTE)pDeviceContextVTable[DX11_PS_SET_SHADER]) != MH_OK)
		{
			IGCS::Console::WriteError("Enabling of PSSetShader hook failed!");
		}

	}


	void createRenderTarget(IDXGISwapChain* pSwapChain)
	{
		DXGI_SWAP_CHAIN_DESC sd;
		pSwapChain->GetDesc(&sd);
		ID3D11Texture2D* pBackBuffer;
		D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
		ZeroMemory(&render_target_view_desc, sizeof(render_target_view_desc));
		render_target_view_desc.Format = sd.BufferDesc.Format;
		render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		_device->CreateRenderTargetView(pBackBuffer, &render_target_view_desc, &_mainRenderTargetView);
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


	void initImGui()
	{
		ImGui_ImplDX11_Init(IGCS::Globals::instance().mainWindowHandle(), _device, _context);
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = IGCS_OVERLAY_INI_FILENAME;
		initImGuiStyle();
	}


	void initImGuiStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		style.WindowRounding = 2.0f;
		style.FrameRounding = 1.0f;
		style.IndentSpacing = 25.0f;
		style.ScrollbarSize = 16.0f;
		style.ScrollbarRounding = 1.0f;

		style.Colors[ImGuiCol_Text] = ImVec4(0.84f, 0.84f, 0.88f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.24f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 0.90f);
		style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.22f, 0.22f, 0.24f, 0.31f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.24f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.35f, 0.38f, 1.00f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.27f, 0.27f, 0.33f, 0.37f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.37f, 0.37f, 0.42f, 0.42f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.45f, 0.45f, 0.45f, 0.30f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
		style.Colors[ImGuiCol_ComboBg] = ImVec4(0.13f, 0.13f, 0.16f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.53f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.65f, 0.31f, 0.00f, 0.71f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.65f, 0.31f, 0.00f, 0.86f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.80f, 0.41f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.50f, 0.50f, 0.53f, 0.49f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.47f, 0.47f, 0.49f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.40f, 0.40f, 0.44f, 0.31f);
		style.Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
		style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.44f, 0.44f, 0.44f, 0.30f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
		style.Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.40f, 0.40f, 0.44f);
		style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.93f);
		style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
	}
}


/* 
// from d3d11.h. Simply walk the inheritance. In C++ the order of methods in a .h file is the order in the vtable, after the vtable of inherited
// types, so it's easy to determine what the location is without silly loggers. 

// IUnknown
0	virtual HRESULT STDMETHODCALLTYPE QueryInterface
1	virtual ULONG STDMETHODCALLTYPE AddRef
2	virtual ULONG STDMETHODCALLTYPE Release

// ID3D11Device
3	virtual HRESULT STDMETHODCALLTYPE CreateBuffer
4	virtual HRESULT STDMETHODCALLTYPE CreateTexture1D
5	virtual HRESULT STDMETHODCALLTYPE CreateTexture2D
6	virtual HRESULT STDMETHODCALLTYPE CreateTexture3D
7	virtual HRESULT STDMETHODCALLTYPE CreateShaderResourceView
8	virtual HRESULT STDMETHODCALLTYPE CreateUnorderedAccessView
9	virtual HRESULT STDMETHODCALLTYPE CreateRenderTargetView
10	virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilView
11	virtual HRESULT STDMETHODCALLTYPE CreateInputLayout
12	virtual HRESULT STDMETHODCALLTYPE CreateVertexShader
13	virtual HRESULT STDMETHODCALLTYPE CreateGeometryShader
14	virtual HRESULT STDMETHODCALLTYPE CreateGeometryShaderWithStreamOutput
15	virtual HRESULT STDMETHODCALLTYPE CreatePixelShader
16	virtual HRESULT STDMETHODCALLTYPE CreateHullShader
17	virtual HRESULT STDMETHODCALLTYPE CreateDomainShader
18	virtual HRESULT STDMETHODCALLTYPE CreateComputeShader
19	virtual HRESULT STDMETHODCALLTYPE CreateClassLinkage
20	virtual HRESULT STDMETHODCALLTYPE CreateBlendState
21	virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilState
22	virtual HRESULT STDMETHODCALLTYPE CreateRasterizerState
23	virtual HRESULT STDMETHODCALLTYPE CreateSamplerState
24	virtual HRESULT STDMETHODCALLTYPE CreateQuery
25	virtual HRESULT STDMETHODCALLTYPE CreatePredicate
26	virtual HRESULT STDMETHODCALLTYPE CreateCounter
27	virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext
28	virtual HRESULT STDMETHODCALLTYPE OpenSharedResource
29	virtual HRESULT STDMETHODCALLTYPE CheckFormatSupport
30	virtual HRESULT STDMETHODCALLTYPE CheckMultisampleQualityLevels
31	virtual void STDMETHODCALLTYPE CheckCounterInfo
32	virtual HRESULT STDMETHODCALLTYPE CheckCounter
33	virtual HRESULT STDMETHODCALLTYPE CheckFeatureSupport
34	virtual HRESULT STDMETHODCALLTYPE GetPrivateData
35	virtual HRESULT STDMETHODCALLTYPE SetPrivateData
36	virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface
37	virtual D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetFeatureLevel
38	virtual UINT STDMETHODCALLTYPE GetCreationFlags
39	virtual HRESULT STDMETHODCALLTYPE GetDeviceRemovedReason
40	virtual void STDMETHODCALLTYPE GetImmediateContext
41	virtual HRESULT STDMETHODCALLTYPE SetExceptionMode
42	virtual UINT STDMETHODCALLTYPE GetExceptionMode

---------------------------------------------------------------------------
// IUnknown
0	virtual HRESULT STDMETHODCALLTYPE QueryInterface
1	virtual ULONG STDMETHODCALLTYPE AddRef
2	virtual ULONG STDMETHODCALLTYPE Release

// ID3D11DeviceChild
3	virtual void STDMETHODCALLTYPE GetDevice
4	virtual HRESULT STDMETHODCALLTYPE GetPrivateData
5	virtual HRESULT STDMETHODCALLTYPE SetPrivateData
6	virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface

// ID3D11DeviceContext
7	virtual void STDMETHODCALLTYPE VSSetConstantBuffers
8	virtual void STDMETHODCALLTYPE PSSetShaderResources
9	virtual void STDMETHODCALLTYPE PSSetShader
10	virtual void STDMETHODCALLTYPE PSSetSamplers
11	virtual void STDMETHODCALLTYPE VSSetShader
12	virtual void STDMETHODCALLTYPE DrawIndexed
13	virtual void STDMETHODCALLTYPE Draw
14	virtual HRESULT STDMETHODCALLTYPE Map
15	virtual void STDMETHODCALLTYPE Unmap
16	virtual void STDMETHODCALLTYPE PSSetConstantBuffers
17	virtual void STDMETHODCALLTYPE IASetInputLayout
18	virtual void STDMETHODCALLTYPE IASetVertexBuffers
19	virtual void STDMETHODCALLTYPE IASetIndexBuffer
20	virtual void STDMETHODCALLTYPE DrawIndexedInstanced
21	virtual void STDMETHODCALLTYPE DrawInstanced
22	virtual void STDMETHODCALLTYPE GSSetConstantBuffers
23	virtual void STDMETHODCALLTYPE GSSetShader
24	virtual void STDMETHODCALLTYPE IASetPrimitiveTopology
25	virtual void STDMETHODCALLTYPE VSSetShaderResources
26	virtual void STDMETHODCALLTYPE VSSetSamplers
27	virtual void STDMETHODCALLTYPE Begin
28	virtual void STDMETHODCALLTYPE End
29	virtual HRESULT STDMETHODCALLTYPE GetData
30	virtual void STDMETHODCALLTYPE SetPredication
31	virtual void STDMETHODCALLTYPE GSSetShaderResources
32	virtual void STDMETHODCALLTYPE GSSetSamplers
33	virtual void STDMETHODCALLTYPE OMSetRenderTargets
34	virtual void STDMETHODCALLTYPE OMSetRenderTargetsAndUnorderedAccessViews
35	virtual void STDMETHODCALLTYPE OMSetBlendState
36	virtual void STDMETHODCALLTYPE OMSetDepthStencilState
37	virtual void STDMETHODCALLTYPE SOSetTargets
38	virtual void STDMETHODCALLTYPE DrawAuto
39	virtual void STDMETHODCALLTYPE DrawIndexedInstancedIndirect
40	virtual void STDMETHODCALLTYPE DrawInstancedIndirect
41	virtual void STDMETHODCALLTYPE Dispatch
42	virtual void STDMETHODCALLTYPE DispatchIndirect
43	virtual void STDMETHODCALLTYPE RSSetState
44	virtual void STDMETHODCALLTYPE RSSetViewports
45	virtual void STDMETHODCALLTYPE RSSetScissorRects
46	virtual void STDMETHODCALLTYPE CopySubresourceRegion
47	virtual void STDMETHODCALLTYPE CopyResource
48	virtual void STDMETHODCALLTYPE UpdateSubresource
49	virtual void STDMETHODCALLTYPE CopyStructureCount
50	virtual void STDMETHODCALLTYPE ClearRenderTargetView
51	virtual void STDMETHODCALLTYPE ClearUnorderedAccessViewUint
52	virtual void STDMETHODCALLTYPE ClearUnorderedAccessViewFloat
53	virtual void STDMETHODCALLTYPE ClearDepthStencilView
54	virtual void STDMETHODCALLTYPE GenerateMips
55	virtual void STDMETHODCALLTYPE SetResourceMinLOD
56	virtual FLOAT STDMETHODCALLTYPE GetResourceMinLOD
57	virtual void STDMETHODCALLTYPE ResolveSubresource
58	virtual void STDMETHODCALLTYPE ExecuteCommandList
59	virtual void STDMETHODCALLTYPE HSSetShaderResources
60	virtual void STDMETHODCALLTYPE HSSetShader
61	virtual void STDMETHODCALLTYPE HSSetSamplers
62	virtual void STDMETHODCALLTYPE HSSetConstantBuffers
63	virtual void STDMETHODCALLTYPE DSSetShaderResources
64	virtual void STDMETHODCALLTYPE DSSetShader
65	virtual void STDMETHODCALLTYPE DSSetSamplers
66	virtual void STDMETHODCALLTYPE DSSetConstantBuffers
67	virtual void STDMETHODCALLTYPE CSSetShaderResources
68	virtual void STDMETHODCALLTYPE CSSetUnorderedAccessViews
69	virtual void STDMETHODCALLTYPE CSSetShader
70	virtual void STDMETHODCALLTYPE CSSetSamplers
71	virtual void STDMETHODCALLTYPE CSSetConstantBuffers
72	virtual void STDMETHODCALLTYPE VSGetConstantBuffers
73	virtual void STDMETHODCALLTYPE PSGetShaderResources
74	virtual void STDMETHODCALLTYPE PSGetShader
75	virtual void STDMETHODCALLTYPE PSGetSamplers
76	virtual void STDMETHODCALLTYPE VSGetShader
77	virtual void STDMETHODCALLTYPE PSGetConstantBuffers
78	virtual void STDMETHODCALLTYPE IAGetInputLayout
79	virtual void STDMETHODCALLTYPE IAGetVertexBuffers
80	virtual void STDMETHODCALLTYPE IAGetIndexBuffer
81	virtual void STDMETHODCALLTYPE GSGetConstantBuffers
82	virtual void STDMETHODCALLTYPE GSGetShader
83	virtual void STDMETHODCALLTYPE IAGetPrimitiveTopology
84	virtual void STDMETHODCALLTYPE VSGetShaderResources
85	virtual void STDMETHODCALLTYPE VSGetSamplers
86	virtual void STDMETHODCALLTYPE GetPredication
87	virtual void STDMETHODCALLTYPE GSGetShaderResources
88	virtual void STDMETHODCALLTYPE GSGetSamplers
89	virtual void STDMETHODCALLTYPE OMGetRenderTargets
90	virtual void STDMETHODCALLTYPE OMGetRenderTargetsAndUnorderedAccessViews
91	virtual void STDMETHODCALLTYPE OMGetBlendState
92	virtual void STDMETHODCALLTYPE OMGetDepthStencilState
93	virtual void STDMETHODCALLTYPE SOGetTargets
94	virtual void STDMETHODCALLTYPE RSGetState
95	virtual void STDMETHODCALLTYPE RSGetViewports
96	virtual void STDMETHODCALLTYPE RSGetScissorRects
97	virtual void STDMETHODCALLTYPE HSGetShaderResources
98	virtual void STDMETHODCALLTYPE HSGetShader
99	virtual void STDMETHODCALLTYPE HSGetSamplers
100	virtual void STDMETHODCALLTYPE HSGetConstantBuffers
101	virtual void STDMETHODCALLTYPE DSGetShaderResources
102	virtual void STDMETHODCALLTYPE DSGetShader
103	virtual void STDMETHODCALLTYPE DSGetSamplers
104	virtual void STDMETHODCALLTYPE DSGetConstantBuffers
105	virtual void STDMETHODCALLTYPE CSGetShaderResources
106	virtual void STDMETHODCALLTYPE CSGetUnorderedAccessViews
107	virtual void STDMETHODCALLTYPE CSGetShader
108	virtual void STDMETHODCALLTYPE CSGetSamplers
109	virtual void STDMETHODCALLTYPE CSGetConstantBuffers
110	virtual void STDMETHODCALLTYPE ClearState
111	virtual void STDMETHODCALLTYPE Flush
112	virtual D3D11_DEVICE_CONTEXT_TYPE STDMETHODCALLTYPE GetType
113	virtual UINT STDMETHODCALLTYPE GetContextFlags
114	virtual HRESULT STDMETHODCALLTYPE FinishCommandList

-----------------------------------------------------------------------
// IUnknown
0	virtual HRESULT STDMETHODCALLTYPE QueryInterface
1	virtual ULONG STDMETHODCALLTYPE AddRef
2	virtual ULONG STDMETHODCALLTYPE Release

// IDXGIObject
3	virtual HRESULT STDMETHODCALLTYPE SetPrivateData
4	virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface
5	virtual HRESULT STDMETHODCALLTYPE GetPrivateData
6	virtual HRESULT STDMETHODCALLTYPE GetParent

// IDXGIDeviceSubObject
7   virtual HRESULT STDMETHODCALLTYPE GetDevice

// IDXGISwapChain
8	virtual HRESULT STDMETHODCALLTYPE Present
9	virtual HRESULT STDMETHODCALLTYPE GetBuffer
10	virtual HRESULT STDMETHODCALLTYPE SetFullscreenState
11	virtual HRESULT STDMETHODCALLTYPE GetFullscreenState
12	virtual HRESULT STDMETHODCALLTYPE GetDesc
13	virtual HRESULT STDMETHODCALLTYPE ResizeBuffers
14	virtual HRESULT STDMETHODCALLTYPE ResizeTarget
15	virtual HRESULT STDMETHODCALLTYPE GetContainingOutput
16	virtual HRESULT STDMETHODCALLTYPE GetFrameStatistics
17	virtual HRESULT STDMETHODCALLTYPE GetLastPresentCount

*/