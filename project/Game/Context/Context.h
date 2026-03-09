#pragma once
#include "GrowthEngine.h"

class Context
{
public:

	/// @brief コンストラクタ
	Context();



private:

	/// @brief エンジン
	const GrowthEngine* engine_ = nullptr;
};

