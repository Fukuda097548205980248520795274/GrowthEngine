#pragma once
#include "StageData/StageData.h"
#include "IObject/Player/Player.h"

class Stage
{
public:

	/// @brief 初期化
	/// @param stageData 
	void Initialize(const StageData* stageData);

	/// @brief リセット
	void Reset(const StageData* stageData);

	/// @brief 更新処理
	void Update();

	/// @brief 描画処理
	void Draw();


private:

	/// @brief プレイヤー
	std::unique_ptr<Player> player_ = nullptr;

	/// @brief オブジェクトリスト
	std::list<std::unique_ptr<IObject>> objects_;


	// プレイヤーモデル
	std::unique_ptr<Render3DStaticModel> playerModel_ = nullptr;

	// 壁モデルプレハブ
	std::unique_ptr<PrefabBaseStaticModel> wallModelPrefab_ = nullptr;


	/// @brief タイル
	std::vector<std::vector<StageData::Tile>> tiles_;
};

