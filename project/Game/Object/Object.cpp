#include "Object.h"

/// @brief コンストラクタ
Object::Object()
{
	// ワールドトラスフォーム
	worldTransform_ = std::make_unique<WorldTransform2D>();
}

/// @brief 更新処理
void Object::Update()
{
	worldTransform_->Update();
}