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

		/// @brief リセット
		void Reset() override;

		/// @brief 
		/// @return 
		void* GetParam() override { return param_.get(); }

		/// @brief 輝度を取得する
		/// @return 
		float GetIntensity() override { return param_->intensity; }

		/// @brief 種別名を取得する
		/// @return 
		Light::Type GetType() const override { return Light::Type::Point; }


	public:

		/// @brief デバッグ用描画処理
		void DebugParameter() override;


	private:

		// パラメータ
		std::unique_ptr<Light::PointLightParam> param_ = nullptr;
	};
}