#pragma once
#include "Handle/Handle.h"
#include "Data/Prefab3DData/Prefab3DData.h"
#include "Data/Prefab2DData/Prefab2DData.h"
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

		/// @brief 描画処理
		virtual void Draw() = 0;


	protected:

		/// @brief エンジン
		const GrowthEngine* engine_ = nullptr;

		/// @brief 名前
		std::string name_{};
	};
}