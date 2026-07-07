#pragma once
#include "../Attack.h"

struct GrabAttackInitData
{
	AnimationHandle hAttackMotion = 0; // つかみ～投げの一連のモーション
	float attackTime = 0.0f;           // モーションの総時間
	float moveSpeed = 0.0f;            // 踏み込み速度
	float moveStartTime = 0.0f;
	float moveEndTime = 0.0f;
	JointType jointType = JointType::HandR; // つかみ判定を行うジョイント
	float hitboxStartTime = 0.0f;      // つかみ判定の開始時間
	float hitboxEndTime = 0.0f;        // つかみ判定の終了時間
	float grabTime = 0.0f;           // つかんでいる時間（投げるまでの時間）
};

class GrabAttack : public Attack
{
public:

	/// @brief コンストラクタ
	/// @param character 
	/// @param initData 
	GrabAttack(Character* character, const GrabAttackInitData& initData);

	/// @brief 実行
	virtual void Exec() override;

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief リセット
	virtual void Reset() override;
	
	/// @brief 終了処理
	virtual void Exit() override;

	/// @brief 攻撃キャンセル可能かどうか
	/// @return 
	bool IsCancelable() const override { return hasHit_ && !hasThrown_; }

private:

	/// @brief 攻撃の終了時間
	float grabMaxTime_ = 0.0f;

	// 現在つかんでいるターゲット
	Character* grabbedTarget_ = nullptr;

	// 投げ飛ばしが完了したか
	bool hasThrown_ = false;


private:

	/// @brief 攻撃判定を削除する
	void DeleteHitbox();

	/// @brief 攻撃判定
	AppCollider hitbox_;

	/// @brief 攻撃判定開始時間
	float hitboxStartTime_ = 0.0f;

	/// @brief 攻撃判定終了時間
	float hitboxEndTime_ = 0.0f;

	/// @brief 攻撃判定がヒットしたかどうか
	bool hasHit_ = false;

	/// @brief 攻撃判定を出すジョイント
	JointType jointType_ = JointType::HandR; // デフォルトは右手
};

