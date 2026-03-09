#include "Object.h"

/// @brief コンストラクタ
/// @param position 
Object::Object(const Vector2& position)
{
	// ワールドトラスフォーム
	worldTransform_ = std::make_unique<WorldTransform2D>();
	worldTransform_->translate_ = position;
}

/// @brief 更新処理
void Object::Update()
{
	worldTransform_->Update();
}