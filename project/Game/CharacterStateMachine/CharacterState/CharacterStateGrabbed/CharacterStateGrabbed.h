#pragma once
#include "../CharacterState.h"

class CharacterStateGrabbed : public CharacterState
{
public:

	/// @brief コンストラクタ
	/// @param owner 
	CharacterStateGrabbed(Character* owner) : CharacterState(owner) {}

	/// @brief この状態に入るときに呼ばれる処理
	void Enter() override;

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt) override;

	/// @brief この状態からでるときに呼ばれる処理
	void Exit() override;

	/// @brief 自分を掴む相手を設定する
	/// @param grabber 
	void SetGrabber(Character* grabber) { grabber_ = grabber; }

	/// @brief 自分を掴む相手を取得する
	/// @return 
	Character* GetGrabber() const { return grabber_; }


private:

	/// @brief 自分をつかんでいる相手
	Character* grabber_ = nullptr;

	/// @brief 掴まれ時間
	float grabbedTimer_ = 0.0f;
};

