#include "Avoid.h"
#include "Entity/Character/Character.h"

/// @brief コンストラクタ
/// @param character 
/// @param initData 
Avoid::Avoid(Character* character, const AvoidInitData& initData)
	: Action(character), localDirection_(initData.localDirection), time_(initData.time), distance_(initData.distance), isTargetDirection_(initData.isTargetDirection)
{

}

/// @brief 実行
void Avoid::Exec()
{
	// すでに実行されている場合は何もしない
	if (IsExec()) return;

	// 基底クラスの実行
	Action::Exec();


	// キャラクターの向き（前）と右方向
	Vector3 forward = owner_->GetDirection();
	Vector3 right = Vector3(forward.z, 0.0f, -forward.x);

	// 回避方向と各軸の内積を取り、ローカルの前後・左右の移動成分を出す
	float localZ = Dot(Vector3(localDirection_.x, 0.0f, localDirection_.y), forward);
	float localX = -Dot(Vector3(localDirection_.x, 0.0f, localDirection_.y), right);

	// キャラクターの回避開始処理を呼ぶ
	owner_->StartAvoid(Vector3(localX, 0.0f, localZ).Normalize(), distance_, time_);


	// タイマーをリセット
	timer_ = 0.0f;


	// 現在の回避を設定する
	owner_->SetCurrentAvoid(this);
}

/// @brief 終了、中断
void Avoid::Exit()
{
	// 自分が現在の回避処理として登録されている場合のみ、停止とクリアを行う
	if (owner_->GetCurrentAvoid() == this)
	{
		// 回避を停止する
		owner_->SetCurrentAvoid(nullptr);
	}

	Action::Exit();
}

/// @brief リセット
void Avoid::Reset()
{
	Action::Reset(); 
}

/// @brief 更新処理
void Avoid::Update()
{
	// 実行されていない場合は何もしない
	if (!IsExec()) return;

	// タイマーを更新する
	float dt = engine_->GetDeltaTime() * engine_->GetTimeScale();
	timer_ += dt;

	// ターゲット方向を向く場合は、ターゲットの方向を向く
	if (isTargetDirection_ && owner_->GetLockOnTarget())
	{
		
	}

	// キャラクターが掴まれているか、行動不能状態なら回避を終了する
	if (owner_->IsGrabbing() || owner_->IsIncapacitated())
	{
		this->Exit();
		return;
	}

	// 時間を超えたら成功
	if (timer_ >= time_)
	{
		// 回避が終了した場合は、成功フラグを立ててExit()を呼ぶ
		Action::Update();
	}
}

/// @brief 使用中かどうか
/// @return 
bool Avoid::IsUse() const
{
	// 実行していない場合は使用していないとみなす
	if (!IsExec()) return false;

	// 現在の回避がこの回避であればtrue
	return this == owner_->GetCurrentAvoid();
}