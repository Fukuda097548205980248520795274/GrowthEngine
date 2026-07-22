#pragma once
#include "GrowthEngine.h"

class PivotPoint
{
public:

	/// @brief データ
	struct Data
	{
		// 中心点
		Vector3 center;

		// 半径
		float radius;

		// 回転角度
		float theta;

		// 回転角度
		float phi;

		/// @brief 球面座標系の位置
		Vector3 sphericalCoordinates = Vector3(0.0, 0.0f, 0.0f);

		/// @brief 中心方向のベクトル
		Vector3 toCenter = Vector3(0.0f, 0.0f, 0.0f);

		/// @brief オイラー角
		Vector3 eularAngles = Vector3(0.0f, 0.0f, 0.0f);
	};


public:

	/// @brief コンストラクタ
	PivotPoint();

	/// @brief 更新処理
	void Update();

	/// @brief データを取得する
	/// @return 
	Data* GetData();


private:

	/// @brief データ
	std::unique_ptr<Data> data_ = nullptr;
};

