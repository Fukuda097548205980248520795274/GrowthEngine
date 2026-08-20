#include "StageObject.h"

// ステージオブジェクトの更新を有効にするかどうか
bool StageObject::updateEnabled_ = false;

/// @brief コンストラクタ
StageObject::StageObject()
{
	// ワールドトランスフォームの生成
	worldTransform_ = std::make_unique<WorldTransform3D>();
}

/// @brief 更新処理
void StageObject::Update()
{
	// ワールドトランスフォームの更新
	worldTransform_->Update();
}