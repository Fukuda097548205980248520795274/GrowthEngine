#include "InputGamepadLeftTriggerResource.h"

/// @brief コンストラクタ
/// @param inputState 
/// @param hInput 
/// @param name 
/// @param controller 
/// @param threshold 
Engine::InputGamepadLeftTriggerResource::InputGamepadLeftTriggerResource(InputState inputState, InputHandle hInput, const std::string& name, DWORD controller, float threshold)
	: InputBaseResource(inputState, hInput, name)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<InputData::GamepadTriggerParam>();
	param_->controller = controller;
	param_->threshold = threshold;
}

/// @brief 更新処理
void Engine::InputGamepadLeftTriggerResource::Update()
{
	// 入力を初期化する
	isInput_ = false;

	// 入力状態
	switch (inputState_)
	{
	case InputState::Press:

		// 閾値を越えたかどうか
		if (input_->GetGamepadRightTrigger(param_->controller) >= param_->threshold)
			isInput_ = true;

		break;

	case InputState::Trigger:

		break;

	case InputState::Release:

		break;
	}
}