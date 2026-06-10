#include "TitleScene.h"

/// @brief 初期化
void TitleScene::Initialize()
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 当たり判定の生成
	capsule_ = std::make_unique<Collision3DBaseCapsule>("Capsule");
	aabb_ = std::make_unique<Collision3DBaseAABB>("AABB");

	// 衝突対象の設定
	capsule_->SetCollisionTarget(aabb_->GetHandle());

	// 当たり判定インスタンスの生成
	capsuleInstance_ = capsule_->CreateInstance();
	aabbInstance_ = aabb_->CreateInstance();
}

/// @brief 更新処理
void TitleScene::Update()
{
	ImGui::Begin("Capsule");
	ImGui::DragFloat3("Start", &capsuleInstance_->param_->start.x, 0.1f);
	ImGui::DragFloat3("Diff", &capsuleInstance_->param_->diff.x, 0.1f);
	ImGui::DragFloat("Radius", &capsuleInstance_->param_->radius, 0.1f);
	ImGui::End();
}

/// @brief 描画処理
void TitleScene::Draw()
{
	
}