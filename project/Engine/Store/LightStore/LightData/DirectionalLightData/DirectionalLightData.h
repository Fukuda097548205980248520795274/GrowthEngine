#pragma once
#include "../BaseLightData.h"

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
		void* GetParam() override { return &param_; }

		/// @brief 種別名を取得する
		/// @return 
		const char* GetTypeName() const override { return "Directional"; }


	private:

		// パラメータ
		DirectionalLightParam param_{};
	};
}