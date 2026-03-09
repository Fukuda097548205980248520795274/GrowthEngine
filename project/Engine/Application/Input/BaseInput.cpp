#include "BaseInput.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
Engine::BaseInput::BaseInput(const std::string& name) : name_(name)
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