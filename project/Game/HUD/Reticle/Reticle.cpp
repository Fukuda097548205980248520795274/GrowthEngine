#include "Reticle.h"
#include <numbers>

/// @brief デストラクタ
Reticle::~Reticle()
{
	if (leftUpFrame_)
	{
		leftUpFrame_->isDelete_ = true;
		leftUpFrame_ = nullptr;
	}

	if (rightUpFrame_)
	{
		rightUpFrame_->isDelete_ = true;
		rightUpFrame_ = nullptr;
	}

	if (leftDownFrame_)
	{
		leftDownFrame_->isDelete_ = true;
		leftDownFrame_ = nullptr;
	}

	if (rightDownFrame_)
	{
		rightDownFrame_->isDelete_ = true;
		rightDownFrame_ = nullptr;
	}
}

/// @brief 初期化処理
/// @param leftUpFrame 
/// @param rightUpFrame 
/// @param leftDownFrame 
/// @param rightDownFrame 
/// @param centerPoint 
void Reticle::Initialize(PrefabInstanceSprite* leftUpFrame, PrefabInstanceSprite* rightUpFrame, PrefabInstanceSprite* leftDownFrame, PrefabInstanceSprite* rightDownFrame,
	Sprite* centerPoint)
{
	// nullptrチェック
	assert(leftUpFrame);
	assert(rightUpFrame);
	assert(leftDownFrame);
	assert(rightDownFrame);
	assert(centerPoint);

	// 引数を受け取る
	leftUpFrame_ = leftUpFrame;
	rightUpFrame_ = rightUpFrame;
	leftDownFrame_ = leftDownFrame;
	rightDownFrame_ = rightDownFrame;
	centerPoint_ = centerPoint;

	leftUpFrame_->param_.texture.anchor = Vector2(1.0f, 1.0f);

	rightUpFrame_->param_.texture.anchor = Vector2(1.0f, 1.0f);
	rightUpFrame_->param_.transform.rotate = -std::numbers::pi_v<float> / 2.0f;

	leftDownFrame_->param_.texture.anchor = Vector2(1.0f, 1.0f);
	leftDownFrame_->param_.transform.rotate = std::numbers::pi_v<float> / 2.0f;

	rightDownFrame_->param_.texture.anchor = Vector2(1.0f, 1.0f);
	rightDownFrame_->param_.transform.rotate = std::numbers::pi_v<float>;

	// 親を設定する
	leftUpFrame_->param_.parent = worldTransform_.get();
	rightUpFrame_->param_.parent = worldTransform_.get();
	leftDownFrame_->param_.parent = worldTransform_.get();
	rightDownFrame_->param_.parent = worldTransform_.get();
	centerPoint_->SetParent(worldTransform_.get());
}

/// @brief 更新処理
void Reticle::Update()
{
	isPrevLockOn_ = isLockOn_;
	isLockOn_ = false;

	// ロックオン対象が終了していたらロックオンを解除する
	if (!IsLockOn())
		lockOnTarget_ = nullptr;

	float easing = 0.0f;
	if (IsLockOn())
	{
		startLockOnTimer_ -= engine_->GetDeltaTime();
		startLockOnTimer_ = std::max(startLockOnTimer_, 0.0f);

		float t = 1.0f - (startLockOnTimer_ / kStartLockOnTime);
		easing = 1.0f - std::pow(1.0f - t, 3.0f);
	}
	else
	{
		startLockOnTimer_ += engine_->GetDeltaTime();
		startLockOnTimer_ = std::min(startLockOnTimer_, kStartLockOnTime);

		float t = 1.0f - (startLockOnTimer_ / kStartLockOnTime);
		easing = std::pow(t, 3.0f);
	}

	leftUpFrame_->param_.material.color.w = easing;
	rightUpFrame_->param_.material.color.w = easing;
	leftDownFrame_->param_.material.color.w = easing;
	rightDownFrame_->param_.material.color.w = easing;

	const float kFrameSpacing = 25.0f;

	leftUpFrame_->param_.transform.translate = Lerp(Vector2(-kFrameSpacing, kFrameSpacing) * 2.0f, Vector2(-kFrameSpacing, kFrameSpacing), easing);
	rightUpFrame_->param_.transform.translate = Lerp(Vector2(kFrameSpacing, kFrameSpacing) * 2.0f, Vector2(kFrameSpacing, kFrameSpacing), easing);
	leftDownFrame_->param_.transform.translate = Lerp(Vector2(-kFrameSpacing, -kFrameSpacing) * 2.0f, Vector2(-kFrameSpacing, -kFrameSpacing), easing);
	rightDownFrame_->param_.transform.translate = Lerp(Vector2(kFrameSpacing, -kFrameSpacing) * 2.0f, Vector2(kFrameSpacing, -kFrameSpacing), easing);

	centerPoint_->param_->transform.scale = Lerp(Vector2(0.0f, 0.0f), Vector2(0.25f, 0.25f), easing);
	centerPoint_->param_->material.color.w = 1.0f - easing;

	// 基底クラスの更新
	HUD::Update();
}

/// @brief 描画処理
void Reticle::Draw()
{
	leftUpFrame_->Draw();
	rightUpFrame_->Draw();
	leftDownFrame_->Draw();
	rightDownFrame_->Draw();
}

/// @brief ロックオンする
/// @param lockOnTarget 
void Reticle::LockOn(const Character* lockOnTarget)
{
	isLockOn_ = true;

	if (lockOnTarget_ != lockOnTarget)
	{
		startLockOnTimer_ = kStartLockOnTime;
		lockOnTarget_ = lockOnTarget;

		// ロックオンのSEを再生する
		SoundManager::GetInstance()->SeLockOn();
	}

	// 位置を設定する
	SetPosition(lockOnTarget->GetBonePosition(JointType::Head));
}