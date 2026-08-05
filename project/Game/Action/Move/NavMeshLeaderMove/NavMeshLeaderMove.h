#pragma once
#include "../Move.h"
#include "NavMesh/NavMesh.h"

struct NavMeshLeaderMoveInitData
{
	float stopDistance = 1.0f;
	float moveSpeed = 3.0f;
	bool isDash = false;
};

class NavMeshLeaderMove : public Move
{
public:

    /// @brief コンストラクタ
    /// @param character 
    /// @param initData
    NavMeshLeaderMove(Character* character, const NavMeshLeaderMoveInitData& initData);

    /// @brief 実行
    void Exec() override;

    /// @brief 更新処理
    void Update() override;

private:

    /// @brief 計算されたスムーズな経路
    std::vector<Vector3> path_;

    /// @brief 現在目指している経路上の中継点インデックス
    size_t currentPathIndex_ = 0;

    /// @brief 目的地の停止距離の二乗
    float stopDistanceSq_ = 1.0f;

	/// @brief 目的地の少し前の停止距離の二乗
	float stopBeforeDistanceSq_ = 2.0f;

    /// @brief 移動速度
    float moveSpeed_ = 3.0f;

    /// @brief 経路上の中継点に到達したとみなす距離の二乗
    float waypointRadiusSq_ = 0.25f;

    /// @brief 最後にターゲットの位置を記憶しておく（ターゲットが動いている場合に経路を再計算するため）
    Vector3 lastLeaderPosition_ = { 0.0f, 0.0f, 0.0f };

    /// @brief 現在の移動方向（XZ平面）
    Vector2 currentMoveDirection_ = Vector2(0.0f, 0.0f);

    /// @brief リーダー
    Character* leader_ = nullptr;


private:

    /// @brief 現在の左右の回避方向
    float avoidanceSide_ = 0;

    /// @brief 回避方向を維持するタイマー
    float avoidanceTimer_ = 0.0f;
};

