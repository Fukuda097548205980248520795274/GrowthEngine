#pragma once
#include "GrowthEngine.h"

class TitleScene : public Scene
{
public:

	/// @brief コンストラクタ
	/// @param sceneManager 
	TitleScene(SceneManager* sceneManager) : Scene(sceneManager) {}

	/// @brief 初期化
	void Initialize() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;


private:

	/// @brief プレハブのチューブ
	std::unique_ptr<PrefabBaseTube> prefabTube_;
	PrefabInstanceTube* instanceTube0_ = nullptr;
	PrefabInstanceTube* instanceTube1_ = nullptr;
	PrefabInstanceTube* instanceTube2_ = nullptr;
};

