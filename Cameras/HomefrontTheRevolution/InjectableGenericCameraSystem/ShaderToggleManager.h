////////////////////////////////////////////////////////////////////////////////////////////////////////
// Part of Injectable Generic Camera System
// Copyright(c) 2017, Frans Bouma
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

#include <unordered_map>
#include <mutex>
#include <d3d11.h> 

namespace IGCS
{
	class ShaderToggleManager
	{
	public:
		ShaderToggleManager();

		// Singleton instance retriever.
		static ShaderToggleManager& instance()
		{
			static ShaderToggleManager theInstance;
			return theInstance;
		}

		void addShader(const void *buffer, size_t bufferLength, __int64 shaderInstanceAddress);
		void addShader(__int64 shaderInstanceAddress);
		void init(ID3D11Device *device);
		void reset();
		void toggleMarkMode();
		void markModeHideNext();
		void markModeHidePrevious();
		bool isShaderHidden(__int64 shaderAddress);

		ID3D11PixelShader* getDiscardingPixelShader() { return _discardingPixelShader; }
		bool getInMarkMode() { return _inMarkMode; }

		ShaderToggleManager(ShaderToggleManager const&) = delete;			// see: https://stackoverflow.com/a/1008289/44991
		void operator=(ShaderToggleManager const&) = delete;


	private:
		__int64 calculateFNVHash(const void *buffer, size_t bufferLength);
		void clearShaderHashMap();
		void setCurrentMarkedShaderAddress();

		ID3DBlob* _compiledDiscardPixelShaderBlob;
		ID3D11PixelShader* _discardingPixelShader;
		std::unordered_map<__int64, __int64> _shaderHashPerShaderObjectAddress;			// so the 64bit shader object address is the key, the 64bit hash of the buffer is the value. 
		std::mutex _shaderHashLock;		// we need a lock as the call to add a shader and to check whether a shader is present are coming from different threads
		bool _inMarkMode;
		int _markModeCurrentHiddenShaderIndex;		// we simply loop over the hashmap for these amount of times, till we either reach this value or the end of the hashmap, in which case we start at the beginning
		__int64 _markModeCurrentHiddenShaderAddress;  // determined by iterating over shader hashmap.
	};
}

