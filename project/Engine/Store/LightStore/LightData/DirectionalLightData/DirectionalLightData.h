#pragma once
#include "../BaseLightData.h"
#include "Data/LightData/LightData.h"

namespace Engine
{

	class DirectionalLightData : public BaseLightData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param handle 
		DirectionalLightData(const std::string& name, LightHandle handle);

		/// @brief 
		/// @return 
		void* GetParam() override { return param_.get(); }

		/// @brief 種別名を取得する
		/// @return 
		const char* GetTypeName() const override { return "Directional"; }


	private:

		// パラメータ
		std::unique_ptr<DirectionalLightParam> param_ = nullptr;
	};
}