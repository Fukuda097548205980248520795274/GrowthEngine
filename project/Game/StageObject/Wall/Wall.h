#pragma once
#include "../StageObject.h"

class Wall : public StageObject
{
public:

	// 壁の初期化データ
	struct InitData
	{
		/// @brief 壁の位置
		Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

		/// @brief 壁の回転
		float rotateY = 0.0f;

		/// @brief 壁のスケール
		Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

		// 壁の衝突判定
		Collision3DInstanceOBB* collision = nullptr;
	};


public:

	/// @brief コンストラクタ
	/// @param initData 
	Wall();

	/// @brief デストラクタ
	~Wall();

	/// @brief 初期化
	/// @param initData 
	void Initialize(const InitData& initData);

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 描画処理
	void Draw() override;


private:

	/// @brief 床の衝突判定
	Collision3DInstanceOBB* collision_ = nullptr;


public:

	/// @brief デバッグUIを描画する
	/// @param placementData 
	/// @param placementList 
	/// @param history 
	/// @param isDirty 
	void DrawDebugUI(PlacementData* placementData, std::vector<PlacementData>& placementList, StageEditorHistory* history, bool* isDirty) override;
};