#pragma once
#include "../BaseInput.h"

/// @brief キー入力
class InputKey : public Engine::BaseInput
{
public:

	/// @brief コンストラクタ
	/// @param name 
	/// @param inputState 
	/// @param key 
	InputKey(const std::string& name, InputState inputState, BYTE key);

	/// @brief パラメータ
	Engine::InputData::KeyParam* param_ = nullptr;
};

