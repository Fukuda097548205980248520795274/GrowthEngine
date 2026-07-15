#pragma once
#include "GrowthEngine.h"
#include "BehaviorTree/BehaviorTree.h"

class Character;

class CharacterState
{
public:

	/// @brief 仮想デストラクタ
	virtual ~CharacterState() = default;

	/// @brief コンストラクタ
	/// @param owner 
	CharacterState(Character* owner) : owner_(owner) {}

	/// @brief この状態に入るときに呼ばれる処理
	virtual void Enter(){}

	/// @brief 更新処理
	/// @param dt 
	virtual void Update(float dt){}

	/// @brief この状態からでるときに呼ばれる処理
	virtual void Exit(){}

	/// @brief ビヘイビアツリーを設定する
	/// @param behaviorTree 
	void SetBehaviorTree(std::unique_ptr<BehaviorTree> behaviorTree) { behaviorTree_ = std::move(behaviorTree); }

	/// @brief ビヘイビアツリーを取得する
	/// @return 
	BehaviorTree* GetBehaviorTree() const { return behaviorTree_.get(); }

	/// @brief ビヘイビアツリーのリクエスト
	void BehaviorTreeRequest();


protected:

	// 所有者
	Character* owner_ = nullptr;

	/// @brief ビヘイビアツリー
	std::unique_ptr<BehaviorTree> behaviorTree_ = nullptr;
};

