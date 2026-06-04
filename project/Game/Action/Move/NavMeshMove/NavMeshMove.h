#pragma once
#include "../Move.h"
#include "NavMesh/NavMesh.h"

struct NavMeshMoveInitData
{
	float stopDistance = 1.0f;
	float moveSpeed = 3.0f;
	bool isDash = false;
};

class NavMeshMove : public Move
{
public:

	/// @brief コンストラクタ
    /// @param character 
	/// @param initData
    NavMeshMove(Character* character, const NavMeshMoveInitData& initData);

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

    /// @brief 移動速度
    float moveSpeed_ = 3.0f;

	/// @brief 経路上の中継点に到達したとみなす距離の二乗
    float waypointRadiusSq_ = 0.25f;

	/// @brief 最後にターゲットの位置を記憶しておく（ターゲットが動いている場合に経路を再計算するため）
    Vector3 lastTargetPosition_ = { 0.0f, 0.0f, 0.0f };
};

