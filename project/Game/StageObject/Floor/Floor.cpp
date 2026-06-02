#include "Floor.h"

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

	// 衝突判定
	collision_ = initData.collision;

	// モデル
	if (initData.model)
	{
		model_ = initData.model;

		// 親
		model_->param_.parent = worldTransform_.get();
	}
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