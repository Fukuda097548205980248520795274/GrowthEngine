#pragma once
#include "Handle/Handle.h"
#include <string>
#include "Data/LightData/LightData.h"

class GrowthEngine;

namespace Engine
{
	class BaseLight
	{
	public:

		/// @brief 仮想デストラクタ
		virtual ~BaseLight() = default;

		/// @brief コンストラクタ
		/// @param name 
		BaseLight(const std::string& name);

		/// @brief ライトの種類を取得する
		/// @return 
		Light::Type GetType() const { return type_; }


	protected:

		/// @brief エンジン
		const GrowthEngine* engine_ = nullptr;

		/// @brief 名前
		std::string name_{};

		/// @brief ハンドル
		LightHandle hLight_ = 0;

		/// @brief ライトの種類
		Light::Type type_ = Engine::Light::Type::None;
	};
}