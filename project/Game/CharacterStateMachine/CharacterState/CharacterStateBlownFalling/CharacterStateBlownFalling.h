#pragma once
#include "../CharacterState.h"

class Character;
class SoundManager;

class CharacterStateBlownFalling : public CharacterState
{
public:

	/// @brief 軽い怯みの種類
	enum class BlownFallingDamageReaction
	{
		None,
		Front,
		Back,
	};


public:

	/// @brief コンストラクタ
	/// @param owner 
	CharacterStateBlownFalling(Character* owner, AnimationHandle hFront, AnimationHandle hBack);

	/// @brief この状態に入るときに呼ばれる処理
	void Enter() override;

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt) override;

	/// @brief この状態からでるときに呼ばれる処理
	void Exit() override;

	/// @brief ダメージリアクションを起こす
	/// @param hitPosition 
	/// @param attacker 
	void DamageReaction(BlownFallingDamageReaction reaction);


private:

	/// @brief アクション
	BlownFallingDamageReaction reaction_ = BlownFallingDamageReaction::None;

	/// @brief アニメーションハンドル
	AnimationHandle hFront_ = 0;
	AnimationHandle hBack_ = 0;


private:

	/// @brief サウンドマネージャー
	SoundManager* soundManager_ = nullptr;
};

