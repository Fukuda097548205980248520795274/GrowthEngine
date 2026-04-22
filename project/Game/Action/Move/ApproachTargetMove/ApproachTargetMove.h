#pragma once
#include "../Move.h"
#include "Entity/Character/Character.h"

class ApproachTargetMove : public Move
{
public:

    /// @brief コンストラクタ
    /// @param character
    /// @param stopDistance 停止距離[m]
    /// @param moveSpeed 移動速度[m/s]
    ApproachTargetMove(Character* character, float stopDistance = 1.0f, float moveSpeed = 3.0f)
        : Move(character), stopDistanceSq_(stopDistance * stopDistance), moveSpeed_(moveSpeed) {}

    /// @brief 実行
    void Exec() override;

    /// @brief 更新処理
    void Update() override;

private:

    /// @brief 停止距離の二乗
    float stopDistanceSq_ = 1.0f;

    /// @brief 移動速度
    float moveSpeed_ = 3.0f;
};
