#pragma once
#include "GrowthEngine.h"

class Object
{
public:

	/// @brief コンストラクタ
	Object();

	/// @brief 更新処理
	virtual void Update();


protected:

	/// @brief ワールドトランスフォーム
	std::unique_ptr<WorldTransform2D> worldTransform_ = nullptr;
};

