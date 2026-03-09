#include "InputGamepadRightStickResource.h"

/// @brief コンストラクタ
/// @param inputState 
/// @param hInput 
/// @param name 
/// @param button 
Engine::InputGamepadRightStickResource::InputGamepadRightStickResource(InputState inputState, InputHandle hInput, const std::string& name, DWORD controller, const Vector2& direction, float dot)
	: InputBaseResource(inputState, hInput, name)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<InputData::GamepadStickParam>();
	param_->controller = controller;
	param_->direction = direction.Normalize();
	param_->dot = dot;
}

/// @brief 更新処理
void Engine::InputGamepadRightStickResource::Update()
{
	// 入力を初期化する
	isInput_ = false;

	// 入力方向と目標方向の内積
	float dot = Dot(input_->GetGamepadRightStick(param_->controller), param_->direction);

	// 入力状態
	switch (inputState_)
	{
	case InputState::Press:

		// 内積で判定
		if (dot >= param_->dot)
			isInput_ = true;

		break;

	case InputState::Trigger:

		break;

	case InputState::Release:

		break;
	}

}