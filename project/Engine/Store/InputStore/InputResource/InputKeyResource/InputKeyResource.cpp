#include "InputKeyResource.h"

/// @brief コンストラクタ
/// @param inputState 
/// @param hInput 
/// @param name 
Engine::InputKeyResource::InputKeyResource(InputState inputState, InputHandle hInput, const std::string& name, BYTE key)
	: InputBaseResource(inputState, hInput, name)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<InputData::KeyParam>();
	param_->key = key;
}

/// @brief 更新処理
void Engine::InputKeyResource::Update()
{
	// 入力を初期化する
	isInput_ = false;

	switch (inputState_)
	{
	case InputState::Press:
		isInput_ = input_->GetKeyPress(param_->key);
		break;

	case InputState::Trigger:
		isInput_ = input_->GetKeyTrigger(param_->key);
		break;

	case InputState::Release:
		isInput_ = input_->GetKeyRelease(param_->key);
		break;
	}
}