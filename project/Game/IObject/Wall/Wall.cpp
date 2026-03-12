#include "Wall.h"

/// @brief 初期化
/// @param model 
void Wall::Initialize(PrefabInstanceStaticModel* model)
{
	// nullptrチェック
	assert(model);

	// 引数を受け取る
	model_ = model;
}

/// @brief 更新処理
void Wall::Update()
{
	// 基底クラスの更新
	IObject::Update();

	// モデルの位置
	model_->param_.modelTransform.translate = worldTransform_->GetWorldPosition();
}

/// @brief 描画処理
void Wall::Draw()
{
	// 描画処理
	model_->Draw();
}