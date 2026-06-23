#include "HUD.h"

// ステージオブジェクトの更新を有効にするかどうか
bool HUD::updateEnabled_ = false;

/// @brief コンストラクタ
HUD::HUD()
{
	// ワールドトランスフォームを生成する
	worldTransform_ = std::make_unique<WorldTransform2D>();
}

/// @brief 更新処理
void HUD::Update()
{
	// ワールドトランスフォームを更新する
	worldTransform_->Update();
}

/// @brief 位置を設定する
/// @param position 
void HUD::SetPosition(const Vector3& position)
{
	// クリップ座標を作成する
	Vector4 clip = Vector4(position.x, position.y, position.z, 1.0f);
	clip = Transform(clip, engine_->GetCamera2DViewProjection());

	// クリップ座標のwが0以下なら何もしない
	if (clip.w <= 0.0f)return;

	// クリップ座標を正規化する
	float invW = 1.0f / clip.w;
	
	// NDC座標を作成する
	Vector3 ndc = Vector3(clip.x * invW, clip.y * invW, clip.z * invW);

	// カメラのアスペクトを取得する
	auto cameraParam = engine_->GetCamera2DParam();
	float width = cameraParam->aspect.width;
	float height = cameraParam->aspect.height;

	worldTransform_->translate_.x = (ndc.x + 1.0f) * 0.5f * width;
	worldTransform_->translate_.y = (1.0f - ndc.y) * 0.5f * height;
}