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
#include "ShaderToggleManager.h"
#include "Utils.h"

#pragma comment(lib, "d3d11.lib")

namespace IGCS::DX11Hooker
{
	// DXGISwapchain
	#define DXGI_PRESENT_INDEX						8
	#define DXGI_RESIZEBUFFERS_INDEX				13
	// Device
	#define DX11_CREATE_DEFERRED_CONTEXT			27
	// DeviceContext
	#define DX11_PS_SET_SHADER						9
	#define DX11_DRAW_INDEXED						12
	#define DX11_DRAW								13
	#define DX11_DRAW_INDEXED_INSTANCED				20
	#define DX11_DRAW_INSTANCED						21
	#define DX11_DRAW_AUTO							38
	#define DX11_DRAW_INDEXED_INSTANCED_INDIRECT	39
	#define DX11_DRAW_INSTANCED_INDIRECT			40



	//--------------------------------------------------------------------------------------------------------------------------------
	// Forward declarations
	void createRenderTarget(IDXGISwapChain* pSwapChain);
	void cleanupRenderTarget();
	void hookDeviceMethods();
	void hookContextMethods();
	__int64 getCurrentPixelShaderAddress();
	bool checkIfDrawCallShouldProceedAndRegisterShaderIfNeeded();

	//--------------------------------------------------------------------------------------------------------------------------------
	// Typedefs of functions to hook
	// dxgiswapchain
	typedef HRESULT(__stdcall *D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	typedef HRESULT(__stdcall *D3D11ResizeBuffersHook) (IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
	// device
	typedef HRESULT(__stdcall *D3D11CreateDeferredContextHook) (ID3D11Device* pDevice, UINT contextFlags, ID3D11DeviceContext** ppDeferredContext);
	// devicecontext
	typedef HRESULT(__stdcall *D3D11PSSetShaderHook) (ID3D11DeviceContext* pDeviceContext, ID3D11PixelShader* pPixelShader, ID3D11ClassInstance *const *ppClassInstances, UINT numClassInstances);
	typedef HRESULT(__stdcall *D3D11DrawHook) (ID3D11DeviceContext* pDeviceContext, UINT vertexCount, UINT startVertexLocation);
	typedef HRESULT(__stdcall *D3D11DrawAutoHook) (ID3D11DeviceContext* pDeviceContext);
	typedef HRESULT(__stdcall *D3D11DrawIndexedHook) (ID3D11DeviceContext* pDeviceContext, UINT indexCount, UINT startIndexLocation, INT baseVertexLocation);
	typedef HRESULT(__stdcall *D3D11DrawIndexedInstancedHook) (ID3D11DeviceContext* pDeviceContext, UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation);
	typedef HRESULT(__stdcall *D3D11DrawIndexedInstancedIndirectHook) (ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pBufferForArgs, UINT alignedByteOffsetForArgs);
	typedef HRESULT(__stdcall *D3D11DrawInstancedHook) (ID3D11DeviceContext* pDeviceContext, UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation);
	typedef HRESULT(__stdcall *D3D11DrawInstancedIndirectHook) (ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pBufferForArgs, UINT alignedByteOffsetForArgs);

	static ID3D11Device* _device = nullptr;
	static ID3D11DeviceContext* _context = nullptr;
	static ID3D11RenderTargetView* _mainRenderTargetView = nullptr;

	//--------------------------------------------------------------------------------------------------------------------------------
	// Pointers to the original hooked functions
	// dxgiswapchain
	static D3D11PresentHook hookedD3D11Present = nullptr;
	static D3D11ResizeBuffersHook hookedD3D11ResizeBuffers = nullptr;
	// device
	static D3D11CreateDeferredContextHook hookedD3D11CreateDeferredContext = nullptr;
	// devicecontext
	static D3D11PSSetShaderHook hookedD3D11PSSetShader = nullptr;
	static D3D11DrawHook hookedD3D11Draw = nullptr;
	static D3D11DrawAutoHook hookedD3D11DrawAuto = nullptr;
	static D3D11DrawIndexedHook hookedD3D11DrawIndexed = nullptr;
	static D3D11DrawIndexedInstancedHook hookedD3D11DrawIndexedInstanced = nullptr;
	static D3D11DrawIndexedInstancedIndirectHook hookedD3D11DrawIndexedInstancedIndirect = nullptr;
	static D3D11DrawInstancedHook hookedD3D11DrawInstanced = nullptr;
	static D3D11DrawInstancedIndirectHook hookedD3D11DrawInstancedIndirect = nullptr;

	static bool _tmpSwapChainInitialized = false;
	static bool _showWindow = true;
	static bool _imGuiInitializing = false;

	//--------------------------------------------------------------------------------------------------------------------------------
	// Detour methods which are called by the hooked vtable entrances
	//------[Device]------------------------------------------------------------------------------------------------------------------
	HRESULT __stdcall detourD3D11CreateDeferredContext(ID3D11Device* pDevice, UINT contextFlags, ID3D11DeviceContext** ppDeferredContext)
	{
		// TODO: hook PSSetShader of created devicecontext. Be sure to unhook it after the devicecontext is converted into a commandlist. 

		return hookedD3D11CreateDeferredContext(pDevice, contextFlags, ppDeferredContext);
	}

	//------[DeviceContext]------------------------------------------------------------------------------------------------------------------
	HRESULT __stdcall detourD3D11PSSetShader(ID3D11DeviceContext* pDeviceContext, ID3D11PixelShader* pPixelShader, ID3D11ClassInstance *const *ppClassInstances, UINT numClassInstances)
	{
		ShaderToggleManager::instance().addShader((__int64)pPixelShader);
		return hookedD3D11PSSetShader(pDeviceContext, pPixelShader, ppClassInstances, numClassInstances);
	}
	

	HRESULT __stdcall detourD3D11Draw (ID3D11DeviceContext* pDeviceContext, UINT vertexCount, UINT startVertexLocation)
	{
		HRESULT toReturn = S_OK;
		if (checkIfDrawCallShouldProceedAndRegisterShaderIfNeeded())
		{
			toReturn = hookedD3D11Draw(pDeviceContext, vertexCount, startVertexLocation);
		}
		return toReturn;
	}
	

	HRESULT __stdcall detourD3D11DrawAuto(ID3D11DeviceContext* pDeviceContext)
	{
		HRESULT toReturn = S_OK;
		if (checkIfDrawCallShouldProceedAndRegisterShaderIfNeeded())
		{
			toReturn = hookedD3D11DrawAuto(pDeviceContext);
		}
		return toReturn;
	}
	

	HRESULT __stdcall detourD3D11DrawIndexed(ID3D11DeviceContext* pDeviceContext, UINT indexCount, UINT startIndexLocation, INT baseVertexLocation)
	{
		HRESULT toReturn = S_OK;
		if (checkIfDrawCallShouldProceedAndRegisterShaderIfNeeded())
		{
			toReturn = hookedD3D11DrawIndexed(pDeviceContext, indexCount, startIndexLocation, baseVertexLocation);
		}
		return toReturn;
	}
	

	HRESULT __stdcall detourD3D11DrawIndexedInstanced(ID3D11DeviceContext* pDeviceContext, UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, 
													  INT baseVertexLocation, UINT startInstanceLocation)
	{
		HRESULT toReturn = S_OK;
		if (checkIfDrawCallShouldProceedAndRegisterShaderIfNeeded())
		{
			toReturn = hookedD3D11DrawIndexedInstanced(pDeviceContext, indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
		}
		return toReturn;
	}


	HRESULT __stdcall detourD3D11DrawIndexedInstancedIndirect(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pBufferForArgs, UINT alignedByteOffsetForArgs)
	{
		HRESULT toReturn = S_OK;
		if (checkIfDrawCallShouldProceedAndRegisterShaderIfNeeded())
		{
			toReturn = hookedD3D11DrawIndexedInstancedIndirect(pDeviceContext, pBufferForArgs, alignedByteOffsetForArgs);
		}
		return toReturn;
	}


	HRESULT __stdcall detourD3D11DrawInstanced(ID3D11DeviceContext* pDeviceContext, UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation)
	{
		HRESULT toReturn = S_OK;
		if (checkIfDrawCallShouldProceedAndRegisterShaderIfNeeded())
		{
			toReturn = hookedD3D11DrawInstanced(pDeviceContext, vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
		}
		return toReturn;
	}


	HRESULT __stdcall detourD3D11DrawInstancedIndirect(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pBufferForArgs, UINT alignedByteOffsetForArgs)
	{
		HRESULT toReturn = S_OK;
		if (checkIfDrawCallShouldProceedAndRegisterShaderIfNeeded())
		{
			toReturn = hookedD3D11DrawInstancedIndirect(pDeviceContext, pBufferForArgs, alignedByteOffsetForArgs);
		}
		return toReturn;
	}
	

	//------[DXGISwapChain]------------------------------------------------------------------------------------------------------------------
	HRESULT __stdcall detourD3D11ResizeBuffers(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags)
	{
		_imGuiInitializing = true;

		ShaderToggleManager::instance().reset();
		ImGui_ImplDX11_InvalidateDeviceObjects();
		cleanupRenderTarget();

		HRESULT toReturn = hookedD3D11ResizeBuffers(pSwapChain, bufferCount, width, height, newFormat, swapChainFlags);

		createRenderTarget(pSwapChain);
		ImGui_ImplDX11_CreateDeviceObjects();
		ShaderToggleManager::instance().init(_device);

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
					OverlayControl::initImGui(_device, _context);

					hookDeviceMethods();
					hookContextMethods();

					// initialize our shader manager, which might upload shaders.
					ShaderToggleManager::instance().init(_device);

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

		Utils::setAndEnableHook((LPBYTE)pSwapChainVTable[DXGI_PRESENT_INDEX], &detourD3D11Present, reinterpret_cast<LPVOID*>(&hookedD3D11Present), "DXGISwapChain::Present");
		Utils::setAndEnableHook((LPBYTE)pSwapChainVTable[DXGI_RESIZEBUFFERS_INDEX], &detourD3D11ResizeBuffers, reinterpret_cast<LPVOID*>(&hookedD3D11ResizeBuffers), "DXGISwapChain::ResizeBuffers");

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

		Utils::setAndEnableHook((LPBYTE)pDeviceVTable[DX11_CREATE_DEFERRED_CONTEXT], &detourD3D11CreateDeferredContext, reinterpret_cast<LPVOID*>(&hookedD3D11CreateDeferredContext),
								"ID3D11Device::CreateDeferredContext");
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
		Utils::setAndEnableHook((LPBYTE)pDeviceContextVTable[DX11_PS_SET_SHADER], &detourD3D11PSSetShader, reinterpret_cast<LPVOID*>(&hookedD3D11PSSetShader), "ID3D11DeviceContext::PSSetShader");
		Utils::setAndEnableHook((LPBYTE)pDeviceContextVTable[DX11_DRAW_INDEXED], &detourD3D11DrawIndexed, reinterpret_cast<LPVOID*>(&hookedD3D11DrawIndexed), "ID3D11DeviceContext::DrawIndexed");
		Utils::setAndEnableHook((LPBYTE)pDeviceContextVTable[DX11_DRAW], &detourD3D11Draw, reinterpret_cast<LPVOID*>(&hookedD3D11Draw), "ID3D11DeviceContext::Draw");
		Utils::setAndEnableHook((LPBYTE)pDeviceContextVTable[DX11_DRAW_INDEXED_INSTANCED], &detourD3D11DrawIndexedInstanced, reinterpret_cast<LPVOID*>(&hookedD3D11DrawIndexedInstanced), "ID3D11DeviceContext::DrawIndexedInstanced");
		Utils::setAndEnableHook((LPBYTE)pDeviceContextVTable[DX11_DRAW_INSTANCED], &detourD3D11DrawInstanced, reinterpret_cast<LPVOID*>(&hookedD3D11DrawInstanced), "ID3D11DeviceContext::DrawInstanced");
		Utils::setAndEnableHook((LPBYTE)pDeviceContextVTable[DX11_DRAW_AUTO], &detourD3D11DrawAuto, reinterpret_cast<LPVOID*>(&hookedD3D11DrawAuto), "ID3D11DeviceContext::DrawAuto");
		Utils::setAndEnableHook((LPBYTE)pDeviceContextVTable[DX11_DRAW_INDEXED_INSTANCED_INDIRECT], &detourD3D11DrawIndexedInstancedIndirect, reinterpret_cast<LPVOID*>(&hookedD3D11DrawIndexedInstancedIndirect), "ID3D11DeviceContext::DrawIndexedInstancedIndirect");
		Utils::setAndEnableHook((LPBYTE)pDeviceContextVTable[DX11_DRAW_INSTANCED_INDIRECT], &detourD3D11DrawInstancedIndirect, reinterpret_cast<LPVOID*>(&hookedD3D11DrawInstancedIndirect), "ID3D11DeviceContext::DrawInstancedIndirect");
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


	// Registers the current pixel shader address with the entropy calculated of the current context state, if the shader is unknown. 
	// returns true if the caller should proceed with its original call or do nothing and return S_OK. 
	// This value is calculated from the usability state of the current shader. If the shader is unknown at this point, it will 
	// be picked up the next frame and we're returning true.
	// It's not that great to merge two things into one thing, as that's in general going to bite you in the ass later on, but 
	// it's an optimization to do it this way as it can now be done in one call and no if logic in the caller, and as a lot of methods
	// will need this method, it's best to use 1 method instead of two with if-logic.
	bool checkIfDrawCallShouldProceedAndRegisterShaderIfNeeded()
	{
		__int64 pixelShaderAddress = getCurrentPixelShaderAddress();
		ShaderUsabilityType currentShaderUsabilityState = ShaderToggleManager::instance().getShaderUsability(pixelShaderAddress);
		bool toReturn = false;
		switch (currentShaderUsabilityState)
		{
		case ShaderUsabilityType::Blocked:
			// do nothing
			break;
		case ShaderUsabilityType::Unblocked:
			// simply call the real method
			toReturn = true;
			break;
		case ShaderUsabilityType::Unknown:
			// calculate context state entropy, register shader with the entropy, continue as normal, so render.
// TODO: Add entropy calculation / registration.
			toReturn = true;
			break;
		}
		return toReturn;
	}



	__int64 getCurrentPixelShaderAddress()
	{
		ID3D11PixelShader* currentShader;
		ID3D11ClassInstance* currentShaderInstances[256];
		UINT amountOfInstances;
		_context->PSGetShader(&currentShader, currentShaderInstances, &amountOfInstances);
		__int64 toReturn = (__int64)currentShader;
		currentShader->Release();
		for (UINT i = 0; i < amountOfInstances; i++)
		{
			if (currentShaderInstances[i])
			{
				currentShaderInstances[i]->Release();
			}
		}
		return toReturn;
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