#include "Character.h"

/// @brief 
/// @param position 
Character::Character(const InitData& initData)
	: Entity()
{
	// タグを指定する
	entityTag_ = EntityTag::Character;

	// 位置
	worldTransform_->translate_ = initData.position;

	// モデル
	if(initData.model_)
		model_ = initData.model_;
}

/// @brief 更新処理
void Character::Update()
{
	// 速度の更新
	currentVelocity_ = Lerp(currentVelocity_, targetVelocity_, velocityLerpSpeed_);

	// 位置の更新
	worldTransform_->translate_ += currentVelocity_;

	// 基底クラスの更新
	Entity::Update();
}

/// @brief 移動を停止させる
void Character::MoveStop()
{
	targetVelocity_ = Vector3(0.0f, 0.0f, 0.0f);
	currentVelocity_ = Vector3(0.0f, 0.0f, 0.0f);
}