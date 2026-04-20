#pragma once
#include "GrowthEngine.h"

class Character;

class Attack
{
public:

	/// @brief コンストラクタ
	/// @param owner 
	Attack(Character* owner) : owner_(owner) { engine_ = GrowthEngine::GetInstance(); }

	/// @brief 仮想デストラクタ
	virtual ~Attack() = default;

	/// @brief 攻撃の実行
	virtual void Exec();

	/// @brief 更新処理
	virtual void Update();

	/// @brief 攻撃の終了、中断
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


protected:

	/// @brief エンジン
	const GrowthEngine* engine_ = nullptr;

	/// @brief 攻撃の所有者
	Character* owner_ = nullptr;


private:

	/// @brief 実行中かどうか
	bool isExec_ = false;

	/// @brief 成功したかどうか
	bool isSuccess_ = false;

	/// @brief 失敗したかどうか
	bool isFailure_ = false;
};

