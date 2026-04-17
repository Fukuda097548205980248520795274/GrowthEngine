#pragma once
#include "../BaseInput.h"

class InputGamepadLeftStick : public Engine::BaseInput
{
public:

	/// @brief コンストラクタ
	/// @param name 
	/// @param inputState 
	/// @param controller 
	/// @param direction 
	/// @param dot 
	InputGamepadLeftStick(const std::string& name, InputState inputState, DWORD controller, const Vector2& direction, float dot);

	/// @brief パラメータ
	Engine::InputData::GamepadStickParam* param_ = nullptr;
};

