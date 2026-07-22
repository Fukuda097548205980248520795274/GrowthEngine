#include "CharacterStateAvoid.h"
#include "Entity/Character/Player/Player.h"
#include "Entity/Character/NPC/NPC.h"

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateAvoid::Enter()
{
	// ツリーのリクエストを行う
	TreeRequest();

	// タイマーをリセットする
	avoidTimer_ = avoidDuration_;

	// 回避入力があったことをリセットする
	isAvoidInput_ = false;
}

/// @brief 更新処理
/// @param dt 
void CharacterStateAvoid::Update(float dt)
{
	/// ツリーのリクエストを行う
	HandleBehaviorTreeNotSet();

	// 回避時間が終了したら状態をNoneに変更する
	if (avoidDuration_ <= 0.0f)
	{
		auto stateMachine = owner_->GetStateMachine();
		stateMachine->ChangeState("None");
		return;
	}

	// プレイヤーの場合、回避入力があった場合は連続回避を試みる
	if (owner_->IsPlayer() && currentAvoidCount_ < maxConsecutiveAvoidCount_ && !isAvoidInput_)
	{
		auto player = static_cast<Player*>(owner_);
		auto inputController = player->GetInputController();

		if (inputController && inputController->IsAvoidRequested())
		{
			// 連続回避回数を増やす
			currentAvoidCount_++;

			// タイマーをリセットする
			avoidTimer_ = avoidDuration_;

			bool isInput = false;

			// 移動入力がある場合はその方向に回避する
			Vector2 moveInputXZ = inputController->GetMoveDirection(isInput);
			if (isInput)
			{
				Vector2 avoidDirection = player->GetAvoidDirection(moveInputXZ, isInput, player->GetCameraYaw());
				SetAvoidDirection(Vector3(avoidDirection.x, 0.0f, avoidDirection.y));
			}
			else
			{
				// 移動入力がない場合は、今の回避方向を維持する
				SetAvoidDirection(avoidDirection_);
			}

			return;
		}
	}
	else
	{
		// 回避入力があったことをリセットする
		isAvoidInput_ = false;
	}

	// ワールドトランスフォームを取得する
	WorldTransform3D* worldTransform = owner_->GetWorldTransform();

	// 回避時間を進める
	avoidTimer_ -= dt;

	// 開始位置から終了位置まで線形補間で移動する
	const float kT = std::clamp<float>(1.0f - (avoidTimer_ / avoidDuration_), 0.0f, 1.0f);
	const float kEaseOutT = 1.0f - std::powf(1.0f - kT, 3); // イーズアウト補間
	worldTransform->translate_ = Lerp(avoidStartPosition_, avoidEndPosition_, kEaseOutT);

	// 到達したら回避フラグを下ろす
	if (kT >= 1.0f)
	{
		auto stateMachine = owner_->GetStateMachine();
		stateMachine->ChangeState("None");
		return;
	}
}

/// @brief この状態からでるときに呼ばれる処理
void CharacterStateAvoid::Exit()
{
	// タイマーをリセットする
	avoidTimer_ = avoidDuration_;

	// 連続回避回数をリセットする
	currentAvoidCount_ = 0;

	// 回避入力があったことをリセットする
	isAvoidInput_ = false;
}

/// @brief 回避方向を設定する
/// @param dir 
void CharacterStateAvoid::SetAvoidDirection(const Vector3& dir)
{
	// 移動を停止する
	owner_->MoveStop();

	// 回避方向を正規化して設定する
	avoidDirection_ = dir.Normalize();

	// ワールドトランスフォームを取得する
	WorldTransform3D* worldTransform = owner_->GetWorldTransform();

	// 回避開始位置と終了位置を設定する
	avoidStartPosition_ = worldTransform->translate_;
	avoidEndPosition_ = avoidStartPosition_ + Vector3(avoidDirection_.x * distance_, 0.0f, avoidDirection_.z * distance_);

	// キャラクターの移動コンポーネントを取得する
	auto movement = owner_->GetMovement();

	if (avoidDirection_.Length() > 0.0f)
	{
		// キャラクターの向き（前）と右方向
		Vector3 forward = movement->GetDirection();
		Vector3 right = Vector3(forward.z, 0.0f, -forward.x); 

		// 回避方向と各軸の内積を取り、ローカルの前後・左右の移動成分を出す
		float localZ = Dot(avoidDirection_, forward);
		float localX = Dot(avoidDirection_, right);

		// 前後成分と左右成分、どちらの影響が強いか（絶対値で比較）
		if (std::abs(localZ) > std::abs(localX))
		{
			// 前後への回避
			if (localZ > 0.0f)
			{
				// 前回避モーションを再生する
				owner_->SetAnimation(hFront_, true, false);
			}
			else
			{
				// 後ろ回避モーションを再生する
				owner_->SetAnimation(hBack_, true, false);
			}
		}
		else
		{
			// 左右への回避
			if (localX > 0.0f)
			{
				// 右回避モーションを再生する
				owner_->SetAnimation(hRight_, true, false);
			}
			else
			{
				// 左回避モーションを再生する
				owner_->SetAnimation(hLeft_, true, false);
			}
		}
	}

	// 回避入力があったことを記録する
	isAvoidInput_ = true;
}