#pragma once
#include "../BaseInput.h"

/// @brief ゲームパッドボタン入力
class InputGamepadButton : public Engine::BaseInput
{
public:

	/// @brief コンストラクタ
	/// @param name 
	/// @param inputState 
	/// @param controller 
	/// @param button 
	InputGamepadButton(const std::string& name, InputState inputState, DWORD controller, DWORD button);

	/// @brief パラメータ
	Engine::InputData::GamepadButtonParam* param_ = nullptr;
};

