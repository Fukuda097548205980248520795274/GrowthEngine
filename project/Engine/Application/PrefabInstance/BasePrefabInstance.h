#pragma once
#include <functional>
#include "Data/Prefab3DData/Prefab3DData.h"
#include "Data/Prefab2DData/Prefab2DData.h"

namespace Engine
{
	class BasePrefabInstance
	{
	public:

		/// @brief 仮想デストラクタ
		virtual ~BasePrefabInstance() = default;

		/// @brief コンストラクタ
		BasePrefabInstance() = default;

		/// @brief 描画処理
		virtual void Draw() = 0;

		/// @brief 削除フラグ
		bool isDelete_ = false;
	};
}