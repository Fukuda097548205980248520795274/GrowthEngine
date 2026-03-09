#include "InputGamepadLeftStick.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
/// @param inputState 
/// @param controller 
/// @param direction 
/// @param dot 
InputGamepadLeftStick::InputGamepadLeftStick(const std::string& name, InputState inputState, DWORD controller, const Vector2& direction, float dot) : BaseInput(name)
{
	// スティック入力読み込み
	hInput_ = engine_->LoadInputGamepadStick(name_, inputState, StickType::Left, controller, direction, dot);

	// パラメータを取得する
	param_ = engine_->GetInputParam<Engine::InputData::GamepadStickParam>(hInput_);
}