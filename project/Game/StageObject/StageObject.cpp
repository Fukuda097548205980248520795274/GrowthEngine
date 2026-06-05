#include "StageObject.h"
#include <numbers>

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

/// @brief デバッグUIを描画する
void StageObject::DrawUI()
{
#ifdef _DEVELOPMENT

	// ワールドトランスフォームの位置、回転、拡縮をドラッグで編集できるUI
	ImGui::DragFloat3("Position", &worldTransform_->translate_.x, 0.01f);
	ImGui::DragFloat3("Rotation", &worldTransform_->rotate_.x, 0.001f);
	ImGui::DragFloat3("Scale", &worldTransform_->scale_.x, 0.01f);

#endif
}