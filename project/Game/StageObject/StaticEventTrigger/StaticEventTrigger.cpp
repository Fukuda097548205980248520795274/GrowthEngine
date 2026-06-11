#include "StaticEventTrigger.h"
#include <numbers>
#include "StageEditor/StageData/StageData.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"

/// @brief コンストラクタ
/// @param initData 
StaticEventTrigger::StaticEventTrigger() : StageObject()
{
	// タグ
	tag_ = StageObjectTag::StaticEventTrigger;
}

/// @brief デストラクタ
StaticEventTrigger::~StaticEventTrigger()
{
	// 衝突判定の削除
	if (collision_)
	{
		collision_->Delete();
		collision_ = nullptr;
	}
}

/// @brief 初期化
/// @param initData 
void StaticEventTrigger::Initialize(const InitData& initData)
{
	// nullptr
	assert(initData.collision != nullptr);

	// 位置
	worldTransform_->translate_ = initData.position;

	//　大きさ
	worldTransform_->scale_ = initData.scale;

	// 衝突判定
	collision_ = initData.collision;

	// イベントの種類
	eventType_ = initData.eventType;

	// イベントが発生したときのコールバック関数
	onTriggerCallback_ = initData.onTriggerCallback;

	// イベントの整数パラメータ
	strcpy_s(eventStringParam_, sizeof(eventStringParam_), initData.eventStringParam);

	// ワールドトランスフォームを更新する
	worldTransform_->Update();
}

/// @brief 更新処理
void StaticEventTrigger::Update()
{
	// 基底クラスの更新処理
	StageObject::Update();

	// 衝突判定のパラメータを更新
	collision_->param_->center = GetWorldPosition();
	collision_->param_->radius = worldTransform_->scale_;

	// 衝突しているかどうか
	if (collision_->isCollision_)
	{
		// コールバック関数を呼び出す
		if (onTriggerCallback_)
		{
			bool shouldDelete = onTriggerCallback_(eventType_, eventStringParam_);

			// イベントが発生したときのコールバック関数がtrueを返した場合は削除する
			if (shouldDelete)Delete();
		}
	}
}

/// @brief デバッグUIを描画する
/// @param placementData 
/// @param placementList 
/// @param history 
/// @param isDirty 
void StaticEventTrigger::DrawDebugUI(PlacementData* placementData, std::vector<PlacementData>& placementList, StageEditorHistory* history, bool* isDirty)
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