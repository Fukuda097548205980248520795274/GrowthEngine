#pragma once
#include "GrowthEngine.h"

class IObject
{
public:

	/// @brief コンストラクタ
	/// @param position 
	IObject(const VectorInt3& position);

	/// @brief 仮想デストラクタ
	virtual ~IObject() = default;

	/// @brief 更新処理
	virtual void Update();

	/// @brief 描画処理
	virtual void Draw() = 0;

	/// @brief 終了したかどうか
	/// @return 
	bool IsFinished()const { return isFinished_; }


protected:

	/// @brief ワールドトランスフォーム
	std::unique_ptr<WorldTransform3D> worldTransform_ = nullptr;

	// 位置
	VectorInt3 position_ = VectorInt3(0, 0, 0);

	/// @brief 終了したかどうか
	bool isFinished_ = false;

	/// @brief グリッドサイズ
	static constexpr float kGridSize = 2.0f;
};

