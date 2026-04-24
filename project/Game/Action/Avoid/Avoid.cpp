#include "Avoid.h"
#include "Entity/Character/Character.h"

/// @brief 実行
void Avoid::Exec()
{
	// すでに実行されている場合は何もしない
	if (IsExec()) return;

	// 基底クラスの実行
	Action::Exec();

	// キャラクターの向き（前）と右方向
	Vector3 forward = owner_->GetDirection();
	Vector3 right = Vector3(forward.z, 0.0f, -forward.x);

	// 回避方向と各軸の内積を取り、ローカルの前後・左右の移動成分を出す
	float localZ = Dot(avoidDirection_, forward);
	float localX = -Dot(avoidDirection_, right);

	// キャラクターの回避開始処理を呼ぶ
	owner_->StartAvoid(Vector2(localX, localZ).Normalize(), true, 0.0f);

	// 現在の回避を設定する
	owner_->SetCurrentAvoid(this);
}

/// @brief 更新処理
void Avoid::Update()
{
	// 実行されていない場合は何もしない
	if (!IsExec()) return;

	// 回避中であれば何もしない
	if (owner_->IsAvoid())return;

	// 回避が終了した場合は、成功フラグを立ててExit()を呼ぶ
	Action::Update();

	// ポインタを消す
	owner_->SetCurrentAvoid(nullptr);
}