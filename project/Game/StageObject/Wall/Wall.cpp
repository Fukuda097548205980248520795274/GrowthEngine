#include "Wall.h"

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
void Wall::Update()
{
	// 基底クラスの更新処理
	StageObject::Update();
	
	// 回転行列を作成する
	Quaternion rotation = worldTransform_->GetQuaternion();
	Matrix4x4 rotate = Make3DRotateMatrix4x4(rotation);

	// 衝突判定のパラメータを更新
	collision_->param_->center = GetWorldPosition();
	collision_->param_->radius = worldTransform_->scale_;
	collision_->param_->oriented[0] = Vector3(rotate.m[0][0], rotate.m[1][0], rotate.m[2][0]);
	collision_->param_->oriented[1] = Vector3(rotate.m[0][1], rotate.m[1][1], rotate.m[2][1]);
	collision_->param_->oriented[2] = Vector3(rotate.m[0][2], rotate.m[1][2], rotate.m[2][2]);
}

/// @brief 描画処理
void Wall::Draw()
{
	if (model_)model_->Draw();
}