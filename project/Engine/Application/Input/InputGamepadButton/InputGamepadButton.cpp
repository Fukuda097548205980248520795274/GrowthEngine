#include "InputGamepadButton.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
/// @param inputState 
/// @param controller 
/// @param button 
InputGamepadButton::InputGamepadButton(const std::string& name, InputState inputState, DWORD controller, DWORD button) : BaseInput(name)
{
	// キー入力の読み込み
	hInput_ = engine_->LoadInputGamepadButton(name_, inputState, controller, button);

	// パラメータを取得する
	param_ = engine_->GetInputParam<Engine::InputData::GamepadButtonParam>(hInput_);
}