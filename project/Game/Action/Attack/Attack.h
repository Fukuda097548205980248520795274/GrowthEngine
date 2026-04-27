#pragma once
#include "../Action.h"
#include "AppCollider/AppCollider.h"

/// @brief 攻撃入力の種類
enum class AttackInputType
{
	None,
	Light,
	Heavy
};

enum class DamageReaction
{
	None, // ダメージなし
	LightStagger, // 軽い怯み
	HeavyStagger, // 重い怯み
	Down, // ダウン
	BlowAway, // 吹き飛び
};

class Attack : public Action
{
public:

	/// @brief コンストラクタ
	/// @param character 
	Attack(Character* character) : Action(character) {}

	/// @brief 実行
	virtual void Exec() override;

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 次の攻撃があるかどうか
	/// @return 
	virtual bool HasNextAttack(AttackInputType inputType) const { return false; }

	/// @brief 攻撃中かどうか
	/// @return 
	bool IsUse() const override;

	/// @brief 攻撃力を取得する
	/// @return 
	int GetDamage() const { return damage_; }

	/// @brief ノックバック力を取得する
	/// @return 
	float GetKnockback() const { return knockback_; }


protected:

	// 攻撃モーションのハンドル
	AnimationHandle hAttackMotion_ = 0;


protected:

	/// @brief 攻撃時間
	float attackTime_ = 0.0f;

	/// @brief 攻撃タイマー
	float attackTimer_ = 0.0f;


protected:

	/// @brief 移動速度
	float moveSpeed_ = 0.0f;

	/// @brief 移動開始時間（攻撃開始からの遅延時間）
	float moveStartTime_ = 0.0f;

	/// @brief 移動終了時間
	float moveEndTime_ = 0.0f;


protected:

	/// @brief 攻撃力
	int damage_ = 10;

	/// @brief ダメージリアクション
	DamageReaction damageReaction_ = DamageReaction::LightStagger;

	/// @brief ノックバック力（吹き飛ばす強さ）
	float knockback_ = 0.0f;


protected:

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

	/// @brief パーツ名
	std::string partName_{};
};

