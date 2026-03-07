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

	std::unique_ptr<PrimitiveStaticModel> animationModel_ = nullptr;

	std::unique_ptr<GameCamera2D> gameCamera2D_ = nullptr;

	std::unique_ptr<GameCamera3D> gameCamera_ = nullptr;

	std::unique_ptr<PrefabBaseStaticModel> modelPrefab_ = nullptr;

	PrefabInstanceStaticModel* instance1_ = nullptr;
	PrefabInstanceStaticModel* instance2_ = nullptr;

	std::unique_ptr<PostEffectRadialBlur> blur_ = nullptr;

	std::unique_ptr<Sprite> sprite_ = nullptr;


	std::unique_ptr<Collision2DBaseSprite> collisionA_ = nullptr;
	Collision2DInstanceSprite* collisionInstanceA_ = nullptr;

	std::unique_ptr<Collision2DBaseCircle> collisionB_ = nullptr;
	Collision2DInstanceCircle* collisionInstanceB_ = nullptr;



	enum TileID
	{
		kAir,
		kBlock,
		kBomb
	};

	std::string tileName[3] = { "Air", "Block", "Bomb" };

	int32_t id_ = kAir;

	int32_t stage_[12][18] = { 0 };
};

