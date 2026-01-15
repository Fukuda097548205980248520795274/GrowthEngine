#include "Matrix4x4.h"
#include <cmath>

/// @brief 逆行列
/// @return 
Matrix4x4 Matrix4x4::Inverse() const
{
	float determinant =
		m[0][0] * m[1][1] * m[2][2] * m[3][3] + m[0][0] * m[1][2] * m[2][3] * m[3][1] + m[0][0] * m[1][3] * m[2][1] * m[3][2] -
		m[0][0] * m[1][3] * m[2][2] * m[3][1] - m[0][0] * m[1][2] * m[2][1] * m[3][3] - m[0][0] * m[1][1] * m[2][3] * m[3][2] -

		m[0][1] * m[1][0] * m[2][2] * m[3][3] - m[0][2] * m[1][0] * m[2][3] * m[3][1] - m[0][3] * m[1][0] * m[2][1] * m[3][2] +
		m[0][3] * m[1][0] * m[2][2] * m[3][1] + m[0][2] * m[1][0] * m[2][1] * m[3][3] + m[0][1] * m[1][0] * m[2][3] * m[3][2] +

		m[0][1] * m[1][2] * m[2][0] * m[3][3] + m[0][2] * m[1][3] * m[2][0] * m[3][1] + m[0][3] * m[1][1] * m[2][0] * m[3][2] -
		m[0][3] * m[1][2] * m[2][0] * m[3][1] - m[0][2] * m[1][1] * m[2][0] * m[3][3] - m[0][1] * m[1][3] * m[2][0] * m[3][2] -

		m[0][1] * m[1][2] * m[2][3] * m[3][0] - m[0][2] * m[1][3] * m[2][1] * m[3][0] - m[0][3] * m[1][1] * m[2][2] * m[3][0] +
		m[0][3] * m[1][2] * m[2][1] * m[3][0] + m[0][2] * m[1][1] * m[2][3] * m[3][0] + m[0][1] * m[1][3] * m[2][2] * m[3][0];


	Matrix4x4 adjugateMatrix;

	adjugateMatrix.m[0][0] =
		m[1][1] * m[2][2] * m[3][3] + m[1][2] * m[2][3] * m[3][1] + m[1][3] * m[2][1] * m[3][2] -
		m[1][3] * m[2][2] * m[3][1] - m[1][2] * m[2][1] * m[3][3] - m[1][1] * m[2][3] * m[3][2];

	adjugateMatrix.m[0][1] =
		-m[0][1] * m[2][2] * m[3][3] - m[0][2] * m[2][3] * m[3][1] - m[0][3] * m[2][1] * m[3][2] +
		m[0][3] * m[2][2] * m[3][1] + m[0][2] * m[2][1] * m[3][3] + m[0][1] * m[2][3] * m[3][2];

	adjugateMatrix.m[0][2] =
		m[0][1] * m[1][2] * m[3][3] + m[0][2] * m[1][3] * m[3][1] + m[0][3] * m[1][1] * m[3][2] -
		m[0][3] * m[1][2] * m[3][1] - m[0][2] * m[1][1] * m[3][3] - m[0][1] * m[1][3] * m[3][2];

	adjugateMatrix.m[0][3] =
		-m[0][1] * m[1][2] * m[2][3] - m[0][2] * m[1][3] * m[2][1] - m[0][3] * m[1][1] * m[2][2] +
		m[0][3] * m[1][2] * m[2][1] + m[0][2] * m[1][1] * m[2][3] + m[0][1] * m[1][3] * m[2][2];


	adjugateMatrix.m[1][0] =
		-m[1][0] * m[2][2] * m[3][3] - m[1][2] * m[2][3] * m[3][0] - m[1][3] * m[2][0] * m[3][2] +
		m[1][3] * m[2][2] * m[3][0] + m[1][2] * m[2][0] * m[3][3] + m[1][0] * m[2][3] * m[3][2];

	adjugateMatrix.m[1][1] =
		m[0][0] * m[2][2] * m[3][3] + m[0][2] * m[2][3] * m[3][0] + m[0][3] * m[2][0] * m[3][2] -
		m[0][3] * m[2][2] * m[3][0] - m[0][2] * m[2][0] * m[3][3] - m[0][0] * m[2][3] * m[3][2];

	adjugateMatrix.m[1][2] =
		-m[0][0] * m[1][2] * m[3][3] - m[0][2] * m[1][3] * m[3][0] - m[0][3] * m[1][0] * m[3][2] +
		m[0][3] * m[1][2] * m[3][0] + m[0][2] * m[1][0] * m[3][3] + m[0][0] * m[1][3] * m[3][2];

	adjugateMatrix.m[1][3] =
		m[0][0] * m[1][2] * m[2][3] + m[0][2] * m[1][3] * m[2][0] + m[0][3] * m[1][0] * m[2][2] -
		m[0][3] * m[1][2] * m[2][0] - m[0][2] * m[1][0] * m[2][3] - m[0][0] * m[1][3] * m[2][2];


	adjugateMatrix.m[2][0] =
		m[1][0] * m[2][1] * m[3][3] + m[1][1] * m[2][3] * m[3][0] + m[1][3] * m[2][0] * m[3][1] -
		m[1][3] * m[2][1] * m[3][0] - m[1][1] * m[2][0] * m[3][3] - m[1][0] * m[2][3] * m[3][1];

	adjugateMatrix.m[2][1] =
		-m[0][0] * m[2][1] * m[3][3] - m[0][1] * m[2][3] * m[3][0] - m[0][3] * m[2][0] * m[3][1] +
		m[0][3] * m[2][1] * m[3][0] + m[0][1] * m[2][0] * m[3][3] + m[0][0] * m[2][3] * m[3][1];

	adjugateMatrix.m[2][2] =
		m[0][0] * m[1][1] * m[3][3] + m[0][1] * m[1][3] * m[3][0] + m[0][3] * m[1][0] * m[3][1] -
		m[0][3] * m[1][1] * m[3][0] - m[0][1] * m[1][0] * m[3][3] - m[0][0] * m[1][3] * m[3][1];

	adjugateMatrix.m[2][3] =
		-m[0][0] * m[1][1] * m[2][3] - m[0][1] * m[1][3] * m[2][0] - m[0][3] * m[1][0] * m[2][1] +
		m[0][3] * m[1][1] * m[2][0] + m[0][1] * m[1][0] * m[2][3] + m[0][0] * m[1][3] * m[2][1];


	adjugateMatrix.m[3][0] =
		-m[1][0] * m[2][1] * m[3][2] - m[1][1] * m[2][2] * m[3][0] - m[1][2] * m[2][0] * m[3][1] +
		m[1][2] * m[2][1] * m[3][0] + m[1][1] * m[2][0] * m[3][2] + m[1][0] * m[2][2] * m[3][1];

	adjugateMatrix.m[3][1] =
		m[0][0] * m[2][1] * m[3][2] + m[0][1] * m[2][2] * m[3][0] + m[0][2] * m[2][0] * m[3][1] -
		m[0][2] * m[2][1] * m[3][0] - m[0][1] * m[2][0] * m[3][2] - m[0][0] * m[2][2] * m[3][1];

	adjugateMatrix.m[3][2] =
		-m[0][0] * m[1][1] * m[3][2] - m[0][1] * m[1][2] * m[3][0] - m[0][2] * m[1][0] * m[3][1] +
		m[0][2] * m[1][1] * m[3][0] + m[0][1] * m[1][0] * m[3][2] + m[0][0] * m[1][2] * m[3][1];

	adjugateMatrix.m[3][3] =
		m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] + m[0][2] * m[1][0] * m[2][1] -
		m[0][2] * m[1][1] * m[2][0] - m[0][1] * m[1][0] * m[2][2] - m[0][0] * m[1][2] * m[2][1];

	for (int i = 0; i < 4; ++i)for (int j = 0; j < 4; ++j)
	{
		adjugateMatrix.m[i][j] *= 1.0f / determinant;
	}

	return adjugateMatrix;
}

/// @brief 転置行列
/// @return 
Matrix4x4 Matrix4x4::Transpose() const
{
	Matrix4x4 transpose;

	transpose.m[0][0] = this->m[0][0];
	transpose.m[0][1] = this->m[1][0];
	transpose.m[0][2] = this->m[2][0];
	transpose.m[0][3] = this->m[3][0];

	transpose.m[1][0] = this->m[0][1];
	transpose.m[1][1] = this->m[1][1];
	transpose.m[1][2] = this->m[2][1];
	transpose.m[1][3] = this->m[3][1];

	transpose.m[2][0] = this->m[0][2];
	transpose.m[2][1] = this->m[1][2];
	transpose.m[2][2] = this->m[2][2];
	transpose.m[2][3] = this->m[3][2];

	transpose.m[3][0] = this->m[0][3];
	transpose.m[3][1] = this->m[1][3];
	transpose.m[3][2] = this->m[2][3];
	transpose.m[3][3] = this->m[3][3];

	return transpose;
}

/// @brief 拡大縮小行列
/// @param scale 大きさ
/// @return 
Matrix4x4 MakeScaleMatrix4x4(const Vector3& scale)
{
	Matrix4x4 matrix;

	matrix.m[0][0] = scale.x;
	matrix.m[0][1] = 0.0f;
	matrix.m[0][2] = 0.0f;
	matrix.m[0][3] = 0.0f;

	matrix.m[1][0] = 0.0f;
	matrix.m[1][1] = scale.y;
	matrix.m[1][2] = 0.0f;
	matrix.m[1][3] = 0.0f;

	matrix.m[2][0] = 0.0f;
	matrix.m[2][1] = 0.0f;
	matrix.m[2][2] = scale.z;
	matrix.m[2][3] = 0.0f;

	matrix.m[3][0] = 0.0f;
	matrix.m[3][1] = 0.0f;
	matrix.m[3][2] = 0.0f;
	matrix.m[3][3] = 1.0f;

	return matrix;
}

/// @brief 回転行列
/// @param quaternion クォータニオン
/// @return 
Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion)
{
	Matrix4x4 matrix;

	matrix.m[0][0] = std::powf(quaternion.w, 2.0f) + std::powf(quaternion.x, 2.0f) - std::powf(quaternion.y, 2.0f) - std::powf(quaternion.z, 2.0f);
	matrix.m[0][1] = 2.0f * (quaternion.x * quaternion.y + quaternion.w * quaternion.z);
	matrix.m[0][2] = 2.0f * (quaternion.x * quaternion.z - quaternion.w * quaternion.y);
	matrix.m[0][3] = 0.0f;

	matrix.m[1][0] = 2.0f * (quaternion.x * quaternion.y - quaternion.w * quaternion.z);
	matrix.m[1][1] = std::powf(quaternion.w, 2.0f) - std::powf(quaternion.x, 2.0f) + std::powf(quaternion.y, 2.0f) - std::powf(quaternion.z, 2.0f);
	matrix.m[1][2] = 2.0f * (quaternion.y * quaternion.z + quaternion.w * quaternion.x);
	matrix.m[1][3] = 0.0f;

	matrix.m[2][0] = 2.0f * (quaternion.x * quaternion.z + quaternion.w * quaternion.y);
	matrix.m[2][1] = 2.0f * (quaternion.y * quaternion.z - quaternion.w * quaternion.x);
	matrix.m[2][2] = std::powf(quaternion.w, 2.0f) - std::powf(quaternion.x, 2.0f) - std::powf(quaternion.y, 2.0f) + std::powf(quaternion.z, 2.0f);
	matrix.m[2][3] = 0.0f;

	matrix.m[3][0] = 0.0f;
	matrix.m[3][1] = 0.0f;
	matrix.m[3][2] = 0.0f;
	matrix.m[3][3] = 1.0f;

	return matrix;
}

/// @brief 平行移動行列
/// @param translate 平行移動
/// @return 
Matrix4x4 MakeTranslateMatrix4x4(const Vector3& translate)
{
	Matrix4x4 matrix;

	matrix.m[0][0] = 1.0f;
	matrix.m[0][1] = 0.0f;
	matrix.m[0][2] = 0.0f;
	matrix.m[0][3] = 0.0f;

	matrix.m[1][0] = 0.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[1][2] = 0.0f;
	matrix.m[1][3] = 0.0f;

	matrix.m[2][0] = 0.0f;
	matrix.m[2][1] = 0.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[2][3] = 0.0f;

	matrix.m[3][0] = translate.x;
	matrix.m[3][1] = translate.y;
	matrix.m[3][2] = translate.z;
	matrix.m[3][3] = 1.0f;

	return matrix;
}

/// @brief アフィン変換行列
/// @param scale 拡縮
/// @param quaternion クォータニオン
/// @param translate 平行移動
/// @return 
Matrix4x4 MakeAffineMatrix4x4(const Vector3& scale, const Quaternion& quaternion, const Vector3& translate)
{
	return MakeScaleMatrix4x4(scale) * MakeRotateMatrix(quaternion) * MakeTranslateMatrix4x4(translate);
}

/// @brief 透視投影行列 3D
/// @param fovY 画角
/// @param aspectRatio アスペクト比
/// @param nearClip 近平面の距離
/// @param farClip 遠平面の距離
/// @return 
Matrix4x4 MakePerspectiveFovMatrix4x4(float fovY, float aspectRatio, float nearClip, float farClip)
{
	Matrix4x4 matrix;

	matrix.m[0][0] = (1.0f / aspectRatio) * (1.0f / std::tan(fovY / 2.0f));
	matrix.m[0][1] = 0.0f;
	matrix.m[0][2] = 0.0f;
	matrix.m[0][3] = 0.0f;

	matrix.m[1][0] = 0.0f;
	matrix.m[1][1] = 1.0f / std::tan(fovY / 2.0f);
	matrix.m[1][2] = 0.0f;
	matrix.m[1][3] = 0.0f;

	matrix.m[2][0] = 0.0f;
	matrix.m[2][1] = 0.0f;
	matrix.m[2][2] = farClip / (farClip - nearClip);
	matrix.m[2][3] = 1.0f;

	matrix.m[3][0] = 0.0f;
	matrix.m[3][1] = 0.0f;
	matrix.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	matrix.m[3][3] = 0.0f;

	return matrix;
}

/// @brief 平行投影行列 2D
/// @param left 左
/// @param top 上
/// @param right 右
/// @param bottom 下
/// @param nearClip 近平面の距離
/// @param farClip 遠平面の距離
/// @return 
Matrix4x4 MakeOrthographicMatrix4x4(float left, float top, float right, float bottom, float nearClip, float farClip)
{
	Matrix4x4 matrix;

	matrix.m[0][0] = 2.0f / (right - left);
	matrix.m[0][1] = 0.0f;
	matrix.m[0][2] = 0.0f;
	matrix.m[0][3] = 0.0f;

	matrix.m[1][0] = 0.0f;
	matrix.m[1][1] = 2.0f / (top - bottom);
	matrix.m[1][2] = 0.0f;
	matrix.m[1][3] = 0.0f;

	matrix.m[2][0] = 0.0f;
	matrix.m[2][1] = 0.0f;
	matrix.m[2][2] = 1.0f / (farClip - nearClip);
	matrix.m[2][3] = 0.0f;

	matrix.m[3][0] = (left + right) / (left - right);
	matrix.m[3][1] = (top + bottom) / (bottom - top);
	matrix.m[3][2] = nearClip / (nearClip - farClip);
	matrix.m[3][3] = 1.0f;

	return matrix;
}

/// @brief ビューポート変換行列
/// @param left 左
/// @param top 上
/// @param width スクリーン横幅
/// @param height スクリーン縦幅
/// @param minDepth 最小深度値
/// @param maxDepth 最大深度値
/// @return 
Matrix4x4 MakeViewportMatrix4x4(float left, float top, float width, float height, float minDepth, float maxDepth)
{
	Matrix4x4 matrix;

	matrix.m[0][0] = width / 2.0f;
	matrix.m[0][1] = 0.0f;
	matrix.m[0][2] = 0.0f;
	matrix.m[0][3] = 0.0f;

	matrix.m[1][0] = 0.0f;
	matrix.m[1][1] = -(height / 2.0f);
	matrix.m[1][2] = 0.0f;
	matrix.m[1][3] = 0.0f;

	matrix.m[2][0] = 0.0f;
	matrix.m[2][1] = 0.0f;
	matrix.m[2][2] = maxDepth - minDepth;
	matrix.m[2][3] = 0.0f;

	matrix.m[3][0] = left + (width / 2.0f);
	matrix.m[3][1] = top + (height / 2.0f);
	matrix.m[3][2] = minDepth;
	matrix.m[3][3] = 1.0f;

	return matrix;
}