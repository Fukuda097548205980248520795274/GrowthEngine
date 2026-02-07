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
		/// @param handle 
		/// @return 
		void* GetParam(LightHandle handle);


	private:

		/// @brief データテーブル
		std::vector<std::unique_ptr<BaseLightData>> dataTable_;
	};
}