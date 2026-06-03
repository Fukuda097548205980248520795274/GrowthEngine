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
	void SpawnActualEntity(PlacementData& data);

	/// @brief 実体を削除する
	/// @param data 
	void DeleteActualEntity(PlacementData& data);


private:

	/// @brief ゲームシーン
	GameScene* scene_ = nullptr;

	/// @brief モーションマネージャ
	MotionManager* motionManager_ = nullptr;
};

