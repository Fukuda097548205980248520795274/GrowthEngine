#include "HP.h"
#include "Entity/Character/Character.h"

/// @brief デストラクタ
HP::~HP()
{
	// hp枠のスプライトを削除する
	if (hpFrameLeftSprite_)hpFrameLeftSprite_->isDelete_ = true;
	hpFrameLeftSprite_ = nullptr;

	if (hpFrameMiddleSprite_)hpFrameMiddleSprite_->isDelete_ = true;
	hpFrameMiddleSprite_ = nullptr;

	if (hpFrameRightSprite_)hpFrameRightSprite_->isDelete_ = true;
	hpFrameRightSprite_ = nullptr;

	// hpのスプライトを削除する
	if (hpLeftSprite_)hpLeftSprite_->isDelete_ = true;
	hpLeftSprite_ = nullptr;

	if (hpMiddleSprite_)hpMiddleSprite_->isDelete_ = true;
	hpMiddleSprite_ = nullptr;

	if (hpRightSprite_)hpRightSprite_->isDelete_ = true;
	hpRightSprite_ = nullptr;

	// 遅延hpのスプライトを削除する
	if (delayHpLeftSprite_)delayHpLeftSprite_->isDelete_ = true;
	delayHpLeftSprite_ = nullptr;

	if (delayHpMiddleSprite_)delayHpMiddleSprite_->isDelete_ = true;
	delayHpMiddleSprite_ = nullptr;

	if (delayHpRightSprite_)delayHpRightSprite_->isDelete_ = true;
	delayHpRightSprite_ = nullptr;
}

/// @brief 初期化
/// @param initData 
void HP::Initialize(const InitData& initData)
{
	// nullptrチェック
	assert(initData.hpFrameLeftSprite && "hp枠の左のスプライトが設定されていません。");
	assert(initData.hpFrameMiddleSprite && "hp枠の真ん中のスプライトが設定されていません。");
	assert(initData.hpFrameRightSprite && "hp枠の右のスプライトが設定されていません。");
	assert(initData.hpLeftSprite && "hpの左のスプライトが設定されていません。");
	assert(initData.hpMiddleSprite && "hpの真ん中のスプライトが設定されていません。");
	assert(initData.delayHpLeftSprite && "遅延hpの左のスプライトが設定されていません。");
	assert(initData.delayHpMiddleSprite && "遅延hpの真ん中のスプライトが設定されていません。");
	assert(initData.delayHpRightSprite && "遅延hpの右のスプライトが設定されていません。");
	assert(initData.hpRightSprite && "hpの右のスプライトが設定されていません。");

	// 横幅を設定する
	width_ = std::max(initData.width, 0);

	// 色を設定する
	color_ = initData.color;
	alpha_ = std::clamp(initData.alpha, 0.0f, 1.0f);

	// hp枠のスプライトを設定する
	hpFrameLeftSprite_ = initData.hpFrameLeftSprite;
	hpFrameMiddleSprite_ = initData.hpFrameMiddleSprite;
	hpFrameRightSprite_ = initData.hpFrameRightSprite;
	hpLeftSprite_ = initData.hpLeftSprite;
	hpMiddleSprite_ = initData.hpMiddleSprite;
	hpRightSprite_ = initData.hpRightSprite;
	delayHpLeftSprite_ = initData.delayHpLeftSprite;
	delayHpMiddleSprite_ = initData.delayHpMiddleSprite;
	delayHpRightSprite_ = initData.delayHpRightSprite;

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
	delayHpLeftSprite_->param_.parent = worldTransform_.get();
	delayHpMiddleSprite_->param_.parent = worldTransform_.get();
	delayHpRightSprite_->param_.parent = worldTransform_.get();

	// スケールと位置を設定する
	hpFrameMiddleSprite_->param_.transform.scale.x = static_cast<float>(width_ / 2);
	hpFrameLeftSprite_->param_.transform.translate.x = -hpFrameMiddleSprite_->param_.transform.scale.x;
	hpFrameRightSprite_->param_.transform.translate.x = hpFrameMiddleSprite_->param_.transform.scale.x;

	hpLeftSprite_->param_.transform.translate.x = -static_cast<float>((width_ - 6) / 2);
	hpMiddleSprite_->param_.transform.translate.x = hpLeftSprite_->param_.transform.translate.x;
	hpMiddleSprite_->param_.transform.scale.x = static_cast<float>((width_ - 6) / 2);
	hpRightSprite_->param_.transform.translate.x = hpMiddleSprite_->param_.transform.translate.x + hpMiddleSprite_->param_.transform.scale.x * 2.0f;

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

	// ワールドトランスフォームを更新する
	worldTransform_->Update();
}

/// @brief 更新処理
void HP::Update()
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

	// HPの割合を計算する
	float hpRate = 0.0f;
	if (maxHP_ > 0) hpRate = static_cast<float>(currentHP_) / static_cast<float>(maxHP_);

	// 遅延HPの割合を計算する
	float delayHpRate = 0.0f;
	if (maxHP_ > 0) delayHpRate = static_cast<float>(delayHP_) / static_cast<float>(maxHP_);

	// HPのスケールと位置を設定する
	float hpBarWidth = static_cast<float>(width_ - 6) / 2.0f;
	float hpWidth = hpBarWidth * hpRate;
	float delayHpWidth = hpBarWidth * delayHpRate;

	hpLeftSprite_->param_.transform.translate.x = -hpBarWidth;
	hpMiddleSprite_->param_.transform.translate.x = hpLeftSprite_->param_.transform.translate.x;
	hpMiddleSprite_->param_.transform.scale.x = hpWidth;
	hpRightSprite_->param_.transform.translate.x = hpMiddleSprite_->param_.transform.translate.x + hpMiddleSprite_->param_.transform.scale.x * 2.0f;

	delayHpLeftSprite_->param_.transform.translate.x = -hpBarWidth;
	delayHpMiddleSprite_->param_.transform.translate.x = delayHpLeftSprite_->param_.transform.translate.x;
	delayHpMiddleSprite_->param_.transform.scale.x = delayHpWidth;
	delayHpRightSprite_->param_.transform.translate.x = delayHpMiddleSprite_->param_.transform.translate.x + delayHpMiddleSprite_->param_.transform.scale.x * 2.0f;

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

	// 色を設定する
	hpFrameLeftSprite_->param_.material.color.w = alpha;
	hpFrameMiddleSprite_->param_.material.color.w = alpha;
	hpFrameRightSprite_->param_.material.color.w = alpha;
	hpLeftSprite_->param_.material.color = Vector4(color_.x, color_.y, color_.z, alpha);
	hpMiddleSprite_->param_.material.color = Vector4(color_.x, color_.y, color_.z, alpha);
	hpRightSprite_->param_.material.color = Vector4(color_.x, color_.y, color_.z, alpha);
	delayHpLeftSprite_->param_.material.color = Vector4(0.1f, 0.1f, 0.1f, alpha);
	delayHpMiddleSprite_->param_.material.color = Vector4(0.1f, 0.1f, 0.1f, alpha);
	delayHpRightSprite_->param_.material.color = Vector4(0.1f, 0.1f, 0.1f, alpha);

	// 基底クラスの更新処理を呼び出す
	HUD::Update();
}

/// @brief 描画処理
void HP::Draw()
{
	// 表示されていない場合は描画しない
	if (!isVisible_)return;

	// hp枠の描画
	hpFrameMiddleSprite_->Draw();
	hpFrameLeftSprite_->Draw();
	hpFrameRightSprite_->Draw();

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
}

/// @brief 現在の体力を設定する
/// @param hp 
void HP::SetCurrentHP(int hp)
{
	// 体力が変化したかどうかを判定する
	if (currentHP_ != hp)
	{ 
		// 遅延して減少する体力を設定する
		if (!isChanged_)delayHP_ = currentHP_;

		isChanged_ = true;

		// 体力変化タイマーをリセットする
		changeTimer_ = 1.0f;
	}

	// 体力を設定する
	currentHP_ = hp; 
}

/// @brief 体力バーを持つキャラクターが死亡したときの処理
void HP::Death()
{
	// 既に死亡している場合は処理しない
	if (isDeath_)return;

	// 死亡フラグを立てる
	isDeath_ = true;
}