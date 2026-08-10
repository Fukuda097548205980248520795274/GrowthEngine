#pragma once
#include "../CharacterState.h"

class Character;

class CharacterStateAvoid : public CharacterState
{
public:

    /// @brief コンストラクタ
    /// @param owner 
    CharacterStateAvoid(Character* owner, AnimationHandle hFront, AnimationHandle hBack, AnimationHandle hLeft, AnimationHandle hRight)
		: CharacterState(owner), hFront_(hFront), hBack_(hBack), hLeft_(hLeft), hRight_(hRight) {
	}

	/// @brief この状態に入るときに呼ばれる処理
    void Enter() override;

	/// @brief 更新処理
    /// @param dt 
    void Update(float dt) override;

	/// @brief この状態からでるときに呼ばれる処理
    void Exit() override;

	/// @brief 回避方向を設定する
    /// @param dir 
    void SetAvoidDirection(const Vector3& dir);

	/// @brief 回避距離を設定する
	/// @param distance 
	void SetAvoidDistance(float distance) { distance_ = distance; }

	/// @brief 回避時間を設定する
	/// @param duration 
	void SetAvoidDuration(float duration) { avoidDuration_ = duration; }

	/// @brief 連続回避の最大回数を設定する
	/// @param count 
	void SetAvoidCount(int count) { maxConsecutiveAvoidCount_ = count; }


private:

	/// @brief  イージングの前回の値
    float prevEaseT_ = 0.0f;

	/// @brief 回避のタイマー
    float avoidTimer_ = 0.0f;

    /// @brief 回避時間
    float avoidDuration_ = 0.3f;

    /// @brief 連続回避の最大回数
    int maxConsecutiveAvoidCount_ = 3;

    /// @brief 現在の連続回避回数
    int currentAvoidCount_ = 0;

	/// @brief 回避距離
	float distance_ = 1.5f;


    // 回避する方向
    Vector3 avoidDirection_ = { 0.0f, 0.0f, 1.0f };

	/// @brief 回避中に入力があったかどうか
	bool isAvoidInput_ = false;


private:

    /// @brief アニメーションハンドル
    AnimationHandle hFront_ = 0;
    AnimationHandle hBack_ = 0;
    AnimationHandle hLeft_ = 0;
    AnimationHandle hRight_ = 0;
};