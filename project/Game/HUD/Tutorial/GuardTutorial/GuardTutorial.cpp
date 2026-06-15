#include "GuardTutorial.h"
#include "Entity/Character/Player/Player.h"

/// @brief 初期化処理
/// @param initData 
void GuardTutorial::Initialize(const InitData& initData)
{
	// 練習時間
	practiceTime_ = initData.practiceTime;
	practiceTimer_ = practiceTime_;

	// プレイヤー
	player_ = initData.player;

	// スプライト
	sprite_ = initData.sprite;

	// 攻撃の最大回数
	guardMaxCount_ = initData.guardMaxCount;
}

/// @brief 更新処理
void GuardTutorial::Update()
{
	if (state_ == State::Practice)
	{
		// 攻撃の回数を更新
		if (player_->IsHitAttack())
		{
			guardCount_++;
		}

		// クリア条件を満たしているかどうかをチェック
		if (guardCount_ >= guardMaxCount_)
		{
			state_ = State::Clear;
		}
	}

	// 基底クラスの更新
	Tutorial::Update();
}