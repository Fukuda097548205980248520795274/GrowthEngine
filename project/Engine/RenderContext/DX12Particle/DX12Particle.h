#pragma once
#include "Store/Particle3DStore/Particle3DStore.h"

namespace Engine
{
	class DX12Particle
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param heap 
		/// @param modelStore 
		/// @param textureStore
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, DX12Heap* heap, ModelStore* modelStore, TextureStore* textureStore, Log* log);


	private:

		// 3Dパーティクルストア
		std::unique_ptr<Particle3DStore> particle3DStore_ = nullptr;
	};
}