#pragma once
#include "GrowthEngine.h"

class GameScene : public Scene
{
public:

	/// @brief コンストラクタ
	/// @param sceneManager 
	GameScene(SceneManager* sceneManager) : Scene(sceneManager) {}

	/// @brief 初期化
	void Initialize() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;

private:

	std::unique_ptr<LightDirectional> light_ = nullptr;

	std::unique_ptr<AudioObject> object_ = nullptr;

	std::unique_ptr<PrimitiveSkinningModel> model_ = nullptr;

	std::unique_ptr<PrimitiveAnimationModel> animationModel_ = nullptr;

	std::unique_ptr<GameCamera2D> gameCamera2D_ = nullptr;

	std::unique_ptr<GameCamera3D> gameCamera_ = nullptr;
};

