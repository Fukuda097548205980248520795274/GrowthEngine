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

	// ワールドトランスフォームを更新する
	worldTransform_->Update();

	// 衝突判定
	if (initData.collision)
	{
		collision_ = initData.collision;

		collision_->param_->center = GetWorldPosition();
		collision_->param_->radius = worldTransform_->scale_;
	}
}

/// @brief 更新処理
void Floor::Update()
{
	// 基底クラスの更新処理
	StageObject::Update();

	if (collision_)
	{
		collision_->param_->center = GetWorldPosition();
		collision_->param_->radius = worldTransform_->scale_;
	}
}

/// @brief 描画処理
void Floor::Draw()
{
	
}
