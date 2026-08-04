#include "NavigationArrow.h"
#include "Entity/Character/Player/Player.h"

/// @brief コンストラクタ
/// @param player 
/// @param camera 
/// @param sprite 
void NavigationArrow::Initialize(Sprite* sprite)
{
	// nullptrチェック
	assert(sprite);

	// 引数を受け取る
	sprite_ = sprite;
	sprite_->SetParent(worldTransform_.get());
	sprite_->param_->screenAnchor = Engine::Render2D::ScreenAnchor::RightBottom;

	worldTransform_->translate_ = Vector2(-128.0f, 128.0f);
}

/// @brief 更新処理
void NavigationArrow::Update()
{
	// 目標位置が設定されていない場合はタイマーを減算する
	if (targetPosition_ == std::nullopt || !player_)
	{
		timer_ -= engine_->GetDeltaTime();
		timer_ = std::max(timer_, 0.0f);
		t_ = timer_ / kMaxTime;
		sprite_->param_->material.color.w = t_;

		HUD::Update();
		return;
	}

	// タイマーを加算する
	timer_ += engine_->GetDeltaTime();
	timer_ = std::min(timer_, kMaxTime);
	t_ = timer_ / kMaxTime;

	// プレイヤーと目標位置のワールド座標を取得する
	Vector3 playerPos = player_->GetWorldPosition();
	Vector3 targetPos = targetPosition_.value();

	// 目的地への水平ベクトルを計算
	Vector3 toTarget = targetPos - playerPos;
	toTarget.y = 0.0f;

	// 目的地へのワールド角度をラジアンで計算
	float targetYaw = std::atan2(-toTarget.x, toTarget.z);

	// カメラのY回転を取得
	float cameraYaw = player_->GetCameraYaw();

	// 矢印スプライトの最終的な回転角
	float arrowRotationRadian = targetYaw + cameraYaw;

	// 目標位置が設定されている場合はスプライトの色を更新する
	sprite_->param_->material.color.w = t_;
	sprite_->param_->transform.rotate = arrowRotationRadian;

	// 目標位置が設定されている場合は、目標位置をリセットする
	targetPosition_ = std::nullopt;

	HUD::Update();
}

/// @brief 描画処理
void NavigationArrow::Draw()
{
	/// @brief スプライトが存在する場合は描画する
	if (sprite_ && t_ > 0.0f)sprite_->Draw();
}