#pragma once
#include "../BaseLightData.h"

namespace Engine
{
	class SpotLightData : public BaseLightData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param handle 
		SpotLightData(const std::string& name, LightHandle handle);

		/// @brief 
		/// @return 
		void* GetParam() override { return param_.get(); }

		/// @brief 種別名を取得する
		/// @return 
		Light::Type GetType() const override { return Light::Type::Spot; }


	private:

		// パラメータ
		std::unique_ptr<Light::SpotLightParam> param_ = nullptr;
	};
}