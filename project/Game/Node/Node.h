#pragma once
#include "GrowthEngine.h"

class Node
{
public:

	/// @brief ノードの状態
	enum class State
	{
		// 成功
		Success,

		// 失敗
		Failure,

		// 実行中
		Running
	};


public:

	/// @brief コンストラクタ
	Node();

	/// @brief コンストラクタ
	virtual ~Node() = default;

	/// @brief 実行
	/// @return ノードの状態
	virtual State Exec() = 0;


protected:

	/// @brief エンジン
	const GrowthEngine* engine_ = nullptr;
};

