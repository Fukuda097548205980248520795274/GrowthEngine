#pragma once
#include "../IObject.h"
#include "StageData/StageData.h"

class Player : public IObject
{
public:

	/// @brief コンストラクタ
	/// @param position 
	/// @param tiles 
	Player(const VectorInt3& position) : IObject(position) {}

	/// @brief 初期化
	/// @param tiles 
	/// @param model 
	void Initialize(PrimitiveStaticModel* model);

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;


private:

	// モデル
	PrimitiveStaticModel* model_ = nullptr;
};

