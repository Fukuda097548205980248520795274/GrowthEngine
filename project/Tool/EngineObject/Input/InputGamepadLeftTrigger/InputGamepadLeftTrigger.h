#pragma once
#include "../BaseInput.h"

class InputGamepadLeftTrigger : public Engine::BaseInput
{
public:

	/// @brief コンストラクタ
	/// @param name 
	/// @param inputState 
	/// @param controller 
	/// @param threshold 
	InputGamepadLeftTrigger(const std::string& name, InputState inputState, DWORD controller, float threshold);

	/// @brief パラメータ
	Engine::InputData::GamepadTriggerParam* param_ = nullptr;
};

