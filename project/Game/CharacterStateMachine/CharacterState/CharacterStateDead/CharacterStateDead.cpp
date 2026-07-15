#include "CharacterStateDead.h"
#include "Entity/Character/Character.h"

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateDead::Enter()
{
	// ビヘイビアツリーのリクエストを行う
	BehaviorTreeRequest();

	// キャラクターの死亡処理を呼び出す
	owner_->Dead();

	// 移動を停止
	owner_->MoveStop();

	// タイマーをリセット
	deadTimer_ = 0.0f;

	// 死亡モーションを再生
	owner_->SetAnimation(hMotion_, true, false);

}

/// @brief 更新処理
/// @param dt 
void CharacterStateDead::Update(float dt)
{
	// タイマーを進める
	deadTimer_ += dt;

	// 死亡時間を超えたら削除
	if (deadTimer_ >= deadTime_)
	{
		owner_->Delete();
		return;
	}
}

/// @brief この状態からでるときに呼ばれる処理
void CharacterStateDead::Exit()
{
	// タイマーをリセット
	deadTimer_ = 0.0f;
}