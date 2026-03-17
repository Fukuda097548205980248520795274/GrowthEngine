#pragma once
#include "Handle/Handle.h"
#include "Data/PrefabData/PrefabData.h"
#include <string>

class GrowthEngine;

namespace Engine
{
	class BasePrefabBase
	{
	public:

		/// @brief 仮想デストラクタ
		virtual ~BasePrefabBase() = default;

		/// @brief コンストラクタ
		/// @param name 
		BasePrefabBase(const std::string& name);


	protected:

		/// @brief エンジン
		const GrowthEngine* engine_ = nullptr;

		/// @brief 名前
		std::string name_{};
	};
}