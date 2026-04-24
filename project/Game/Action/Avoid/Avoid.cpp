#include "Avoid.h"
#include "Entity/Character/Character.h"

/// @brief 実行
void Avoid::Exec()
{
	// 基底クラスの実行
	Action::Exec();

	// キャラクターの向き（前）と右方向
	Vector3 forward = owner_->GetDirection();
	Vector3 right = Vector3(forward.z, 0.0f, -forward.x);

	// 回避方向と各軸の内積を取り、ローカルの前後・左右の移動成分を出す
	float localZ = Dot(avoidDirection_, forward);
	float localX = Dot(avoidDirection_, right);

	// 前後成分と左右成分、どちらの影響が強いか（絶対値で比較）
	localAvoidDirection_ = Vector3(localX, 0.0f, localZ).Normalize();
}

/// @brief 更新処理
void Avoid::Update()
{

}