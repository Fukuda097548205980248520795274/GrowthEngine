#include "CameraGuard.h"
#include <numbers>
#include "StageEditor/StageData/StageData.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"

/// @brief コンストラクタ
/// @param initData 
CameraGuard::CameraGuard() : StageObject()
{
	// タグ
	tag_ = StageObjectTag::CameraGuard;
}

/// @brief デストラクタ
CameraGuard::~CameraGuard()
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
void CameraGuard::Initialize(const InitData& initData)
{
	// nullptr
	assert(initData.collision != nullptr);

	// 位置
	worldTransform_->translate_ = initData.position;

	// 回転
	worldTransform_->rotate_ = Vector3(0.0f, initData.rotateY, 0.0f);

	//　大きさ
	worldTransform_->scale_ = initData.scale;

	// ワールドトランスフォームを更新する
	worldTransform_->Update();


	// 衝突判定
	if (initData.collision)
	{
		collision_ = initData.collision;

		// 回転行列を作成する
		Quaternion rotation = worldTransform_->GetQuaternion();
		Matrix4x4 rotate = Make3DRotateMatrix4x4(rotation);

		// 衝突判定のパラメータを更新
		collision_->param_->center = GetWorldPosition();
		collision_->param_->radius = worldTransform_->scale_;
		collision_->param_->oriented[0] = Vector3(rotate.m[0][0], rotate.m[0][1], rotate.m[0][2]);
		collision_->param_->oriented[1] = Vector3(rotate.m[1][0], rotate.m[1][1], rotate.m[1][2]);
		collision_->param_->oriented[2] = Vector3(rotate.m[2][0], rotate.m[2][1], rotate.m[2][2]);
	}
}

/// @brief 更新処理
void CameraGuard::Update()
{
	// 衝突判定のパラメータを更新
	if (collision_)
	{
		// 回転行列を作成する
		Quaternion rotation = worldTransform_->GetQuaternion();
		Matrix4x4 rotate = Make3DRotateMatrix4x4(rotation);

		collision_->param_->center = GetWorldPosition();
		collision_->param_->radius = worldTransform_->scale_;
		collision_->param_->oriented[0] = Vector3(rotate.m[0][0], rotate.m[0][1], rotate.m[0][2]);
		collision_->param_->oriented[1] = Vector3(rotate.m[1][0], rotate.m[1][1], rotate.m[1][2]);
		collision_->param_->oriented[2] = Vector3(rotate.m[2][0], rotate.m[2][1], rotate.m[2][2]);
	}
}

/// @brief デバッグUIを描画する
/// @param placementData 
/// @param placementList 
/// @param history 
/// @param isDirty 
void CameraGuard::DrawDebugUI(PlacementData* placementData, std::vector<PlacementData>& placementList, StageEditorHistory* history, bool* isDirty)
{
#ifdef DEVELOPMENT

	// 更新が有効なときはUIを表示しない（誤操作防止のため）
	if (updateEnabled_)return;

	// 位置の編集
	if (ImGui::IsItemActivated()) { history->SaveHistory(placementList); *isDirty = true; }
	ImGui::DragFloat3("位置", &worldTransform_->translate_.x, 0.01f);
	if (ImGui::IsItemDeactivatedAfterEdit())placementData->position = worldTransform_->translate_;

	// 回転の編集
	if (ImGui::IsItemActivated()) { history->SaveHistory(placementList); *isDirty = true; }
	ImGui::DragFloat("回転 Y軸", &worldTransform_->rotate_.y, 0.001f);
	if (ImGui::IsItemDeactivatedAfterEdit())placementData->rotate_ = Vector3(0.0f, worldTransform_->rotate_.y, 0.0f);

	// 拡縮の編集
	if (ImGui::IsItemActivated()) { history->SaveHistory(placementList); *isDirty = true; }
	ImGui::DragFloat3("大きさ", &worldTransform_->scale_.x, 0.01f);
	if (ImGui::IsItemDeactivatedAfterEdit())placementData->scale = worldTransform_->scale_;

	// ワールドトランスフォームの更新
	worldTransform_->Update();

	// 衝突判定のパラメータを更新
	if (collision_)
	{
		// 回転行列を作成する
		Quaternion rotation = worldTransform_->GetQuaternion();
		Matrix4x4 rotate = Make3DRotateMatrix4x4(rotation);

		// 衝突判定のパラメータを更新
		collision_->param_->center = GetWorldPosition();
		collision_->param_->radius = worldTransform_->scale_;
		collision_->param_->oriented[0] = Vector3(rotate.m[0][0], rotate.m[0][1], rotate.m[0][2]);
		collision_->param_->oriented[1] = Vector3(rotate.m[1][0], rotate.m[1][1], rotate.m[1][2]);
		collision_->param_->oriented[2] = Vector3(rotate.m[2][0], rotate.m[2][1], rotate.m[2][2]);
	}

#endif
}