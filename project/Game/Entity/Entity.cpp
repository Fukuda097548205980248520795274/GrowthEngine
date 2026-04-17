#include "Entity.h"

/// @brief コンストラクタ 位置を指定して生成する
/// @param position 
Entity::Entity()
{
	// ワールドトランスフォームを生成
	worldTransform_ = std::make_unique<WorldTransform3D>();
}

/// @brief 更新処理
void Entity::Update()
{
	// ワールドトランスフォームの更新
	worldTransform_->Update();
}