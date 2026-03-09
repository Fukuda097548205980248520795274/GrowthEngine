#include "Context.h"

/// @brief コンストラクタ
Context::Context()
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();


}