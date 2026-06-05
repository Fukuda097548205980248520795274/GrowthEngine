#include "StageObject.h"
#include <numbers>
#include "StageEditor/StageData/StageData.h"

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
void StageObject::DrawDebugUI(PlacementData* placementData, std::vector<PlacementData>& placementList, StageEditorHistory* history)
{
#ifdef _DEVELOPMENT

	// 更新が有効なときはUIを表示しない（誤操作防止のため）
	if (updateEnabled_)return;

	// ワールドトランスフォームの位置、回転、拡縮をドラッグで編集できるUI
	ImGui::DragFloat3("Position", &worldTransform_->translate_.x, 0.01f);
	ImGui::DragFloat("RotationY", &worldTransform_->rotate_.y, 0.001f);
	ImGui::DragFloat3("Scale", &worldTransform_->scale_.x, 0.01f);

	// ワールドトランスフォームの更新
	worldTransform_->Update();

	// 配置データに反映
	placementData->position = worldTransform_->translate_;
	placementData->rotateY = worldTransform_->rotate_.y;
	placementData->scale = worldTransform_->scale_;

#endif
}