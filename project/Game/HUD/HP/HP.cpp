#include "HP.h"
#include "Entity/Character/Character.h"

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
	assert(initData.hpLeftSprite && "hpの左のスプライトが設定されていません。");
	assert(initData.hpMiddleSprite && "hpの真ん中のスプライトが設定されていません。");
	assert(initData.hpRightSprite && "hpの右のスプライトが設定されていません。");
	assert(initData.character && "キャラクターが設定されていません。");

	// 横幅を設定する
	width_ = std::max(initData.width, 0);

	// hp枠のスプライトを設定する
	hpFrameLeftSprite_ = initData.hpFrameLeftSprite;
	hpFrameMiddleSprite_ = initData.hpFrameMiddleSprite;
	hpFrameRightSprite_ = initData.hpFrameRightSprite;
	hpLeftSprite_ = initData.hpLeftSprite;
	hpMiddleSprite_ = initData.hpMiddleSprite;
	hpRightSprite_ = initData.hpRightSprite;

	// キャラクターを設定する
	character_ = initData.character;

	// ワールドトランスフォームを設定する
	worldTransform_->translate_ = initData.position;
	worldTransform_->scale_ = initData.scale;

	// 最大体力を設定する
	maxHP_ = character_->GetHp();

	// 親を設定する
	hpFrameMiddleSprite_->param_.parent = worldTransform_.get();
	hpFrameLeftSprite_->param_.parent = worldTransform_.get();
	hpFrameRightSprite_->param_.parent = worldTransform_.get();
	hpMiddleSprite_->param_.parent = worldTransform_.get();
	hpLeftSprite_->param_.parent = worldTransform_.get();
	hpRightSprite_->param_.parent = worldTransform_.get();

	// スケールと位置を設定する
	hpFrameMiddleSprite_->param_.transform.scale.x = static_cast<float>(width_ / 2);
	hpFrameLeftSprite_->param_.transform.translate.x = -hpFrameMiddleSprite_->param_.transform.scale.x;
	hpFrameRightSprite_->param_.transform.translate.x = hpFrameMiddleSprite_->param_.transform.scale.x;

	hpLeftSprite_->param_.transform.translate.x = -static_cast<float>((width_ - 6) / 2);
	hpMiddleSprite_->param_.transform.translate.x = hpLeftSprite_->param_.transform.translate.x;
	hpMiddleSprite_->param_.transform.scale.x = static_cast<float>((width_ - 6) / 2);
	hpRightSprite_->param_.transform.translate.x = hpMiddleSprite_->param_.transform.translate.x + hpMiddleSprite_->param_.transform.scale.x * 2.0f;

	// ワールドトランスフォームを更新する
	worldTransform_->Update();
}

/// @brief 更新処理
void HP::Update()
{
	SetPosition(Vector3(0.0f, 2.0f, 0.0f));

	// スケールと位置を設定する
	hpFrameMiddleSprite_->param_.transform.scale.x = static_cast<float>(width_ / 2);
	hpFrameLeftSprite_->param_.transform.translate.x = -hpFrameMiddleSprite_->param_.transform.scale.x;
	hpFrameRightSprite_->param_.transform.translate.x = hpFrameMiddleSprite_->param_.transform.scale.x;

	hpLeftSprite_->param_.transform.translate.x = -static_cast<float>((width_ - 6) / 2);
	hpMiddleSprite_->param_.transform.translate.x = hpLeftSprite_->param_.transform.translate.x;
	hpMiddleSprite_->param_.transform.scale.x = static_cast<float>((width_ - 6) / 2);
	hpRightSprite_->param_.transform.translate.x = hpMiddleSprite_->param_.transform.translate.x + hpMiddleSprite_->param_.transform.scale.x * 2.0f;

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

	// hpの描画
	hpMiddleSprite_->Draw();
	hpLeftSprite_->Draw();
	hpRightSprite_->Draw();
}