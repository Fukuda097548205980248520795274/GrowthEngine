#include "Player.h"

/// @brief 初期化
/// @param stageData 
void Player::Initialize(Render3DStaticModel* model)
{
	// nullptrチェック
	assert(model);

	// 引数を受け取る
	model_ = model;
}

/// @brief 更新処理
void Player::Update()
{
	// 基底クラス更新
	IObject::Update();
}

/// @brief 描画処理
void Player::Draw()
{
	// モデル描画
	model_->Draw();
}