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
}

/// @brief 更新処理
void Player::Update()
{
	if (inputMove_ && inputMove_->param_)
	{
		if (inputMove_->IsInput())
		{
			// 左スティックの入力を取得する
			const Vector2 stick = GrowthEngine::GetInstance()->GetGamepadLeftStick(inputMove_->param_->controller);
			constexpr float kMaxMoveSpeed = 0.2f;
			SetMoveInputXZ(stick.Normalize(), kMaxMoveSpeed);
		}
		else
		{
			// 左スティックが入力されていない場合は移動を停止する
			SetMoveInputXZ(Vector2(0.0f, 0.0f), 0.2f);
		}
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