#include "RepelEffect.h"

/// @brief 初期化
/// @param instanceModel0 
/// @param instanceModel1 
/// @param instanceModel2 
/// @param position 
void RepelEffect::Initialize(PrefabInstanceTube* instanceModel0, PrefabInstanceTube* instanceModel1, PrefabInstanceTube* instanceModel2, const Vector3& position)
{
	// nullptrチェック
	assert(instanceModel0);
	assert(instanceModel1);
	assert(instanceModel2);

	// 引数を受けとる
	instanceModel0_ = instanceModel0;
	instanceModel1_ = instanceModel1;
	instanceModel2_ = instanceModel2;


	instanceModel0_->param_.transform.translate = position;
	instanceModel1_->param_.transform.translate = position;
	instanceModel2_->param_.transform.translate = position;

	instanceModel0_->param_.transform.rotate = Vector3(GetRandomRange(-3.14f, 3.14f), GetRandomRange(-3.14f, 3.14f), GetRandomRange(-3.14f, 3.14f));
	instanceModel1_->param_.transform.rotate = Vector3(GetRandomRange(-3.14f, 3.14f), GetRandomRange(-3.14f, 3.14f), GetRandomRange(-3.14f, 3.14f));
	instanceModel2_->param_.transform.rotate = Vector3(GetRandomRange(-3.14f, 3.14f), GetRandomRange(-3.14f, 3.14f), GetRandomRange(-3.14f, 3.14f));

	// サイズを設定
	instanceModel0_->param_.material.enableLighting = false;
	instanceModel0_->param_.material.color = Vector4(1.0f, 0.5f, 0.0f, 1.0f);
	instanceModel0_->param_.size.height = 0.0f;
	instanceModel0_->param_.size.radiusTop = 0.25f;
	instanceModel0_->param_.size.radiusBottom = 0.25f;

	instanceModel1_->param_.material.enableLighting = false;
	instanceModel1_->param_.material.color = Vector4(1.0f, 0.5f, 0.0f, 1.0f);
	instanceModel1_->param_.size.height = 0.0f;
	instanceModel1_->param_.size.radiusTop = 0.25f;
	instanceModel1_->param_.size.radiusBottom = 0.25f;
	
	instanceModel2_->param_.material.enableLighting = false;
	instanceModel2_->param_.material.color = Vector4(1.0f, 0.5f, 0.0f, 1.0f);
	instanceModel2_->param_.size.height = 0.0f;
	instanceModel2_->param_.size.radiusTop = 0.25f;
	instanceModel2_->param_.size.radiusBottom = 0.25f;
}

/// @brief 更新処理
void RepelEffect::Update()
{
	// 0.0f ~ 1.0f
	float t = 1.0f - (duration_ / startTime_);

	float radiusBottom = Lerp(0.1f, 1.0f, t);
	instanceModel0_->param_.size.radiusBottom = radiusBottom;
	instanceModel1_->param_.size.radiusBottom = radiusBottom;
	instanceModel2_->param_.size.radiusBottom = radiusBottom;

	if (t >= 0.0f && t <= 0.5f)
	{
		float t2 = t / 0.5f;

		float radiusTop = Lerp(0.1f, 1.0f, t2);
		instanceModel0_->param_.size.radiusTop = radiusTop;
		instanceModel1_->param_.size.radiusTop = radiusTop;
		instanceModel2_->param_.size.radiusTop = radiusTop;
	}

	instanceModel0_->param_.material.color.w = 1.0f - t;
	instanceModel1_->param_.material.color.w = 1.0f - t;
	instanceModel2_->param_.material.color.w = 1.0f - t;

	// 基底クラスの更新
	Effect::Update();

	// 終了したらモデルを消す
	if (isFinished_)
	{
		instanceModel0_->isDelete_ = true;
		instanceModel0_ = nullptr;

		instanceModel1_->isDelete_ = true;
		instanceModel1_ = nullptr;

		instanceModel2_->isDelete_ = true;
		instanceModel2_ = nullptr;
	}
}

/// @brief 描画処理
void RepelEffect::Draw()
{
	if (!instanceModel0_ && !instanceModel1_ && !instanceModel2_)return;

	// 描画
	if (instanceModel0_)
	{
		instanceModel0_->Draw();
	}
	if (instanceModel1_)
	{
		instanceModel1_->Draw();
	}
	if (instanceModel2_)
	{
		instanceModel2_->Draw();
	}
}