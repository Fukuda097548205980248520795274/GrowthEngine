#pragma once
#include "Data/CollisionData/CollisionData.h"
#include "Handle/Handle.h"
#include <functional>
#include <string>

class CollisionInstanceAABB;

class CollisionBaseAABB
{
public:

	/// @brief コンストラクタ
	/// @param func 
	/// @param name 
	CollisionBaseAABB(std::function<void()> func, const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	CollisionInstanceAABB* CreateInstance();

	/// @brief ハンドルを取得する
	/// @return 
	Collision3DHandle GetHandle()const { return hCollision_; }

	/// @brief 衝突対象の設定
	/// @param hCollision 
	void SetTargetCollision(Collision3DHandle hCollision);

private:

	/// @brief ハンドル
	Collision3DHandle hCollision_ = 0;
};

