#pragma once
#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>

#include "Handle/Handle.h"
#include "LightData/BaseLightData.h"

#include "Application/Light/LightDirectional/LightDirectional.h"

namespace Engine
{
	class DX12Heap;
	class Log;

	class LightStore
	{
	public:

		/// @brief 読み込み
		/// @param name 
		/// @param type 
		/// @param heap 
		/// @param device 
		/// @param log 
		/// @return 
		LightHandle Load(const std::string& name, const std::string& type, DX12Heap* heap, ID3D12Device* device, Log* log);

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param handle 
		/// @return 
		template <typename T>
		T* GetParam(LightHandle handle)
		{
			BaseLightData* data = dataTable_[handle].get();
			return static_cast<T*>(data->GetParam());
		}


	private:

		/// @brief データテーブル
		std::vector<std::unique_ptr<BaseLightData>> dataTable_;
	};
}