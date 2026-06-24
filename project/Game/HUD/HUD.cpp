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
	// ワールド座標からクリップ座標へ変換する
	Vector4 clip = Vector4(position.x, position.y, position.z, 1.0f);
	Matrix4x4 viewMatrix = engine_->GetCamera3DViewProjection();

	// クリップ座標をビュー行列で変換する
	Vector4 transform = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	transform.x = clip.x * viewMatrix.m[0][0] + clip.y * viewMatrix.m[1][0] + clip.z * viewMatrix.m[2][0] + clip.w * viewMatrix.m[3][0];
	transform.y = clip.x * viewMatrix.m[0][1] + clip.y * viewMatrix.m[1][1] + clip.z * viewMatrix.m[2][1] + clip.w * viewMatrix.m[3][1];
	transform.z = clip.x * viewMatrix.m[0][2] + clip.y * viewMatrix.m[1][2] + clip.z * viewMatrix.m[2][2] + clip.w * viewMatrix.m[3][2];
	transform.w = clip.x * viewMatrix.m[0][3] + clip.y * viewMatrix.m[1][3] + clip.z * viewMatrix.m[2][3] + clip.w * viewMatrix.m[3][3];
	clip = transform;

	// カメラの後ろにいる場合（wが0以下）は表示しない
	if (clip.w <= 0.0f)
	{
		isVisible_ = false;
		return;
	}
	isVisible_ = true;

	// W除算（同次除算）を行ってNDC（正規化デバイス座標）にする
	Vector3 ndc;
	ndc.x = clip.x / clip.w;
	ndc.y = clip.y / clip.w;
	ndc.z = clip.z / clip.w;

	// カメラの解像度（アスペクトパラメータ）を取得する
	auto cameraParam = engine_->GetCamera2DParam();
	float width = cameraParam->aspect.width;
	float height = cameraParam->aspect.height;

	// NDCをスクリーン座標に変換する
	worldTransform_->translate_.x = (ndc.x + 1.0f) * 0.5f * width;
	worldTransform_->translate_.y = (ndc.y + 1.0f) * 0.5f * height;
}