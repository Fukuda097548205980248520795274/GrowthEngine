#pragma once
#include "../StageData/StageData.h"

class GameScene;
class MotionManager;

class StageSpawner
{
public:

	/// @brief コンストラクタ
	/// @param scene 
	StageSpawner(GameScene* scene) : scene_(scene) {}

	/// @brief デストラクタ
	~StageSpawner() = default;

	/// @brief 初期化
	void Initialize();

	/// @brief 実体を生成する
	/// @param data 
	bool SpawnActualEntity(PlacementData& data);

	/// @brief 実体を生成する（戦闘エリアの情報も渡す）
	/// @param data 
	/// @param battleAreas 
	bool SpawnActualEntity(PlacementData& data,BattleArea* battleAreas);

	/// @brief 実体を削除する
	/// @param data 
	void DeleteActualEntity(PlacementData& data);

	/// @brief 自動生成された武器をすべて削除する
	void DeleteAllAutoSpawnedWeapons();


private:

	/// @brief ゲームシーン
	GameScene* scene_ = nullptr;

	/// @brief モーションマネージャ
	MotionManager* motionManager_ = nullptr;


	/// @brief 戦闘エリアのリスト
	std::vector<std::unique_ptr<BattleArea>> battleAreas_;

	/// @brief 自動生成された武器のマップ（武器のポインタをキーにして、配置データを保持する）
	std::unordered_map<void*, void*> autoSpawnedWeaponsMap_;
};

