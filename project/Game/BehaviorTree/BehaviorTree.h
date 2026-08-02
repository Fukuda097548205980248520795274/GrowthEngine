#pragma once
#include "Node/Node.h"

class Character;

class BehaviorTree
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

	/// @brief コンストラクタ
	/// @param root ルートノード
	BehaviorTree(std::unique_ptr<Node> root, const std::string& name) : root_(std::move(root)),name_(name) {}

	/// @brief 仮想デストラクタ
	virtual ~BehaviorTree() = default;

	/// @brief 実行
	State Exec();

	/// @brief ルートノードを取得する
	/// @return 
	Node* GetRoot() const { return root_.get(); }

	/// @brief 中断処理
	void Abort() { if (root_) root_->Abort(); }

	/// @brief 現在の状態を取得する
	/// @return 
	State GetCurrentState() const { return currentState_; }

	/// @brief 状態を初期化する
	void InitState() { currentState_ = State::None; }

	/// @brief ツリーの名前を取得する
	/// @return 
	const std::string& GetName() const { return name_; }

	/// @brief 所有者を設定する
	/// @param owner 
	void SetOwner(Character* owner) { if (root_) root_->SetOwner(owner); }

protected:

	/// @brief ルートノード
	std::unique_ptr<Node> root_ = nullptr;

	/// @brief 現在の状態
	State currentState_ = State::None;

	/// @brief ツリーの名前
	std::string name_{};
};

