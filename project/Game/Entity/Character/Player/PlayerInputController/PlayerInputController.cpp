#include "PlayerInputController.h"

/// @brief コンストラクタ
PlayerInputController::PlayerInputController()
{

}

/// @brief 初期化
void PlayerInputController::Initialize()
{
	// 移動入力の生成
	inputMove_ = std::make_unique<InputGamepadLeftStick>("Player_Move", InputState::Press, 0, Vector2(0.0f, 0.0f), 0.5f);

	// ダッシュ入力の生成
	inputDash_ = std::make_unique<InputGamepadButton>("Player_Dash", InputState::Trigger, 0, XINPUT_GAMEPAD_RIGHT_SHOULDER);

	// 回避入力の生成
	inputAvoid_ = std::make_unique<InputGamepadButton>("Player_Avoid", InputState::Trigger, 0, XINPUT_GAMEPAD_A);

	// X攻撃入力の生成
	inputXAttack_ = std::make_unique<InputGamepadButton>("Player_XAttack", InputState::Trigger, 0, XINPUT_GAMEPAD_X);

	// Y攻撃入力の生成
	inputYAttack_ = std::make_unique<InputGamepadButton>("Player_YAttack", InputState::Trigger, 0, XINPUT_GAMEPAD_Y);

	// B攻撃入力の生成
	inputBAttack_ = std::make_unique<InputGamepadButton>("Player_BAttack", InputState::Trigger, 0, XINPUT_GAMEPAD_B);

	// 防御入力の生成
	inputGuard_ = std::make_unique<InputGamepadButton>("Player_Guard", InputState::Press, 0, XINPUT_GAMEPAD_LEFT_SHOULDER);

	// 掴まれ解き入力の生成
	inputEscapeMash_ = std::make_unique<InputGamepadButton>("Player_EscapeMash", InputState::Trigger, 0, XINPUT_GAMEPAD_A);

	// スタイルチェンジ入力の生成
	inputStyleChange_ = std::make_unique<InputGamepadButton>("Player_StyleChange", InputState::Trigger, 0, XINPUT_GAMEPAD_DPAD_DOWN);

	// レイジモード入力の生成
	inputRageMode_ = std::make_unique<InputGamepadRightTrigger>("Player_RageMode", InputState::Trigger, 0, 0.5f);


	// キーの前移動入力の生成
	keyFrontMove_ = std::make_unique<InputKey>("Player_KeyFrontMove", InputState::Press, DIK_W);

	// キーの後移動入力の生成
	keyBackMove_ = std::make_unique<InputKey>("Player_KeyBackMove", InputState::Press, DIK_S);

	// キーの左移動入力の生成
	keyLeftMove_ = std::make_unique<InputKey>("Player_KeyLeftMove", InputState::Press, DIK_A);

	// キーの右移動入力の生成
	keyRightMove_ = std::make_unique<InputKey>("Player_KeyRightMove", InputState::Press, DIK_D);
}


/// @brief ダッシュ入力があったかどうかを取得する
/// @return 
bool PlayerInputController::IsDashRequested() const
{
	return inputDash_ && inputDash_->IsInput();
}

/// @brief 回避入力があったかどうかを取得する
/// @return 
bool PlayerInputController::IsAvoidRequested() const
{
	return inputAvoid_ && inputAvoid_->IsInput();
}

/// @brief X攻撃入力があったかどうかを取得する
/// @return 
bool PlayerInputController::IsInputXAttackRequested() const
{
	return inputXAttack_ && inputXAttack_->IsInput();
}

/// @brief Y攻撃入力があったかどうかを取得する
/// @return 
bool PlayerInputController::IsInputYAttackRequested() const
{
	return inputYAttack_ && inputYAttack_->IsInput();
}

/// @brief B攻撃入力があったかどうかを取得する
/// @return 
bool PlayerInputController::IsInputBAttackRequested() const
{
	return inputBAttack_ && inputBAttack_->IsInput();
}

/// @brief 防御入力があったかどうかを取得する
/// @return 
bool PlayerInputController::IsGuardRequested() const
{
	return inputGuard_ && inputGuard_->IsInput();
}

/// @brief 掴まれ解き入力があったかどうかを取得する
/// @return 
bool PlayerInputController::IsEscapeMashRequested() const
{
	return inputEscapeMash_ && inputEscapeMash_->IsInput();
}

/// @brief スタイルチェンジ入力があったかどうかを取得する
/// @return 
bool PlayerInputController::IsStyleChangeRequested() const
{
	return inputStyleChange_ && inputStyleChange_->IsInput();
}

/// @brief レイジモード入力があったかどうかを取得する
/// @return 
bool PlayerInputController::IsRageModeRequested() const
{
	return inputRageMode_ && inputRageMode_->IsInput();
}

/// @brief 移動入力方向を取得する
/// @param outHasMoveInput 
/// @return 
Vector2 PlayerInputController::GetMoveDirection(bool& outHasMoveInput) const
{
	// 移動入力があるかどうかを初期化する
	outHasMoveInput = false;
	Vector2 keyMoveDirection(0.0f, 0.0f);

	// キー入力の方向を取得する
	if (keyFrontMove_ && keyFrontMove_->IsInput()) keyMoveDirection.y += 1.0f;
	if (keyBackMove_ && keyBackMove_->IsInput()) keyMoveDirection.y -= 1.0f;
	if (keyLeftMove_ && keyLeftMove_->IsInput()) keyMoveDirection.x -= 1.0f;
	if (keyRightMove_ && keyRightMove_->IsInput()) keyMoveDirection.x += 1.0f;

	// キー入力がある場合はキー入力を優先する
	if (keyMoveDirection.Length() > 0.0f)
	{
		outHasMoveInput = true;
		return keyMoveDirection.Normalize();
	}

	// 左スティックの入力を取得する
	if (inputMove_ && inputMove_->param_ && inputMove_->IsInput())
	{
		const Vector2 stick = GrowthEngine::GetInstance()->GetGamepadLeftStick(inputMove_->param_->controller);
		if (stick.Length() > 0.0f)
		{
			outHasMoveInput = true;
			return stick.Normalize();
		}
	}

	// 入力がない場合はゼロベクトルを返す
	return Vector2(0.0f, 0.0f);
}