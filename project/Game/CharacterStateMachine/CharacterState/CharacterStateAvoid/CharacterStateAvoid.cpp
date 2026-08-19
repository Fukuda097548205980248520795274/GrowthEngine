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

	// イージング差分用変数をリセット
	prevEaseT_ = 0.0f;

	// 回避入力があったことをリセットする
	isAvoidInput_ = false;
}

/// @brief 更新処理
/// @param dt 
void CharacterStateAvoid::Update(float dt)
{
	/// ツリーのリクエストを行う
	HandleBehaviorTreeNotSet();

	// 回避時間が0以下になったら状態を終了する
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

		// 回避入力があった場合、回避方向を設定して連続回避を行う
		if (inputController && inputController->IsAvoidRequested())
		{
			currentAvoidCount_++;
			avoidTimer_ = avoidDuration_;

			bool isInput = false;

			Vector2 moveInputXZ = inputController->GetMoveDirection(isInput);
			if (isInput)
			{
				Vector2 avoidDirection = player->GetAvoidDirection(moveInputXZ, isInput, player->GetCameraYaw());
				SetAvoidDirection(Vector3(avoidDirection.x, 0.0f, avoidDirection.y));
			}
			else
			{
				SetAvoidDirection(avoidDirection_);
			}

			return;
		}
	}
	else
	{
		isAvoidInput_ = false;
	}

	WorldTransform3D* worldTransform = owner_->GetWorldTransform();

	// 回避時間を進める
	avoidTimer_ -= dt;

	// イーズアウト補間の比率を計算
	const float kT = std::clamp<float>(1.0f - (avoidTimer_ / avoidDuration_), 0.0f, 1.0f);
	const float kEaseOutT = 1.0f - std::powf(1.0f - kT, 3); // イーズアウト補間

	// 前フレームからの変化量（デルタ）を計算して移動分だけ加算する
	const float deltaT = kEaseOutT - prevEaseT_;
	prevEaseT_ = kEaseOutT;

	if (worldTransform)
	{
		worldTransform->translate_ += avoidDirection_ * (distance_ * deltaT);
	}

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

	// 回避SEを再生する
	SoundManager::GetInstance()->SeAvoid();

	// 回避方向を正規化して設定する
	avoidDirection_ = dir.Normalize();

	// イージング差分用変数をリセット
	prevEaseT_ = 0.0f;

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
			if (localZ > 0.0f)
			{
				owner_->SetAnimation(hFront_, true, false);
			}
			else
			{
				owner_->SetAnimation(hBack_, true, false);
			}
		}
		else
		{
			if (localX > 0.0f)
			{
				owner_->SetAnimation(hRight_, true, false);
			}
			else
			{
				owner_->SetAnimation(hLeft_, true, false);
			}
		}
	}

	// 回避入力があったことを記録する
	isAvoidInput_ = true;
}