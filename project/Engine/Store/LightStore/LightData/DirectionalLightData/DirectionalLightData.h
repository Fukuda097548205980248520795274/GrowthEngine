#pragma once
#include "../BaseLightData.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

namespace Engine
{
	class Log;

	class DirectionalLightData : public BaseLightData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param handle 
		DirectionalLightData(const std::string& name, LightHandle handle);

		/// @brief 初期化
		/// @param parameter 
		void Initialize(LightParameter* parameter) override;

		/// @brief リセット
		void Reset() override;

		/// @brief ビュープロジェクション行列を取得する
		/// @return 
		Matrix4x4 GetViewProjectionMatrix() const;

		/// @brief 
		/// @return 
		void* GetParam() override { return param_.get(); }

		/// @brief 輝度を取得する
		/// @return 
		float GetIntensity() override { return param_->intensity; }

		/// @brief 種別名を取得する
		/// @return 
		Light::Type GetType() const override { return Light::Type::Directional; }

		/// @brief デバッグ用描画処理
		void DebugDraw() override;


	private:

		// パラメータ
		std::unique_ptr<Light::DirectionalLightParam> param_ = nullptr;
	};
}