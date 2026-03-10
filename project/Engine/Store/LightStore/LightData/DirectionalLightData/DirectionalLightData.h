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

		/// @brief ビュープロジェクション行列を取得する
		/// @return 
		Matrix4x4 GetViewProjectionMatrix() const;

		/// @brief 
		/// @return 
		void* GetParam() override { return param_.get(); }

		/// @brief 種別名を取得する
		/// @return 
		Light::Type GetType() const override { return Light::Type::Directional; }


	private:

		// パラメータ
		std::unique_ptr<Light::DirectionalLightParam> param_ = nullptr;
	};
}