#pragma once
#include "../Move.h"
#include "Entity/Character/Character.h"

struct ApproachTargetMoveInitData
{
	float stopDistance = 1.0f; // 停止距離[m]
	float moveSpeed = 3.0f;    // 移動速度[m/s]
	bool isDash = false;      // ダッシュするかどうか
};

class ApproachTargetMove : public Move
{
public:

    /// @brief コンストラクタ
    /// @param character
	/// @param initData
    ApproachTargetMove(Character* character, const ApproachTargetMoveInitData& initData)
        : Move(character, initData.isDash), stopDistanceSq_(initData.stopDistance * initData.stopDistance), moveSpeed_(initData.moveSpeed){}

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
