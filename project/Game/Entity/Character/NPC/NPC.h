#pragma once
#include "../Character.h"
#include "BehaviorTree/BehaviorTree.h"

class NavMesh;

class NPC : public Character
{
public:

	/// @brief コンストラクタ
	/// @param initData 
	/// @param characterTag 
	/// @param navMesh 
	NPC(const InitData& initData, CharacterTag characterTag);

	/// @brief 初期化
	void Initialize(std::unique_ptr<BehaviorTree> behaviorTree, const NavMesh* navMesh);

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 描画処理
	void Draw();

	/// @brief 現在の攻撃のクールタイムを取得する
	/// @return 
	float GetAttackCooltime()const override { return attackCooltime_; }

	/// @brief 攻撃のクールタイムを設定する
	/// @param cooltime 
	void SetAttackCooltime(float cooltime)override { attackCooltime_ = cooltime; }

	/// @brief ナビゲーションメッシュを取得する
	/// @return 
	const NavMesh* GetNavMesh() const override { return navMesh_; }


private:

	/// @brief ターゲットとの距離で構え状態を更新する
	void UpdateStanceStateByTargetDistance();

	/// @brief スタイルが変化したときの処理
	/// @param newStyle 
	void OnStyleChanged(FightStyle newStyle) override;

	/// @brief スタイルに応じたビヘイビアツリーを生成する
	/// @param style 
	/// @return 
	std::unique_ptr<BehaviorTree> CreateBehaviorTreeForStyle(FightStyle style);

	/// @brief 戦闘中かどうか
	bool isFighting_ = false;

	/// @brief ビヘイビアツリー
	std::unique_ptr<BehaviorTree> behaviorTree_ = nullptr;

	/// @brief ナビメッシュ
	const NavMesh* navMesh_ = nullptr;



private:

	/// @brief 攻撃クールタイムの更新処理
	/// @param deltaTime 
	void UpdateAttackCooltime(float deltaTime);

	/// @brief 攻撃のクールタイム
	float attackCooltime_ = 0.0f;
};

