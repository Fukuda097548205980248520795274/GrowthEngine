#include "Gage.h"
#include "Entity/Character/Character.h"

/// @brief デストラクタ
Gage::~Gage()
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

	// 体力ゲージの区切りを削除する
	if (hpSeparatorSprite_)hpSeparatorSprite_->isDelete_ = true;
	hpSeparatorSprite_ = nullptr;
}

/// @brief 初期化
/// @param initData 
void Gage::Initialize(const InitData& initData)
{
	// nullptrチェック
	assert(initData.hpFrameLeftSprite && "hp枠の左のスプライトが設定されていません。");
	assert(initData.hpFrameMiddleSprite && "hp枠の真ん中のスプライトが設定されていません。");
	assert(initData.hpFrameRightSprite && "hp枠の右のスプライトが設定されていません。");
	assert(initData.hpLeftSprite && "hpの左のスプライトが設定されていません。");
	assert(initData.hpMiddleSprite && "hpの真ん中のスプライトが設定されていません。");
	assert(initData.hpRightSprite && "hpの右のスプライトが設定されていません。");
	assert(initData.hpSeparatorSprite && "体力ゲージの区切りのスプライトが設定されていません。");

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
	hpSeparatorSprite_ = initData.hpSeparatorSprite;

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
	hpSeparatorSprite_->param_.parent = worldTransform_.get();

	// スケールと位置を設定する
	hpFrameMiddleSprite_->param_.transform.scale.x = static_cast<float>(width_ / 2);
	hpFrameLeftSprite_->param_.transform.translate.x = -hpFrameMiddleSprite_->param_.transform.scale.x;
	hpFrameRightSprite_->param_.transform.translate.x = hpFrameMiddleSprite_->param_.transform.scale.x;

	hpLeftSprite_->param_.transform.translate.x = -static_cast<float>((width_ - 6) / 2);
	hpMiddleSprite_->param_.transform.translate.x = hpLeftSprite_->param_.transform.translate.x;
	hpMiddleSprite_->param_.transform.scale.x = static_cast<float>((width_ - 6) / 2);
	hpRightSprite_->param_.transform.translate.x = hpMiddleSprite_->param_.transform.translate.x + hpMiddleSprite_->param_.transform.scale.x * 2.0f;

	// ゲージ区切りのスケールと位置を設定する
	hpSeparatorSprite_->param_.transform.translate = Vector2(hpRightSprite_->param_.transform.translate.x, hpRightSprite_->param_.transform.translate.y);

	// 色を設定する
	hpFrameLeftSprite_->param_.material.color.w = alpha_;
	hpFrameMiddleSprite_->param_.material.color.w = alpha_;
	hpFrameRightSprite_->param_.material.color.w = alpha_;
	hpLeftSprite_->param_.material.color = Vector4(color_.x, color_.y, color_.z, alpha_);
	hpMiddleSprite_->param_.material.color = Vector4(color_.x, color_.y, color_.z, alpha_);
	hpRightSprite_->param_.material.color = Vector4(color_.x, color_.y, color_.z, alpha_);

	// ワールドトランスフォームを更新する
	worldTransform_->Update();
}

/// @brief 更新処理
void Gage::Update()
{
	// スケールと位置を設定する
	hpFrameMiddleSprite_->param_.transform.scale.x = static_cast<float>(width_ / 2);
	hpFrameLeftSprite_->param_.transform.translate.x = -hpFrameMiddleSprite_->param_.transform.scale.x;
	hpFrameRightSprite_->param_.transform.translate.x = hpFrameMiddleSprite_->param_.transform.scale.x;

	// HPの割合を計算する
	float hpRate = 0.0f;
	if (maxGage_ > 0) hpRate = static_cast<float>(currentGage_) / static_cast<float>(maxGage_);

	// HPのスケールと位置を設定する
	float hpBarWidth = static_cast<float>(width_ - 6) / 2.0f;
	float hpWidth = hpBarWidth * hpRate;

	hpLeftSprite_->param_.transform.translate.x = -hpBarWidth;
	hpMiddleSprite_->param_.transform.translate.x = hpLeftSprite_->param_.transform.translate.x;
	hpMiddleSprite_->param_.transform.scale.x = hpWidth;
	hpRightSprite_->param_.transform.translate.x = hpMiddleSprite_->param_.transform.translate.x + hpMiddleSprite_->param_.transform.scale.x * 2.0f;

	// ゲージ区切りのスケールと位置を設定する
	hpSeparatorSprite_->param_.transform.translate = Vector2(hpRightSprite_->param_.transform.translate.x, hpRightSprite_->param_.transform.translate.y);

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
	hpSeparatorSprite_->param_.material.color.w = alpha;

	// 基底クラスの更新処理を呼び出す
	HUD::Update();
}

/// @brief 描画処理
void Gage::Draw()
{
	// 表示されていない場合は描画しない
	if (!isVisible_)return;

	// hp枠の描画
	hpFrameMiddleSprite_->Draw();
	hpFrameLeftSprite_->Draw();
	hpFrameRightSprite_->Draw();

	// hpの描画
	hpMiddleSprite_->Draw();
	if (currentGage_ > 0)
	{
		hpLeftSprite_->Draw();
		hpRightSprite_->Draw();
	}

	// 体力ゲージの区切りの描画
	if (currentGage_ > 0)
	{
		hpSeparatorSprite_->Draw();
	}
}

/// @brief 現在のゲージを設定する
/// @param gage 
void Gage::SetCurrentGage(int gage)
{
	// ゲージが変化したかどうかを判定する
	if (currentGage_ > gage)
	{
		// ダメージなどのとき

		// ゲージを設定する
		currentGage_ = gage;
	}
	else if (currentGage_ < gage)
	{
		// 回復などのとき

		// ゲージを設定する
		currentGage_ = gage;
	}
}

/// @brief ゲージバーを持つキャラクターが死亡したときの処理
void Gage::Death()
{
	// 既に死亡している場合は処理しない
	if (isDeath_)return;

	// 死亡フラグを立てる
	isDeath_ = true;
}