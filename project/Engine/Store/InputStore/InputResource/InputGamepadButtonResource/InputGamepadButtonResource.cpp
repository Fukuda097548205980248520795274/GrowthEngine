#include "InputGamepadButtonResource.h"

/// @brief コンストラクタ
/// @param inputState 
/// @param hInput 
/// @param name 
/// @param button 
Engine::InputGamepadButtonResource::InputGamepadButtonResource(InputState inputState, InputHandle hInput, const std::string& name, DWORD controller, DWORD button)
	: InputBaseResource(inputState, hInput, name)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<InputData::GamepadButtonParam>();
	param_->controller = controller;
	param_->button = button;
}

/// @brief 更新処理
void Engine::InputGamepadButtonResource::Update()
{
	// 入力を初期化する
	isInput_ = false;

	switch (inputState_)
	{
	case InputState::Press:
		isInput_ = input_->GetGamepadButtonPress(param_->controller, param_->button);
		break;

	case InputState::Trigger:
		isInput_ = input_->GetGamepadButtonTrigger(param_->controller, param_->button);
		break;

	case InputState::Release:
		isInput_ = input_->GetGamepadButtonRelease(param_->controller, param_->button);
		break;
	}
}