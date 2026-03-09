#include "BaseInput.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
Engine::BaseInput::BaseInput()
{
	// インスタンスを取得する
	engine_ = GrowthEngine::GetInstance();
}

/// @brief 入力したかどうか
/// @return 
bool Engine::BaseInput::IsInput() const
{
	return engine_->IsInput(hInput_);
}