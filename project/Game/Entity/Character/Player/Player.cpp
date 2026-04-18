#include "Player.h"

#include <cmath>

/// @brief コンストラクタ
/// @param initData 
Player::Player(const InitData& initData) : Character(initData)
{
	// タグを指定する
	characterTag_ = CharacterTag::PlayerSide;
}

/// @brief 初期化
void Player::Initialize()
{
	assert(model_);


	// モデルをワールドトランスフォームの子にする
	model_->SetParent(worldTransform_.get());


	// 移動入力の生成
	inputMove_ = std::make_unique<InputGamepadLeftStick>("Player_Move", InputState::Press, 0, Vector2(0.0f, 0.0f), 0.5f);

	// 回避入力の生成
	inputAvoid_ = std::make_unique<InputGamepadButton>("Player_Avoid", InputState::Trigger, 0, XINPUT_GAMEPAD_A);

	// 弱攻撃入力の生成
	inputLightAttack_ = std::make_unique<InputGamepadButton>("Player_LightAttack", InputState::Trigger, 0, XINPUT_GAMEPAD_X);

	// 強攻撃入力の生成
	inputHeavyAttack_ = std::make_unique<InputGamepadButton>("Player_HeavyAttack", InputState::Trigger, 0, XINPUT_GAMEPAD_Y);

	// 構え入力の生成
	inputStance_ = std::make_unique<InputGamepadButton>("Player_Stance", InputState::Press, 0, XINPUT_GAMEPAD_RIGHT_SHOULDER);

	// キーの前移動入力の生成
	keyFrontMove_ = std::make_unique<InputKey>("Player_KeyFrontMove", InputState::Press, DIK_W);

	// キーの後移動入力の生成
	keyBackMove_ = std::make_unique<InputKey>("Player_KeyBackMove", InputState::Press, DIK_S);

	// キーの左移動入力の生成
	keyLeftMove_ = std::make_unique<InputKey>("Player_KeyLeftMove", InputState::Press, DIK_A);

	// キーの右移動入力の生成
	keyRightMove_ = std::make_unique<InputKey>("Player_KeyRightMove", InputState::Press, DIK_D);

	// キーの構え入力の生成
	keyStance_ = std::make_unique<InputKey>("Player_KeyStance", InputState::Press, DIK_SPACE);
}

/// @brief 更新処理
void Player::Update()
{
    // 通常時の移動速度[m/s]
	constexpr float kNormalMoveSpeed = 6.0f;
	constexpr float kStanceMoveSpeed = kNormalMoveSpeed * 0.5f;

	// 構え入力中は構えフラグを立て、離したらフラグを下ろす
	const bool isGamepadStance = (inputStance_ && inputStance_->IsInput());
	const bool isKeyStance = (keyStance_ && keyStance_->IsInput());
	isStance_ = (isGamepadStance || isKeyStance);

	// 構え中は移動速度を半分にする
	// Character側で速度補間しているため、通常速度↔構え速度の切り替えも補間される
	const float moveSpeed = isStance_ ? kStanceMoveSpeed : kNormalMoveSpeed;

	// WASDキーの入力方向を作成する
	Vector2 keyMoveDirection = Vector2(0.0f, 0.0f);
	if (keyFrontMove_ && keyFrontMove_->IsInput())
	{
		keyMoveDirection.y += 1.0f;
	}
	if (keyBackMove_ && keyBackMove_->IsInput())
	{
		keyMoveDirection.y -= 1.0f;
	}
	if (keyLeftMove_ && keyLeftMove_->IsInput())
	{
		keyMoveDirection.x -= 1.0f;
	}
	if (keyRightMove_ && keyRightMove_->IsInput())
	{
		keyMoveDirection.x += 1.0f;
	}

	// キー入力がある場合はキー移動を優先する
	if (keyMoveDirection.Length() > 0.0f)
	{
        SetMoveInputXZ(keyMoveDirection.Normalize(), moveSpeed);
	}
	else if (inputMove_ && inputMove_->param_)
	{
		if (inputMove_->IsInput())
		{
			// 左スティックの入力を取得する
			const Vector2 stick = GrowthEngine::GetInstance()->GetGamepadLeftStick(inputMove_->param_->controller);
           SetMoveInputXZ(stick.Normalize(), moveSpeed);
		}
		else
		{
			// 左スティックが入力されていない場合は移動を停止する
           SetMoveInputXZ(Vector2(0.0f, 0.0f), moveSpeed);
		}
	}
	else
	{
		// 入力がない場合は移動を停止する
		SetMoveInputXZ(Vector2(0.0f, 0.0f), moveSpeed);
	}

	// 基底クラスの更新
	Character::Update();
}

/// @brief 描画処理
void Player::Draw()
{
	assert(model_);

	// モデルを描画する
	model_->Draw();
}