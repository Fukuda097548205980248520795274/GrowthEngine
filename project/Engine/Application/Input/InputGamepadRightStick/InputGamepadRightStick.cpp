#include "InputGamepadRightStick.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
/// @param inputState 
/// @param controller 
/// @param direction 
/// @param dot 
InputGamepadRightStick::InputGamepadRightStick(const std::string& name, InputState inputState, DWORD controller, const Vector2& direction, float dot) : BaseInput()
{
	// スティック入力読み込み
	hInput_ = engine_->LoadInputGamepadStick(name, inputState, StickType::Right, controller, direction, dot);

	// パラメータを取得する
	param_ = engine_->GetInputParam<Engine::InputData::GamepadStickParam>(hInput_);
}