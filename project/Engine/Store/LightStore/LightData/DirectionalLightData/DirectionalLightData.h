#pragma once
#include "../BaseLightData.h"

namespace Engine
{
	/// @brief 平行光源パラメータ
	struct DirectionalLightParam
	{
		// 向き
		std::unique_ptr<Vector3> direction = nullptr;

		// 輝度
		std::unique_ptr<float> intensity = nullptr;

		// 色
		std::unique_ptr<Vector3> color = nullptr;
	};

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