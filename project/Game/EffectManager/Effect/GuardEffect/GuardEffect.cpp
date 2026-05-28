#include "GuardEffect.h"

/// @brief 初期化
/// @param instanceModel 
/// @param position 
/// @param rotation 
void GuardEffect::Initialize(PrefabInstanceTube* instanceModel, const Vector3& position, const Vector3& rotation)
{
	// nullptrチェック
	assert(instanceModel);

	// 引数を受けとる
	instanceModel_ = instanceModel;
	instanceModel_->param_.transform.translate = position;
	instanceModel_->param_.transform.rotate = rotation;

	// サイズを設定
	instanceModel_->param_.material.enableLighting = false;
	instanceModel_->param_.material.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	instanceModel_->param_.size.height = 0.0f;
	instanceModel_->param_.size.radiusTop = 0.25f;
	instanceModel_->param_.size.radiusBottom = 0.25f;
}

/// @brief 更新処理
void GuardEffect::Update()
{
	// 0.0f ~ 1.0f
	float t = 1.0f - (duration_ / startTime_);

	instanceModel_->param_.size.radiusBottom = Lerp(0.25f, 0.75f, t);
	
	if (t >= 0.0f && t <= 0.5f)
	{
		float t2 = t / 0.5f;
		instanceModel_->param_.size.radiusTop = Lerp(0.25f, 0.75f, t2);
		instanceModel_->param_.material.color.w = Lerp(0.25f, 0.0f, t2);
	}

	// 基底クラスの更新
	Effect::Update();

	// 終了したらモデルを消す
	if (isFinished_)
	{
		instanceModel_->isDelete_ = true;
		instanceModel_ = nullptr;
	}
}

/// @brief 描画処理
void GuardEffect::Draw()
{
	if (!instanceModel_)return;

	// 描画
	instanceModel_->Draw();
}