#include "BossHP.h"

/// @brief デストラクタ
BossHP::~BossHP()
{
	// 後ろ側のhpのスプライトを削除する
	if (hpBackLeftSprite_)hpBackLeftSprite_->isDelete_ = true;
	hpBackLeftSprite_ = nullptr;

	if (hpBackMiddleSprite_)hpBackMiddleSprite_->isDelete_ = true;
	hpBackMiddleSprite_ = nullptr;

	if (hpBackRightSprite_)hpBackRightSprite_->isDelete_ = true;
	hpBackRightSprite_ = nullptr;
}

/// @brief 初期化
/// @param initData 
void BossHP::Initialize(const InitData& initData)
{
	// nullptrチェック
	assert(initData.hpFrameLeftSprite && "hp枠の左のスプライトが設定されていません。");
	assert(initData.hpFrameMiddleSprite && "hp枠の真ん中のスプライトが設定されていません。");
	assert(initData.hpFrameRightSprite && "hp枠の右のスプライトが設定されていません。");
	assert(initData.hpFrontLeftSprite && "hpの左のスプライトが設定されていません。");
	assert(initData.hpFrontMiddleSprite && "hpの真ん中のスプライトが設定されていません。");
	assert(initData.hpFrontRightSprite && "hpの右のスプライトが設定されていません。");
	assert(initData.hpBackLeftSprite && "後ろ側hpの左のスプライトが設定されていません。");
	assert(initData.hpBackMiddleSprite && "後ろ側hpの真ん中のスプライトが設定されていません。");
	assert(initData.hpBackRightSprite && "後ろ側hpの右のスプライトが設定されていません。");
	assert(initData.delayHpBackLeftSprite && "遅延hpの左のスプライトが設定されていません。");
	assert(initData.delayHpBackMiddleSprite && "遅延hpの真ん中のスプライトが設定されていません。");
	assert(initData.delayHpBackRightSprite && "遅延hpの右のスプライトが設定されていません。");
	assert(initData.delayHpFrontLeftSprite && "遅延hpの左のスプライトが設定されていません。");
	assert(initData.delayHpFrontMiddleSprite && "遅延hpの真ん中のスプライトが設定されていません。");
	assert(initData.delayHpFrontRightSprite && "遅延hpの右のスプライトが設定されていません。");

	// 横幅を設定する
	width_ = std::max(initData.width, 0);

	// 色を設定する
	color_ = Vector3(0.0f, 0.0f, 0.0f);
	alpha_ = std::clamp(initData.alpha, 0.0f, 1.0f);

	// hp枠のスプライトを設定する
	hpFrameLeftSprite_ = initData.hpFrameLeftSprite;
	hpFrameMiddleSprite_ = initData.hpFrameMiddleSprite;
	hpFrameRightSprite_ = initData.hpFrameRightSprite;
	hpLeftSprite_ = initData.hpFrontLeftSprite;
	hpMiddleSprite_ = initData.hpFrontMiddleSprite;
	hpRightSprite_ = initData.hpFrontRightSprite;
	hpBackLeftSprite_ = initData.hpBackLeftSprite;
	hpBackMiddleSprite_ = initData.hpBackMiddleSprite;
	hpBackRightSprite_ = initData.hpBackRightSprite;
	delayHpLeftSprite_ = initData.delayHpBackLeftSprite;
	delayHpMiddleSprite_ = initData.delayHpBackMiddleSprite;
	delayHpRightSprite_ = initData.delayHpBackRightSprite;
	delayHpFrontLeftSprite_ = initData.delayHpFrontLeftSprite;
	delayHpFrontMiddleSprite_ = initData.delayHpFrontMiddleSprite;
	delayHpFrontRightSprite_ = initData.delayHpFrontRightSprite;

	// ワールドトランスフォームを設定する
	worldTransform_->translate_ = initData.position;
	worldTransform_->scale_ = initData.scale;

	// 親を設定する
	hpFrameMiddleSprite_->param_.parent = worldTransform_.get();
	hpFrameLeftSprite_->param_.parent = worldTransform_.get();
	hpFrameRightSprite_->param_.parent = worldTransform_.get();
	hpMiddleSprite_->param_.parent = worldTransform_.get();
	hpLeftSprite_->param_.parent = worldTransform_.get();
	hpRightSprite_->param_.parent = worldTransform_.get();
	hpBackMiddleSprite_->param_.parent = worldTransform_.get();
	hpBackLeftSprite_->param_.parent = worldTransform_.get();
	hpBackRightSprite_->param_.parent = worldTransform_.get();
	delayHpLeftSprite_->param_.parent = worldTransform_.get();
	delayHpMiddleSprite_->param_.parent = worldTransform_.get();
	delayHpRightSprite_->param_.parent = worldTransform_.get();
	delayHpFrontLeftSprite_->param_.parent = worldTransform_.get();
	delayHpFrontMiddleSprite_->param_.parent = worldTransform_.get();
	delayHpFrontRightSprite_->param_.parent = worldTransform_.get();


	// HPの割合を計算する
	float hpRate = 1.0f;

	// 遅延HPの割合を計算する
	float delayHpRate = 1.0f;

	// HPのスケールと位置を設定する
	float hpBarWidth = static_cast<float>(width_ - 6) / 2.0f;
	float hpWidth = hpBarWidth * hpRate;
	float delayHpWidth = hpBarWidth * delayHpRate;

	// スケールと位置を設定する
	hpFrameMiddleSprite_->param_.transform.scale.x = static_cast<float>(width_ / 2);
	hpFrameLeftSprite_->param_.transform.translate.x = -hpFrameMiddleSprite_->param_.transform.scale.x;
	hpFrameRightSprite_->param_.transform.translate.x = hpFrameMiddleSprite_->param_.transform.scale.x;

	// 後ろ側HPのスケールと位置を設定する
	hpBackLeftSprite_->param_.transform.translate.x = -hpBarWidth;
	hpBackMiddleSprite_->param_.transform.translate.x = hpBackLeftSprite_->param_.transform.translate.x;
	hpBackMiddleSprite_->param_.transform.scale.x = hpBarWidth;
	hpBackRightSprite_->param_.transform.translate.x = hpBackMiddleSprite_->param_.transform.translate.x + hpBackMiddleSprite_->param_.transform.scale.x * 2.0f;

	// 前側HPのスケールと位置を設定する
	hpLeftSprite_->param_.transform.translate.x = -hpBarWidth;
	hpMiddleSprite_->param_.transform.translate.x = hpLeftSprite_->param_.transform.translate.x;
	hpMiddleSprite_->param_.transform.scale.x = hpWidth;
	hpRightSprite_->param_.transform.translate.x = hpMiddleSprite_->param_.transform.translate.x + hpMiddleSprite_->param_.transform.scale.x * 2.0f;

	// 遅延HPのスケールと位置を設定する
	delayHpLeftSprite_->param_.transform.translate.x = -hpBarWidth;
	delayHpMiddleSprite_->param_.transform.translate.x = delayHpLeftSprite_->param_.transform.translate.x;
	delayHpMiddleSprite_->param_.transform.scale.x = delayHpWidth;
	delayHpRightSprite_->param_.transform.translate.x = delayHpMiddleSprite_->param_.transform.translate.x + delayHpMiddleSprite_->param_.transform.scale.x * 2.0f;


	// 色を設定する
	hpFrameLeftSprite_->param_.material.color.w = alpha_;
	hpFrameMiddleSprite_->param_.material.color.w = alpha_;
	hpFrameRightSprite_->param_.material.color.w = alpha_;
	hpLeftSprite_->param_.material.color = Vector4(color_.x, color_.y, color_.z, alpha_);
	hpMiddleSprite_->param_.material.color = Vector4(color_.x, color_.y, color_.z, alpha_);
	hpRightSprite_->param_.material.color = Vector4(color_.x, color_.y, color_.z, alpha_);
	delayHpLeftSprite_->param_.material.color = Vector4(0.1f, 0.1f, 0.1f, alpha_);
	delayHpMiddleSprite_->param_.material.color = Vector4(0.1f, 0.1f, 0.1f, alpha_);
	delayHpRightSprite_->param_.material.color = Vector4(0.1f, 0.1f, 0.1f, alpha_);
	delayHpFrontLeftSprite_->param_.material.color = Vector4(0.1f, 0.1f, 0.1f, alpha_);
	delayHpFrontMiddleSprite_->param_.material.color = Vector4(0.1f, 0.1f, 0.1f, alpha_);
	delayHpFrontRightSprite_->param_.material.color = Vector4(0.1f, 0.1f, 0.1f, alpha_);

	// ワールドトランスフォームを更新する
	worldTransform_->Update();
}

/// @brief 更新処理
void BossHP::Update()
{
	if (isChanged_)
	{
		// 体力変化タイマーを減少させる
		changeTimer_ -= engine_->GetDeltaTime();

		// 遅延して減少する体力を減少させる
		if (changeTimer_ <= 0.0f)
			isChanged_ = false;
	}
	else
	{
		// 遅延して減少する体力を現在の体力に近づける
		float t = 0.05f;
		delayHP_ = static_cast<int>(static_cast<float>(delayHP_) * (1.0f - t) + static_cast<float>(currentHP_) * t);
	}


	// スケールと位置を設定する
	hpFrameMiddleSprite_->param_.transform.scale.x = static_cast<float>(width_ / 2);
	hpFrameLeftSprite_->param_.transform.translate.x = -hpFrameMiddleSprite_->param_.transform.scale.x;
	hpFrameRightSprite_->param_.transform.translate.x = hpFrameMiddleSprite_->param_.transform.scale.x;

	int32_t oneGageCurrentHp = currentHP_ % kOneGageHp;
	int32_t oneGageDelayHp = delayHP_ % kOneGageHp;

	// HPの割合を計算する
	float hpRate = 0.0f;
	if (maxHP_ > 0) hpRate = static_cast<float>(oneGageCurrentHp) / static_cast<float>(kOneGageHp);

	// 遅延HPの割合を計算する
	float delayHpRate = 0.0f;
	if (maxHP_ > 0) delayHpRate = static_cast<float>(oneGageDelayHp) / static_cast<float>(kOneGageHp);

	// HPのスケールと位置を設定する
	float hpBarWidth = static_cast<float>(width_ - 6) / 2.0f;
	float hpWidth = hpBarWidth * hpRate;
	float delayHpFrontWidth = hpBarWidth * delayHpRate;

	// 後ろ側HPのスケールと位置を設定する
	hpBackLeftSprite_->param_.transform.translate.x = -hpBarWidth;
	hpBackMiddleSprite_->param_.transform.translate.x = hpBackLeftSprite_->param_.transform.translate.x;
	hpBackMiddleSprite_->param_.transform.scale.x = hpBarWidth;
	hpBackRightSprite_->param_.transform.translate.x = hpBackMiddleSprite_->param_.transform.translate.x + hpBackMiddleSprite_->param_.transform.scale.x * 2.0f;

	// 前側HPのスケールと位置を設定する
	hpLeftSprite_->param_.transform.translate.x = -hpBarWidth;
	hpMiddleSprite_->param_.transform.translate.x = hpLeftSprite_->param_.transform.translate.x;
	hpMiddleSprite_->param_.transform.scale.x = hpWidth;
	hpRightSprite_->param_.transform.translate.x = hpMiddleSprite_->param_.transform.translate.x + hpMiddleSprite_->param_.transform.scale.x * 2.0f;

	// 遅延HPのスケールと位置を設定する
	delayHpLeftSprite_->param_.transform.translate.x = -hpBarWidth;
	delayHpMiddleSprite_->param_.transform.translate.x = delayHpLeftSprite_->param_.transform.translate.x;
	if (delayHP_ / kOneGageHp > currentHP_ / kOneGageHp)delayHpMiddleSprite_->param_.transform.scale.x = hpBarWidth;
	else delayHpMiddleSprite_->param_.transform.scale.x = delayHpFrontWidth;
	delayHpRightSprite_->param_.transform.translate.x = delayHpMiddleSprite_->param_.transform.translate.x + delayHpMiddleSprite_->param_.transform.scale.x * 2.0f;

	// 前側の遅延HPのスケールと位置を設定する
	delayHpFrontLeftSprite_->param_.transform.translate.x = -hpBarWidth;
	delayHpFrontMiddleSprite_->param_.transform.translate.x = delayHpFrontLeftSprite_->param_.transform.translate.x;
	delayHpFrontMiddleSprite_->param_.transform.scale.x = delayHpFrontWidth;
	delayHpFrontRightSprite_->param_.transform.translate.x = delayHpFrontMiddleSprite_->param_.transform.translate.x + delayHpFrontMiddleSprite_->param_.transform.scale.x * 2.0f;

	float alpha = alpha_;
	if (isDeath_)
	{
		// 死亡時は徐々に透明にする
		deathTimer_ -= engine_->GetDeltaTime();
		deathTimer_ = std::max(deathTimer_, 0.0f);

		float t = deathTimer_ / kDeathTime;
		alpha = alpha_ * t;

		// 死亡タイマーが0以下になったら削除する
		if (deathTimer_ <= 0.0f)
		{
			alpha = 0.0f;
			Delete();
		}
	}

	// 今のゲージに色を合わせる
	if (currentHP_ > 0)
	{
		// 色のインデックスを計算する
		int colorIndex = currentHP_ / kOneGageHp;
		if (colorIndex >= 12)colorIndex = (colorIndex % 2) + 10;
		color_ = kGageColor[colorIndex];

		// 後ろ側のゲージの色を設定する
		int backColorIndex = colorIndex - 1;
		if (backColorIndex >= 0)
		{
			if (backColorIndex >= 12)backColorIndex = (backColorIndex % 2) + 10;
			backColor_ = kGageColor[backColorIndex];
		}
	}

	// 色を設定する
	hpFrameLeftSprite_->param_.material.color.w = alpha;
	hpFrameMiddleSprite_->param_.material.color.w = alpha;
	hpFrameRightSprite_->param_.material.color.w = alpha;
	hpBackLeftSprite_->param_.material.color = Vector4(backColor_.x, backColor_.y, backColor_.z, alpha);
	hpBackMiddleSprite_->param_.material.color = Vector4(backColor_.x, backColor_.y, backColor_.z, alpha);
	hpBackRightSprite_->param_.material.color = Vector4(backColor_.x, backColor_.y, backColor_.z, alpha);
	hpLeftSprite_->param_.material.color = Vector4(color_.x, color_.y, color_.z, alpha);
	hpMiddleSprite_->param_.material.color = Vector4(color_.x, color_.y, color_.z, alpha);
	hpRightSprite_->param_.material.color = Vector4(color_.x, color_.y, color_.z, alpha);
	delayHpLeftSprite_->param_.material.color = Vector4(0.1f, 0.1f, 0.1f, alpha);
	delayHpMiddleSprite_->param_.material.color = Vector4(0.1f, 0.1f, 0.1f, alpha);
	delayHpRightSprite_->param_.material.color = Vector4(0.1f, 0.1f, 0.1f, alpha);
	delayHpFrontLeftSprite_->param_.material.color = Vector4(0.1f, 0.1f, 0.1f, alpha);
	delayHpFrontMiddleSprite_->param_.material.color = Vector4(0.1f, 0.1f, 0.1f, alpha);
	delayHpFrontRightSprite_->param_.material.color = Vector4(0.1f, 0.1f, 0.1f, alpha);

	// 基底クラスの更新処理を呼び出す
	HUD::Update();
}

/// @brief 描画処理
void BossHP::Draw()
{
	// 表示されていない場合は描画しない
	if (!isVisible_)return;

	// hp枠の描画
	hpFrameMiddleSprite_->Draw();
	hpFrameLeftSprite_->Draw();
	hpFrameRightSprite_->Draw();

	// 後ろ側hpの描画
	if (currentHP_ / kOneGageHp - 1 >= 0)
	{
		hpBackMiddleSprite_->Draw();
		hpBackLeftSprite_->Draw();
		hpBackRightSprite_->Draw();
	}

	// 遅延hpの描画
	delayHpMiddleSprite_->Draw();
	if (delayHP_ > 0)
	{
		delayHpLeftSprite_->Draw();
		delayHpRightSprite_->Draw();
	}

	// hpの描画
	hpMiddleSprite_->Draw();
	if (currentHP_ > 0)
	{
		hpLeftSprite_->Draw();
		hpRightSprite_->Draw();
	}

	// 遅延hpの描画
	if (delayHP_ > 0)
	{
		// 今のゲージ数
		int currentNumGage = currentHP_ / kOneGageHp + 1;
		int currentNumDelayGage = delayHP_ / kOneGageHp + 1;
		if (currentNumDelayGage > currentNumGage)
		{
			delayHpFrontMiddleSprite_->Draw();
			delayHpFrontLeftSprite_->Draw();
			delayHpFrontRightSprite_->Draw();
		}
	}
}