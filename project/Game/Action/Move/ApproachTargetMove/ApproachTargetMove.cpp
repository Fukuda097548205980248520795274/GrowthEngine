#include "ApproachTargetMove.h"

/// @brief 実行
void ApproachTargetMove::Exec()
{
    // 実行済みが処理しない
    if (IsExec())return;

    // 基底クラスで実行
    Move::Exec();
}

/// @brief 更新処理
void ApproachTargetMove::Update()
{
    // ターゲットを取得する
    Character* target = owner_->GetLockOnTarget();

	// ターゲットがいない場合は移動を停止して終了する
    if (!target)
    {
        Exit();
        return;
    }

	// ターゲットへのベクトルを計算する
    Vector3 toTarget = target->GetPosition() - owner_->GetPosition();
    toTarget.y = 0.0f;

	// ターゲットとの距離の二乗を計算する
    const float distanceSq = toTarget.x * toTarget.x + toTarget.z * toTarget.z;

	// 動けない状態や回避中の場合は移動を停止して終了する
    if (owner_->IsJustAvoided() ||  owner_->IsGrabbing() || owner_->IsIncapacitated())
    {
        Exit();
        return;
    }

	// ターゲットとの距離が停止距離以内の場合は移動を停止して終了する
    if (distanceSq <= stopDistanceSq_)
    {
        Move::Update();
        return;
    }

	// ターゲットへの方向を計算して移動入力を設定する
    const Vector2 moveDirection = Vector2(toTarget.x, toTarget.z).Normalize();
    owner_->SetMoveInputXZ(moveDirection, moveSpeed_);
}