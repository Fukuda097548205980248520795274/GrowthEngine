#include "InputMouseResource.h"

/// @brief コンストラクタ
/// @param button 
/// @param inputState 
/// @param hInput 
/// @param name 
Engine::InputMouseResource::InputMouseResource(MouseButton button, InputState inputState, InputHandle hInput, const std::string& name)
	: InputBaseResource(inputState, hInput, name)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<InputData::MouseButtonParam>();
	param_->button = button;
}

/// @brief 更新処理
void Engine::InputMouseResource::Update()
{
	// 入力を初期化する
	isInput_ = false;

	switch (inputState_)
	{
	case InputState::Press:
		isInput_ = input_->GetMousePress(static_cast<uint32_t>(param_->button));
		break;

	case InputState::Trigger:
		isInput_ = input_->GetMouseTrigger(static_cast<uint32_t>(param_->button));
		break;

	case InputState::Release:
		isInput_ = input_->GetMouseRelease(static_cast<uint32_t>(param_->button));
		break;
	}
}