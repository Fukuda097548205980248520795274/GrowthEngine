#include "AttackTutorial.h"
#include "Entity/Character/Player/Player.h"

/// @brief 初期化処理
/// @param initData 
void AttackTutorial::Initialize(const InitData& initData)
{
	// 練習時間
	practiceTime_ = initData.practiceTime;
	practiceTimer_ = practiceTime_;

	// プレイヤー
	if(initData.player)player_ = initData.player;

	// 攻撃の最大回数
	attackMaxCount_ = initData.attackMaxCount;

	// ワールドトランスフォームの更新
	worldTransform_->Update();
}

/// @brief 更新処理
void AttackTutorial::Update()
{
	// 更新が無効な場合は処理しない
	if (!updateEnabled_)return;

	// プレイヤーが存在しない場合は終了する
	if (!player_)
	{
		Delete();
		return;
	}

	if (state_ == State::Practice)
	{
		// 攻撃の回数を更新
		if (player_->IsHitAttack())
		{
			attackCount_++;
		}

		// クリア条件を満たしているかどうかをチェック
		if (attackCount_ >= attackMaxCount_)
		{
			state_ = State::Clear;
		}
	}

	// 基底クラスの更新
	Tutorial::Update();
}