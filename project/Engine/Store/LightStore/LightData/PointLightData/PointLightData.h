#pragma once
#include "../BaseLightData.h"

namespace Engine
{
	class PointLightData : public BaseLightData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param handle 
		PointLightData(const std::string& name, LightHandle handle);

		/// @brief 初期化
		/// @param parameter 
		void Initialize(LightParameter* parameter) override;

		/// @brief 
		/// @return 
		void* GetParam() override { return param_.get(); }

		/// @brief 種別名を取得する
		/// @return 
		Light::Type GetType() const override { return Light::Type::Point; }

		/// @brief デバッグ用描画処理
		void DebugDraw() override;


	private:

		// パラメータ
		std::unique_ptr<Light::PointLightParam> param_ = nullptr;
	};
}