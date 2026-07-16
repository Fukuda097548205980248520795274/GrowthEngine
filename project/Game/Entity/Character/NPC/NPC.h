#pragma once
#include "../Character.h"

class NavMesh;
class BehaviorTreeEditor;
class BehaviorTree;

class NPC : public Character
{
public:

	/// @brief コンストラクタ
	/// @param initData 
	/// @param characterTag 
	/// @param navMesh 
	NPC();

	/// @brief 初期化
	void Initialize(const CharacterInitData& initData, CharacterTag characterTag, const NavMesh* navMesh);

	/// @brief プールに返却したときの処理
	void PoolRelease();

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 描画処理
	void Draw();

	/// @brief 死亡処理
	void Dead() override;

	/// @brief 現在の攻撃のクールタイムを取得する
	/// @return 
	float GetAttackCooltime()const override { return attackCooltime_; }

	/// @brief 攻撃のクールタイムを設定する
	/// @param cooltime 
	void SetAttackCooltime(float cooltime)override { attackCooltime_ = cooltime; }

	/// @brief ナビゲーションメッシュを取得する
	/// @return 
	const NavMesh* GetNavMesh() const override { return navMesh_; }

	/// @brief ビヘイビアツリーの変更をリクエストする
	/// @param newTree 
	void RequestBehaviorTreeChange(BehaviorTree* newTree);

	/// @brief ビヘイビアツリーを初期化する
	/// @param behaviorTreeConfig 
	/// @param behaviorTreeEditor 
	void InitBehaviorTree(const BehaviorTreeConfig& behaviorTreeConfig, BehaviorTreeEditor* behaviorTreeEditor);

	/// @brief ビヘイビアツリーの取得
	/// @return 
	BehaviorTree* GetBehaviorTree() const override { return currentBehaviorTree_; }

	/// @brief ビヘイビアツリーの変更がリクエストされているかどうかを取得する
	/// @return 
	bool IsChangeBehaviorTree()const { return isChangeBehaviorTree_; }


private:

	/// @brief ターゲットとの距離で構え状態を更新する
	void UpdateStanceStateByTargetDistance();

	/// @brief 戦闘中かどうか
	bool isFighting_ = false;

	/// @brief ナビメッシュ
	const NavMesh* navMesh_ = nullptr;


private:

	/// @brief ビヘイビアツリーが変更されるかどうかのフラグ
	bool isChangeBehaviorTree_ = false;

	/// @brief ビヘイビアツリー
	BehaviorTree* currentBehaviorTree_ = nullptr;

	/// @brief 次のビヘイビアツリー
	BehaviorTree* nextBehaviorTree_ = nullptr;


private:

	/// @brief 攻撃クールタイムの更新処理
	/// @param deltaTime 
	void UpdateAttackCooltime(float deltaTime);

	/// @brief 攻撃のクールタイム
	float attackCooltime_ = 0.0f;
};

