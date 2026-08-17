#pragma once
#include "GrowthEngine.h"

class Character;

class Action
{
public:

	/// @brief ブレークポイント
	struct Breakpoints
	{
		bool onExec = false;
		bool onUpdate = false;
		bool onExit = false;
		bool onReset = false;
	};


public:

	/// @brief コンストラクタ
	/// @param owner 
	Action(Character* owner) : owner_(owner) { engine_ = GrowthEngine::GetInstance(); }

	/// @brief 仮想デストラクタ
	virtual ~Action() = default;

	/// @brief 実行
	virtual void Exec();

	/// @brief 更新処理
	virtual void Update();

	/// @brief リセット
	virtual void Reset();

	/// @brief 終了、中断
	virtual void Exit();

	/// @brief 実行したかどうか
	/// @return 
	bool IsExec() const { return isExec_; }

	/// @brief 成功したかどうか
	/// @return 
	bool IsSuccess() const { return isSuccess_; }

	/// @brief 失敗したかどうか
	/// @return 
	bool IsFailure() const { return isFailure_; }

	/// @brief 使用中かどうか
	/// @return 
	virtual bool IsUse() const { return false; }

	/// @brief ブレークポイントを取得する
	/// @return 
	Breakpoints& GetBreakpoints() { return breakpoints_; }

	/// @brief 所有者を設定する
	/// @param owner 
	void SetOwner(Character* owner) { owner_ = owner; }


protected:

	/// @brief エンジン
	const GrowthEngine* engine_ = nullptr;

	/// @brief 所有者
	Character* owner_ = nullptr;


private:

	/// @brief 実行中かどうか
	bool isExec_ = false;

	/// @brief 成功したかどうか
	bool isSuccess_ = false;

	/// @brief 失敗したかどうか
	bool isFailure_ = false;


protected:

	// ブレイクポイントのチェック
	void BreakpointOnExec();
	void BreakpointOnUpdate();
	void BreakpointOnExit();
	void BreakpointOnReset();

	/// @brief ブレークポイント
	Breakpoints breakpoints_;
};

