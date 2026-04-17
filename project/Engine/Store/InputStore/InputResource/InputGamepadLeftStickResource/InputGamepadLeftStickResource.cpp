#include "InputGamepadLeftStickResource.h"

/// @brief コンストラクタ
/// @param inputState 
/// @param hInput 
/// @param name 
/// @param button 
Engine::InputGamepadLeftStickResource::InputGamepadLeftStickResource(InputState inputState, InputHandle hInput, const std::string& name, DWORD controller, const Vector2& direction, float dot)
	: InputBaseResource(inputState, hInput, name)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<InputData::GamepadStickParam>();
	param_->controller = controller;
	param_->direction = direction.Normalize();
	param_->dot = dot;
}

/// @brief 更新処理
void Engine::InputGamepadLeftStickResource::Update()
{
	// 入力を初期化する
	isInput_ = false;

	if (param_->direction.Length() == 0.0f)
	{
		// 方向が指定されていないとき

		// 入力状態
		switch (inputState_)
		{
		case InputState::Press:

			// 内積で判定
			if (input_->GetGamepadLeftStick(param_->controller).Length() >= param_->dot)
				isInput_ = true;

			break;

		case InputState::Trigger:

			break;

		case InputState::Release:

			break;
		}
	}
	else
	{
		// 方向が指定されているとき

		// 入力方向と目標方向の内積
		float dot = Dot(input_->GetGamepadLeftStick(param_->controller), param_->direction);

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
}