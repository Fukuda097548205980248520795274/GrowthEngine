#include "InputKey.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
/// @param inputState 
/// @param key 
InputKey::InputKey(const std::string& name, InputState inputState, BYTE key) : BaseInput()
{
	// キー入力の読み込み
	hInput_ = engine_->LoadInputKey(name, inputState, key);

	// パラメータを取得する
	param_ = engine_->GetInputParam<Engine::InputData::KeyParam>(hInput_);
}