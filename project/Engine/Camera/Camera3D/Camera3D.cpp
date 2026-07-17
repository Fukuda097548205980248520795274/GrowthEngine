#include "Camera3D.h"
#include "GrowthEngine.h"

namespace
{
	/// @brief ハルトン列を取得する
	/// @param index 
	/// @param base 
	/// @return 
	float GetHaltonSequence(int index, int base)
	{
		float result = 0.0f;
		float f = 1.0f / base;
		int i = index;

		while (i > 0)
		{
			// 割り算の余りを使って、小数部を計算していく
			result += f * (i % base);
			i = i / base;     // 整数同士の割り算（桁を落とす）
			f = f / base;     // 次の桁の重み
		}

		return result; // 0.0 ～ 1.0 未満の範囲の値が返る
	}
}

/// @brief コンストラクタ
Engine::Camera3D::Camera3D()
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// パラメータの生成
	param_ = std::make_unique<Camera3DData::Param>();

	// トランスフォーム
	param_->transform.rotate = Vector3(0.0f, 0.0f, 0.0f);
	param_->transform.translate = Vector3(0.0f, 0.0f, 0.0f);

	// 設定
	param_->setting.fov = 0.45f;
	param_->setting.nearClip = 0.01f;
	param_->setting.farClip = 800.0f;

	// 画面の幅を取得する
	param_->aspect.width = 1280.0f;
	param_->aspect.height = 720.0f;

	// 正射影行列を作成する
	projectionMatrix_ =
		MakePerspectiveFovMatrix4x4(param_->setting.fov, param_->aspect.width / param_->aspect.height, param_->setting.nearClip, param_->setting.farClip);

	// ビュー正射影行列を作成する
	currentVPMatrix_ = worldMatrix_.Inverse() * projectionMatrix_;
	prevVPMatrix_ = currentVPMatrix_;
	currentVPUnJitterMatrix_ = currentVPMatrix_;
	prevVPUnJitterMatrix_ = prevVPMatrix_;
}

/// @brief 更新処理
void Engine::Camera3D::Update()
{
	// 前フレームのビュー正射影行列を保存する
	prevVPMatrix_ = currentVPMatrix_;
	prevVPUnJitterMatrix_ = currentVPUnJitterMatrix_;

	// 3軸の回転を合成する
	quaternion_ =
		ToQuaternion(param_->transform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	// 正規化する
	quaternion_.Normalize();

	// ワールド行列を生成する
	worldMatrix_ = Make3DRotateMatrix4x4(quaternion_) * Make3DTranslateMatrix4x4(param_->transform.translate);

	// 正射影行列を作成する
	projectionMatrix_ =
		MakePerspectiveFovMatrix4x4(param_->setting.fov, param_->aspect.width / param_->aspect.height, param_->setting.nearClip, param_->setting.farClip);

	// ビュー正射影行列を作成する
	currentVPMatrix_ = worldMatrix_.Inverse() * projectionMatrix_;
	currentVPUnJitterMatrix_ = currentVPMatrix_;
}

/// @brief ジッタリングして更新処理
void Engine::Camera3D::JitterUpdate()
{
	// 前フレームのビュー正射影行列を保存する
	prevVPMatrix_ = currentVPMatrix_;
	prevVPUnJitterMatrix_ = currentVPUnJitterMatrix_;


	// ジッタリングインデックスを更新する
	jitterIndex_++;

	// 0から7の範囲でループさせる
	jitterIndex_ = jitterIndex_ % 8;

	// X軸には素数の「2」を指定
	float haltonX = GetHaltonSequence(jitterIndex_, 2); // 0.0 ～ 1.0の値になる
	// Y軸には素数の「3」を指定
	float haltonY = GetHaltonSequence(jitterIndex_, 3); // 0.0 ～ 1.0の値になる

	// TAAのジッターは「-0.5 ～ +0.5 ピクセル」の範囲にしたいので、0.5を引く
	float jitterX_in_Pixels = haltonX - 0.5f;
	float jitterY_in_Pixels = haltonY - 0.5f;

	// ジッタリングの値をNDC（Normalized Device Coordinates）に変換する
	float ndcOffsetX = (jitterX_in_Pixels * 2.0f) / static_cast<float>(param_->aspect.width);
	float ndcOffsetY = (jitterY_in_Pixels * 2.0f) / static_cast<float>(param_->aspect.height);


	// 3軸の回転を合成する
	quaternion_ =
		ToQuaternion(param_->transform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	// 正規化する
	quaternion_.Normalize();

	// ワールド行列を生成する
	worldMatrix_ = Make3DRotateMatrix4x4(quaternion_) * Make3DTranslateMatrix4x4(param_->transform.translate);

	// 正射影行列を作成する
	projectionMatrix_ =
		MakePerspectiveFovMatrix4x4(param_->setting.fov, param_->aspect.width / param_->aspect.height, param_->setting.nearClip, param_->setting.farClip);

	// ビュー正射影行列を作成する
	currentVPUnJitterMatrix_ = worldMatrix_.Inverse() * projectionMatrix_;

	// ジッタリングの値を正射影行列に反映させる
	projectionMatrix_.m[2][0] += ndcOffsetX;
	projectionMatrix_.m[2][1] += ndcOffsetY;
	currentVPMatrix_ = worldMatrix_.Inverse() * projectionMatrix_;
}