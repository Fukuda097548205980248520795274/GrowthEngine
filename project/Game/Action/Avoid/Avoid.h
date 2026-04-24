#pragma once
#include "../Action.h"

class Avoid : public Action
{
public:

	/// @brief コンストラクタ
	/// @param character 
	/// @param avoidDirection 
	Avoid(Character* character, const Vector3& avoidDirection) : Action(character), avoidDirection_(avoidDirection) {}

	/// @brief 実行
	void Exec() override;

	/// @brief 更新処理
	void Update() override;


private:

	/// @brief 回避方向
	Vector3 avoidDirection_ = Vector3(0.0f, 0.0f, 0.0f);
};

