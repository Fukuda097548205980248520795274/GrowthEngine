#pragma once
#include "GrowthEngine.h"

class Effect
{
public:

	/// @brief 仮想デストラクタ
	virtual  ~Effect() = default;

	/// @brief 更新処理
	virtual void Update();

	/// @brief 終了したかどうか
	/// @return 
	bool IsFinished() const { return isFinished_; }


protected:

	/// @brief エンジンのインスタンス
	const GrowthEngine* engine_ = GrowthEngine::GetInstance();

	/// @brief 終了フラグ
	bool isFinished_ = false;

	/// @brief 開始時間
	float startTime_ = 0.0f;

	/// @brief 経過時間
	float duration_ = 0.0f;
};

