#pragma once
#include "../IObject.h"

class Wall : public IObject
{
public:

	/// @brief コンストラクタ
	/// @param position 
	Wall(const VectorInt3& position) : IObject(position) {}

	/// @brief 初期化
	/// @param model 
	void Initialize(PrefabInstanceStaticModel* model);

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;


private:

	// モデル
	PrefabInstanceStaticModel* model_ = nullptr;
};

