#include "InputGamepadLeftTrigger.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
/// @param inputState 
/// @param controller 
/// @param threshold 
InputGamepadLeftTrigger::InputGamepadLeftTrigger(const std::string& name, InputState inputState, DWORD controller, float threshold) : BaseInput()
{
	// スティック入力読み込み
	hInput_ = engine_->LoadInputGamepadTrigger(name, inputState, TriggerType::Left, controller, threshold);

	// パラメータを取得する
	param_ = engine_->GetInputParam<Engine::InputData::GamepadTriggerParam>(hInput_);
}