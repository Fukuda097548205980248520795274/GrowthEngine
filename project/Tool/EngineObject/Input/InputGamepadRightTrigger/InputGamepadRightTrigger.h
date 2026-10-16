#pragma once
#include "../BaseInput.h"

/// @brief ゲームパッド右トリガー入力
class InputGamepadRightTrigger : public Engine::BaseInput
{
public:

	/// @brief コンストラクタ
	/// @param name 
	/// @param inputState 
	/// @param controller 
	/// @param threshold 
	InputGamepadRightTrigger(const std::string& name, InputState inputState, DWORD controller, float threshold);

	/// @brief パラメータ
	Engine::InputData::GamepadTriggerParam* param_ = nullptr;
};

