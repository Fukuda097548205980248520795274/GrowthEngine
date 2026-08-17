#include "Defense.h"
#include "Entity/Character/Character.h"
#include "CharacterStateMachine/CharacterState/CharacterStateGuard/CharacterStateGuard.h"

/// @brief コンストラクタ
/// @param character 
/// @param initData 
Defense::Defense(Character* character, const DefenseInitData& initData) : Action(character)
{
	// 初期化データをメンバ変数にコピーする
	defenseTime_ = initData.defenseTime;
	parryType_ = initData.parryType;
}

/// @brief 実行
void Defense::Exec()
{
	// ブレイクポイントのチェック
	BreakpointOnExec();

	// 基底の実行処理
	Action::Exec();

	// 所有者が存在しない場合、または回避、掴み、気絶などの状態であれば終了する
	if (!owner_ || owner_->IsJustAvoided() || owner_->IsGrabbing() || owner_->IsIncapacitated() || owner_->IsDash())
	{
		// 失敗
		Exit();
		return;
	}

	// 所有者がガード状態でない場合は、ガードステートに変更する
	if (!owner_->IsGuard())
	{
		// ステートマシンからガードステートを取得し、防御時間を設定する
		auto stateMachine = owner_->GetStateMachine();
		stateMachine->ChangeState("Guard");

		// ガードステートが取得できた場合は、防御時間を設定する
		if (auto guardState = dynamic_cast<CharacterStateGuard*>(stateMachine->GetCurrentState()))
		{
			// ガードステートが取得できた場合は、防御時間を設定する
			if (guardState)
			{
				guardState->SetGuardDuration(defenseTime_);
				guardState->SetParryType(parryType_);
			}
		}
		else
		{
			// ガードステートが取得できなかった場合は、ステートをNoneに変更して終了する
			stateMachine->ChangeState("None");

			Exit();
			return;
		}
	}
	else
	{
		// 失敗
		Exit();
		return;
	}

	// ここまで来たら成功
	Action::Update();
}

/// @brief リセット
void Defense::Reset()
{
	// ブレイクポイントのチェック
	BreakpointOnReset();

	// 基底のリセット
	Action::Reset();
}

/// @brief 終了、中断
void Defense::Exit()
{
	// ブレイクポイントのチェック
	BreakpointOnExit();

	// 基底の終了処理
	Action::Exit();
}