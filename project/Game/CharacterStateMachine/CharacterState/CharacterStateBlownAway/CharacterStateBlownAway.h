#pragma once
#include "../CharacterState.h"

class Character;

class CharacterStateBlownAway : public CharacterState
{
public:

	/// @brief 軽い怯みの種類
	enum class DamageReactionType
	{
		None,
		Front,
		Back,
	};


public:

	/// @brief コンストラクタ
	/// @param owner 
	CharacterStateBlownAway(Character* owner, AnimationHandle hFront, AnimationHandle hBack);

	/// @brief この状態に入るときに呼ばれる処理
	void Enter() override;

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt) override;

	/// @brief この状態からでるときに呼ばれる処理
	void Exit() override;

	/// @brief ダメージリアクションを起こす
	/// @param hitPosition 
	void DamageReaction(const std::optional<Vector3>& hitPosition);


private:

	/// @brief アクション
	DamageReactionType reaction_ = DamageReactionType::None;

	/// @brief アニメーションハンドル
	AnimationHandle hFront_ = 0;
	AnimationHandle hBack_ = 0;
};

