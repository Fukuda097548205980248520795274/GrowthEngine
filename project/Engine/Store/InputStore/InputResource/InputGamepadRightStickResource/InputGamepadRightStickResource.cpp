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
	isPrevRecordInput_ = isRecordInput_;
	isRecordInput_ = false;

	if (param_->direction.Length() == 0.0f)
	{
		// 方向が指定されていないとき

		// 入力状態
		switch (inputState_)
		{
		case InputState::Press:

			// 内積で判定
			if (input_->GetGamepadRightStick(param_->controller).Length() >= param_->dot)
				isInput_ = true;

			break;

		case InputState::Trigger:

			// 内積で判定
			if (input_->GetGamepadRightStick(param_->controller).Length() >= param_->dot)
				isRecordInput_ = true;

			// 前回の入力が記録されていなくて、今回の入力が記録された場合は、入力されたと判定する
			if (isRecordInput_ && !isPrevRecordInput_)
				isInput_ = true;

			break;

		case InputState::Release:

			// 内積で判定
			if (input_->GetGamepadRightStick(param_->controller).Length() >= param_->dot)
				isRecordInput_ = true;

			// 前回の入力が記録されていなくて、今回の入力が記録された場合は、入力されたと判定する
			if (isRecordInput_ && !isPrevRecordInput_)
				isInput_ = true;

			break;
		}
	}
	else
	{
		// 方向が指定されているとき

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

			// 内積で判定
			if (dot >= param_->dot)
				isRecordInput_ = true;

			// 前回の入力が記録されていなくて、今回の入力が記録された場合は、入力されたと判定する
			if (isRecordInput_ && !isPrevRecordInput_)
				isInput_ = true;

			break;

		case InputState::Release:

			// 内積で判定
			if (dot >= param_->dot)
				isRecordInput_ = true;

			// 前回の入力が記録されていて、今回の入力が記録されていない場合は、入力されたと判定する
			if (!isRecordInput_ && isPrevRecordInput_)
				isInput_ = true;

			break;
		}
	}
}