#include "AttackTutorial.h"
#include "Entity/Character/Player/Player.h"

/// @brief 初期化処理
/// @param initData 
void AttackTutorial::Initialize(const InitData& initData)
{
	// プレイヤーがいることを確認
	assert(initData.player && "プレイヤーがいません");

	// 練習時間
	practiceTime_ = initData.practiceTime;
	practiceTimer_ = practiceTime_;

	// プレイヤー
	player_ = initData.player;

	// スプライト
	sprite_ = initData.sprite;

	// 攻撃の最大回数
	attackMaxCount_ = initData.attackMaxCount;

	// ワールドトランスフォームの更新
	worldTransform_->Update();
}

/// @brief 更新処理
void AttackTutorial::Update()
{
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