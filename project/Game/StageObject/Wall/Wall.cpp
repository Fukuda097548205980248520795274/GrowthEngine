#include "Wall.h"
#include <numbers>
#include "StageEditor/StageData/StageData.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"

/// @brief コンストラクタ
/// @param initData 
Wall::Wall() : StageObject()
{
	// タグ
	tag_ = StageObjectTag::Wall;
}

/// @brief デストラクタ
Wall::~Wall()
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
void Wall::Initialize(const InitData& initData)
{
	// nullptr
	assert(initData.collision != nullptr);

	// 位置
	worldTransform_->translate_ = initData.position;

	// 回転
	worldTransform_->rotate_ = Vector3(0.0f, initData.rotateY, 0.0f);

	//　大きさ
	worldTransform_->scale_ = initData.scale;

	// モデル
	if (initData.model)
	{
		model_ = initData.model;

		// 親
		model_->param_.parent = worldTransform_.get();
	}

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
void Wall::Update()
{
	// 更新が有効でないときは処理しない（誤操作防止のため）
	if (!updateEnabled_)return;

	// 基底クラスの更新処理
	StageObject::Update();
	
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

/// @brief 描画処理
void Wall::Draw()
{
	if (model_)model_->Draw();
}

/// @brief デバッグUIを描画する
/// @param placementData 
/// @param placementList 
/// @param history 
/// @param isDirty 
void Wall::DrawDebugUI(PlacementData* placementData, std::vector<PlacementData>& placementList, StageEditorHistory* history, bool* isDirty)
{
#ifdef _DEVELOPMENT

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