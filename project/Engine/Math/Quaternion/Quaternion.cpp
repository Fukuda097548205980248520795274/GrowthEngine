#include "Quaternion.h"
#include <cmath>

/// @brief ノルム
/// @return 
float Quaternion::Norm() const
{
	return std::sqrtf(std::powf(w, 2.0f) + std::powf(x, 2.0f) + std::powf(y, 2.0f) + std::powf(z, 2.0f));
}

/// @brief 正規化
/// @return 
Quaternion Quaternion::Normalize() const
{
	// ノルム
	float norm = this->Norm();

	// 正規化した値
	Quaternion normalize = Quaternion(0.0f, 0.0f, 0.0f, 0.0f);

	if (norm != 0.0f)
	{
		normalize = *this / norm;
	}

	return normalize;
}

/// @brief 共役
/// @return 
Quaternion Quaternion::Conjugate() const
{
	return Quaternion(-x, -y, -z, w);
}

/// @brief 逆
/// @return 
Quaternion Quaternion::Inverse() const
{
	return this->Conjugate() / std::powf(this->Norm(), 2.0f);
}

/// @brief 乗法単位元
/// @return 
Quaternion MakeIdentityQuaternion()
{
	return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

/// @brief 内積
/// @param q1 
/// @param q2 
/// @return 
float Dot(const Quaternion& q1 , const Quaternion& q2)
{
	return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}

/// @brief 球面線形補間
/// @param q1 
/// @param q2 
/// @param t 
/// @return 
Quaternion Slerp(const Quaternion& q1, const Quaternion& q2 , float t)
{
	// 片方のクォータニオンのコピー
	Quaternion q0Copy = q1;

	// 内積
	float dot = Dot(q0Copy, q1);

	// 最短経路を確保
	if (dot < 0.0f)
	{
		q0Copy = -q0Copy;
		dot = -dot;
	}

	// 許容誤差（イプシロン）を定義。例えば 1e-6f など。
	const float DOT_THRESHOLD = 1.0f - 1e-4f; // 例として 0.9999f

	// クォータニオンが非常に近い場合（thetaが非常に小さい場合）
	// 線形補間（Lerp）にフォールバックして、ゼロ除算を防ぐ
	if (dot > DOT_THRESHOLD)
	{
		// 単純な線形補間を実行（後で正規化が必要）
		Quaternion result = (1.0f - t) * q0Copy + t * q1;
		// 結果を正規化して返却
		return result.Normalize(); // Normalize関数があるとして
	}

	// --- 通常の Slerp 処理 ---

	// なす角を求める
	float theta = std::acos(dot);

	// std::sin(theta) がゼロになることは、上の Lerp 処理で回避済み

	// 補間 0.0f
	float scale0 = std::sin((1.0f - t) * theta) / std::sin(theta);

	// 補間 1.0f
	float scale1 = std::sin(t * theta) / std::sin(theta);

	// 補間した値を返却する
	return scale0 * q0Copy + scale1 * q1;
}