#include "CharacterStateGrabbing.h"
#include "Entity/Character/Character.h"
#include <numbers>

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateGrabbing::Enter()
{
	// ツリーのリクエストを行う
	TreeRequest();

	// 掴む相手をリセットする
	grabTarget_ = nullptr;
}

/// @brief 更新処理
/// @param dt 
void CharacterStateGrabbing::Update(float dt)
{
	// 掴んでいる相手がいない場合は、状態を解除する
	if (!grabTarget_)
	{
		auto stateMachine = owner_->GetStateMachine();
		stateMachine->ChangeState("None");
		return;
	}

	// ワールドトランスフォームを取得する
	WorldTransform3D* ownerTransform = owner_->GetWorldTransform();
	WorldTransform3D* grabTargetTransform = grabTarget_->GetWorldTransform();

	// 掴んでいる相手の位置を、掴んでいる自分の手の位置に合わせる
	Matrix4x4 handMatrix = owner_->GetBoneMatrix(JointType::HandR);
	Vector3 handPos(handMatrix.m[3][0], handMatrix.m[3][1], handMatrix.m[3][2]);
	grabTarget_->SetPosition(handPos + Vector3(0.0f, -1.2f, 0.1f));

	// 掴んでいる相手の向きを、自分の向きと逆にする
	grabTargetTransform->rotate_ = Vector3(0.0f, ownerTransform->rotate_.y + std::numbers::pi_v<float>, 0.0f);

	// 掴んでいる相手が掴まれていない場合は、状態を解除する
	if (!grabTarget_->IsGrabbed())
	{
		grabTarget_ = nullptr;
		auto stateMachine = owner_->GetStateMachine();
		stateMachine->ChangeState("None");
		return;
	}
}

/// @brief この状態からでるときに呼ばれる処理
void CharacterStateGrabbing::Exit()
{
	// 掴んでいる相手を離す
	grabTarget_ = nullptr;
}