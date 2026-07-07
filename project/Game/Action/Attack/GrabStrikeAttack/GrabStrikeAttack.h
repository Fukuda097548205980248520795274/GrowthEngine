#pragma once
#include "../Attack.h"

/// @brief 掴み攻撃の初期化データ
struct GrabStrikeAttackInitData
{
	// 攻撃時間
	float attackTime = 0.0f;

	// 移動
	float moveSpeed = 0.0f;
	float moveStartTime = 0.0f;
	float moveEndTime = 0.0f;

	// ノックバックの強さ
	float knockback = 0.0f;

	// ノックバックの方向
	Vector3 knockbackDirection = Vector3(0.0f, 0.0f, 1.0f);

	// アニメーション
	AnimationHandle hAttackAnimation = 0;

	// 相手側のアニメーション
	AnimationHandle hTargetAnimation = 0;

	// 相手を離すかどうか
	bool isRelease = false;

	// 離すタイミング（攻撃開始からの遅延時間）
	float releaseTime = 0.0f;

	// ダメージリアクション
	DamageReaction damageReaction = DamageReaction::None;

	// ヒット
	std::vector<HitDefinition> hits;
};

class GrabStrikeAttack : public Attack
{
public:

	/// @brief コンストラクタ
	/// @param character 
	/// @param initData 
	GrabStrikeAttack(Character* character, const GrabStrikeAttackInitData& initData);

	/// @brief 実行
	virtual void Exec() override;

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief リセット
	virtual void Reset() override;

	/// @brief 終了、中断
	virtual void Exit() override;

	/// @brief 攻撃キャンセル可能かどうか
	/// @return 
	bool IsCancelable() const override{return (attackTimer_ >= cancelStartTime_ && attackTimer_ <= cancelEndTime_);}


private:

	// 前回の攻撃タイマー
	float prevTimer_ = 0.0f;

	// ノックバックの強さ
	float knockback_ = 0.0f;

	// ノックバックの方向
	Vector3 knockbackDirection_ = Vector3(0.0f, 0.0f, 1.0f);

	/// @brief 掴んでいる相手
	Character* grabbedTarget_ = nullptr;

	// 相手側のアニメーション
	AnimationHandle hTargetAnimation_ = 0;

	// 離すかどうか
	bool isRelease_ = false;

	// 離すタイミング（攻撃開始からの遅延時間）
	float releaseTime_ = 0.0f;

	/// @brief ダメージリアクション
	DamageReaction damageReaction_ = DamageReaction::None;

	// ヒット
	std::vector<HitDefinition> hits_;

	// 既に相手を手放したかどうか
	bool isReleased_ = false;


private:

	// コンボキャンセル受付時間
	float cancelStartTime_ = 0.0f;

	// コンボキャンセル終了時間
	float cancelEndTime_ = 0.0f;
};

