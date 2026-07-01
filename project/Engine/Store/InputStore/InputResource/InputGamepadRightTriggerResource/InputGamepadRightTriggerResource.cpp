#include "InputGamepadRightTriggerResource.h"

/// @brief コンストラクタ
/// @param inputState 
/// @param hInput 
/// @param name 
/// @param controller 
/// @param threshold 
Engine::InputGamepadRightTriggerResource::InputGamepadRightTriggerResource(InputState inputState, InputHandle hInput, const std::string& name, DWORD controller, float threshold)
	: InputBaseResource(inputState, hInput, name)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<InputData::GamepadTriggerParam>();
	param_->controller = controller;
	param_->threshold = threshold;
}

/// @brief 更新処理
void Engine::InputGamepadRightTriggerResource::Update()
{
	// 入力を初期化する
	isInput_ = false;
	isPrevRecordInput_ = isRecordInput_;
	isRecordInput_ = false;

	// 入力状態
	switch (inputState_)
	{
	case InputState::Press:

		// 閾値を越えたかどうか
		if (input_->GetGamepadRightTrigger(param_->controller) >= param_->threshold)
			isInput_ = true;

		break;

	case InputState::Trigger:

		// 閾値を越えたかどうか
		if (input_->GetGamepadRightTrigger(param_->controller) >= param_->threshold)
			isRecordInput_ = true;

		// 前回の入力が記録されていなくて、今回の入力が記録された場合は、入力されたと判定する
		if (isRecordInput_ && !isPrevRecordInput_)
			isInput_ = true;

		break;

	case InputState::Release:

		// 閾値を越えたかどうか
		if (input_->GetGamepadRightTrigger(param_->controller) >= param_->threshold)
			isRecordInput_ = true;

		// 前回の入力が記録されていて、今回の入力が記録されていない場合は、入力されたと判定する
		if (!isRecordInput_ && isPrevRecordInput_)
			isInput_ = true;

		break;
	}
}