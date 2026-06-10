#include "Floor.h"
#include <numbers>
#include "StageEditor/StageData/StageData.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"

/// @brief コンストラクタ
/// @param initData 
Floor::Floor() : StageObject()
{
	// タグ
	tag_ = StageObjectTag::Floor;
}

/// @brief デストラクタ
Floor::~Floor()
{
	// 衝突判定の削除
	if (collision_)
	{
		collision_->Delete();
		collision_ = nullptr;
	}

	// モデルの削除
	if (model_)
	{
		model_->isDelete_ = true;
		model_ = nullptr;
	}
}

/// @brief 初期化
/// @param initData 
void Floor::Initialize(const InitData& initData)
{
	// nullptr
	assert(initData.collision != nullptr);

	// 位置
	worldTransform_->translate_ = initData.position;

	//　大きさ
	worldTransform_->scale_ = initData.scale;

	// 衝突判定
	collision_ = initData.collision;

	// モデル
	if (initData.model)
	{
		model_ = initData.model;

		// 親
		model_->param_.parent = worldTransform_.get();
	}

	// ワールドトランスフォームを更新する
	worldTransform_->Update();
}

/// @brief 更新処理
void Floor::Update()
{
	// 基底クラスの更新処理
	StageObject::Update();

	// 衝突判定のパラメータを更新
	collision_->param_->center = GetWorldPosition();
	collision_->param_->radius = worldTransform_->scale_;
}

/// @brief 描画処理
void Floor::Draw()
{
	if (model_)model_->Draw();
}


/// @brief デバッグUIを描画する
/// @param placementData 
/// @param placementList 
/// @param history 
/// @param isDirty 
void Floor::DrawDebugUI(PlacementData* placementData, std::vector<PlacementData>& placementList, StageEditorHistory* history, bool* isDirty)
{
#ifdef _DEVELOPMENT

	// 更新が有効なときはUIを表示しない（誤操作防止のため）
	if (updateEnabled_)return;

	// 位置の編集
	if (ImGui::IsItemActivated()) { history->SaveHistory(placementList); *isDirty = true; }
	ImGui::DragFloat3("位置", &worldTransform_->translate_.x, 0.01f);
	if (ImGui::IsItemDeactivatedAfterEdit())placementData->position = worldTransform_->translate_;

	// 拡縮の編集
	if (ImGui::IsItemActivated()) { history->SaveHistory(placementList); *isDirty = true; }
	ImGui::DragFloat3("大きさ", &worldTransform_->scale_.x, 0.01f);
	if (ImGui::IsItemDeactivatedAfterEdit())placementData->scale = worldTransform_->scale_;

	// ワールドトランスフォームの更新
	worldTransform_->Update();

#endif
}