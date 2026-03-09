#pragma once
#include "GrowthEngine.h"

class Object
{
public:

	/// @brief コンストラクタ
	/// @param position 
	Object(const Vector2& position);

	/// @brief 更新処理
	virtual void Update();


protected:

	/// @brief ワールドトランスフォーム
	std::unique_ptr<WorldTransform2D> worldTransform_ = nullptr;
};

