#pragma once
#include "../StageObject.h"

// 床の初期化データ
struct FloorInitData
{
	/// @brief 床の位置
	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

	// 地面の衝突判定
	Collision3DInstanceAABB* collision = nullptr;

	/// @brief 地面のモデル
	PrefabInstanceStaticModel* model = nullptr;
};

/// @brief 床
class Floor : public StageObject
{
public:

	/// @brief コンストラクタ
	/// @param initData 
	Floor();

	/// @brief 初期化
	/// @param initData 
	void Initialize(const FloorInitData& initData);

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 描画処理
	void Draw();


private:

	/// @brief 床の衝突判定
	Collision3DInstanceAABB* collision_ = nullptr;

	/// @brief 床のモデル
	PrefabInstanceStaticModel* model_ = nullptr;
};

