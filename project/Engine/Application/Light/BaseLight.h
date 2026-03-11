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

		/// @brief 設置する
		void Set();


	protected:

		/// @brief エンジン
		const GrowthEngine* engine_ = nullptr;

		/// @brief 名前
		std::string name_{};

		/// @brief ハンドル
		LightHandle hLight_ = 0;
	};
}