#include "HP.h"

/// @brief デストラクタ
HP::~HP()
{
	if (hpFrameLeftSprite_)hpFrameLeftSprite_->isDelete_ = true;
	hpFrameLeftSprite_ = nullptr;

	if (hpFrameMiddleSprite_)hpFrameMiddleSprite_->isDelete_ = true;
	hpFrameMiddleSprite_ = nullptr;

	if (hpFrameRightSprite_)hpFrameRightSprite_->isDelete_ = true;
	hpFrameRightSprite_ = nullptr;
}

/// @brief 初期化
/// @param initData 
void HP::Initialize(const InitData& initData)
{
	// nullptrチェック
	assert(initData.hpFrameLeftSprite && "hp枠の左のスプライトが設定されていません。");
	assert(initData.hpFrameMiddleSprite && "hp枠の真ん中のスプライトが設定されていません。");
	assert(initData.hpFrameRightSprite && "hp枠の右のスプライトが設定されていません。");

	// 横幅を設定する
	width_ = std::max(initData.width, 0);

	// hp枠のスプライトを設定する
	hpFrameLeftSprite_ = initData.hpFrameLeftSprite;
	hpFrameMiddleSprite_ = initData.hpFrameMiddleSprite;
	hpFrameRightSprite_ = initData.hpFrameRightSprite;

	// ワールドトランスフォームを設定する
	worldTransform_->translate_ = initData.position;

	// 親を設定する
	hpFrameMiddleSprite_->param_.parent = worldTransform_.get();
	hpFrameLeftSprite_->param_.parent = worldTransform_.get();
	hpFrameRightSprite_->param_.parent = worldTransform_.get();

	// スケールと位置を設定する
	hpFrameMiddleSprite_->param_.transform.scale.x = static_cast<float>(width_ / 2);
	hpFrameLeftSprite_->param_.transform.translate.x = -hpFrameMiddleSprite_->param_.transform.scale.x;
	hpFrameRightSprite_->param_.transform.translate.x = hpFrameMiddleSprite_->param_.transform.scale.x;

	// ワールドトランスフォームを更新する
	worldTransform_->Update();
}

/// @brief 更新処理
void HP::Update()
{
	// スケールと位置を設定する
	hpFrameMiddleSprite_->param_.transform.scale.x = static_cast<float>(width_ / 2);
	hpFrameLeftSprite_->param_.transform.translate.x = -hpFrameMiddleSprite_->param_.transform.scale.x;
	hpFrameRightSprite_->param_.transform.translate.x = hpFrameMiddleSprite_->param_.transform.scale.x;

	// 基底クラスの更新処理を呼び出す
	HUD::Update();
}

/// @brief 描画処理
void HP::Draw()
{
	// hp枠の描画
	hpFrameMiddleSprite_->Draw();
	hpFrameLeftSprite_->Draw();
	hpFrameRightSprite_->Draw();
}