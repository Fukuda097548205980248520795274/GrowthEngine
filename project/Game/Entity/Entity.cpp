#include "Entity.h"
#include "StageEditor/StageData/StageData.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"

// 静的メンバの定義
bool Entity::updateEnabled_ = false;

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
	if (!updateEnabled_)return;

	// ワールドトランスフォームの更新
	worldTransform_->Update();
}

/// @brief デバッグUIを描画する
/// @param placementData 
/// @param placementList 
/// @param history 
/// @param isDirty 
void Entity::DrawDebugUI(PlacementData* placementData, std::vector<PlacementData>& placementList, StageEditorHistory* history, bool* isDirty)
{
#ifdef _DEVELOPMENT

	// 更新処理中は位置の変更はできないようにする
	if (updateEnabled_)return;

	// 位置の編集
	if (ImGui::IsItemActivated()) { history->SaveHistory(placementList); *isDirty = true; }
	ImGui::DragFloat3("Position", &worldTransform_->translate_.x, 0.01f);
	if (ImGui::IsItemDeactivatedAfterEdit())placementData->position = worldTransform_->translate_;

	// 回転の編集（Y軸のみ）
	if (ImGui::IsItemActivated()) { history->SaveHistory(placementList); *isDirty = true; }
	ImGui::DragFloat("RotationY", &worldTransform_->rotate_.y, 0.001f);
	if (ImGui::IsItemDeactivatedAfterEdit())placementData->rotateY = worldTransform_->rotate_.y;

	// 拡縮の編集
	if (ImGui::IsItemActivated()) { history->SaveHistory(placementList); *isDirty = true; }
	ImGui::DragFloat3("Scale", &worldTransform_->scale_.x, 0.01f);
	if (ImGui::IsItemDeactivatedAfterEdit())placementData->scale = worldTransform_->scale_;

	// ワールドトランスフォームの更新
	worldTransform_->Update();

#endif
}