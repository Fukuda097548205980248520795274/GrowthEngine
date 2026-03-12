#include "IObject.h"

/// @brief コンストラクタ
/// @param position 
IObject::IObject(const VectorInt3& position) : position_(position)
{
	// ワールドトランスフォームの生成と初期化
	worldTransform_ = std::make_unique<WorldTransform3D>();

	// 位置を変換する
	worldTransform_->translate_.x = static_cast<int32_t>(position_.x) * kGridSize;
	worldTransform_->translate_.y = static_cast<int32_t>(position_.y) * kGridSize;
	worldTransform_->translate_.z = static_cast<int32_t>(position_.z) * -kGridSize;
}

/// @brief 更新処理
void IObject::Update()
{
	// 位置を変換する
	worldTransform_->translate_.x = static_cast<int32_t>(position_.x) * kGridSize;
	worldTransform_->translate_.y = static_cast<int32_t>(position_.y) * kGridSize;
	worldTransform_->translate_.z = static_cast<int32_t>(position_.z) * -kGridSize;

	// ワールドトランスフォームを更新
	worldTransform_->Update();
}