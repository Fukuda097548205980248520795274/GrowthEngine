#include "PivotPoint.h"

/// @brief コンストラクタ
PivotPoint::PivotPoint()
{
	// データの生成と初期化
	data_ = std::make_unique<Data>();
	data_->center = Vector3{ 0.0f, 0.0f, 0.0f };
	data_->radius = 30.0f;
	data_->theta = 0.0f;
	data_->phi = 0.0f;
}

/// @brief 更新処理
void PivotPoint::Update()
{
	// 球面座標系の位置を計算
	data_->sphericalCoordinates = data_->center + SphericalCoordinate(data_->radius, data_->theta, data_->phi);

	// 中心方向のベクトルを計算
	data_->toCenter = (data_->center - data_->sphericalCoordinates).Normalize();

	// オイラー角を計算
	data_->eulerAngles = Vector3(data_->theta, data_->phi, 0.0f);
}

/// @brief データを取得する
/// @return 
PivotPoint::Data* PivotPoint::GetData()
{
	return  data_.get();
}