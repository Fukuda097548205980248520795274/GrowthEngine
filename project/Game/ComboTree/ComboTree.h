#pragma once
#include <vector>
#include <memory>
#include "Action/Attack/ComboAttack/ComboAttack.h"

class ComboTree
{
public:

	/// @brief 状態
	enum class State
	{
		None,
		Success,
		Failure,
		Running,
	};


public:

	/// @brief 実行
	void Exec() { rootAttack->Exec(); }

	/// @brief 攻撃を追加する
    /// @param attack 
    void AddAttack(std::unique_ptr<Attack> attack) { allAttacks.push_back(std::move(attack)); }

	/// @brief ルート攻撃を設定する
	/// @param attack 
	void SetRootAttack(Attack* attack) { rootAttack = attack; }

	/// @brief 所有者を設定する
	/// @param owner 
	void SetOwner(Character* owner){for (auto& attack : allAttacks)attack->SetOwner(owner);}


private:

    // 生成されたすべての攻撃ステートのメモリを管理
    std::vector<std::unique_ptr<Attack>> allAttacks;

    // コンボの始点となる最初の一撃のポインタ（Playerはこれを呼び出す）
    Attack* rootAttack = nullptr;
};

