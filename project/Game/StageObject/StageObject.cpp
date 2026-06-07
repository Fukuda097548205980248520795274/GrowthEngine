#include "StageObject.h"
#include <numbers>
#include "StageEditor/StageData/StageData.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"

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
	// 更新が有効でないときは処理しない
	if (!updateEnabled_)return;

	// ワールドトランスフォームの更新
	worldTransform_->Update();
}

/// @brief デバッグUIを描画する
/// @param placementData 
/// @param placementList 
/// @param history 
/// @param isDirty 
void StageObject::DrawDebugUI(PlacementData* placementData, std::vector<PlacementData>& placementList, StageEditorHistory* history, bool* isDirty)
{
#ifdef _DEVELOPMENT

	// 更新が有効なときはUIを表示しない（誤操作防止のため）
	if (updateEnabled_)return;

	// 位置の編集
	if (ImGui::IsItemActivated()){history->SaveHistory(placementList); *isDirty = true;}
	ImGui::DragFloat3("Position", &worldTransform_->translate_.x, 0.01f);
	if (ImGui::IsItemDeactivatedAfterEdit())placementData->position = worldTransform_->translate_;

	// 回転の編集
	if (ImGui::IsItemActivated()){history->SaveHistory(placementList); *isDirty = true;}
	ImGui::DragFloat3("Rotation", &worldTransform_->rotate_.x, 0.001f);
	if (ImGui::IsItemDeactivatedAfterEdit())placementData->rotate_ = worldTransform_->rotate_;

	// 拡縮の編集
	if (ImGui::IsItemActivated()){history->SaveHistory(placementList); *isDirty = true;}
	ImGui::DragFloat3("Scale", &worldTransform_->scale_.x, 0.01f);
	if (ImGui::IsItemDeactivatedAfterEdit())placementData->scale = worldTransform_->scale_;

	// ワールドトランスフォームの更新
	worldTransform_->Update();

#endif
}