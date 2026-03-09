#pragma once
#include "Data/InputData/InputData.h"
#include "Store/InputStore/InputResource/InputBaseResource.h"
#include "Handle/Handle.h"
#include <string>

class GrowthEngine;

namespace Engine
{
	class BaseInput
	{
	public:

		/// @brief 仮想デストラクタ
		virtual ~BaseInput() = default;

		/// @brief コンストラクタ
		BaseInput();

		/// @brief 入力したかどうか
		/// @return 
		bool IsInput()const;


	protected:

		/// @brief エンジン
		const GrowthEngine* engine_ = nullptr;

		/// @brief 入力ハンドル
		InputHandle hInput_ = 0;
	};
}