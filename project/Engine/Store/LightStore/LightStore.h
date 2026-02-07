#pragma once
#include <vector>
#include "Handle/Handle.h"
#include "LightData/BaseLightData.h"

#include "Application/Light/LightDirectional/LightDirectional.h"

namespace Engine
{
	class LightStore
	{
	public:

		/// @brief 読み込み
		/// @param name 
		/// @param type 
		/// @return 
		LightHandle Load(const std::string& name, const std::string& type);

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