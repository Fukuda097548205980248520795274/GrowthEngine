#include "GameScene.h"

/// @brief 初期化
void GameScene::Initialize()
{
	engine_->LoadCamera3D("MainCamera");

	engine_->LoadSound("TEST_audio", engine_->LoadAudio("./Assets/Sounds/bgm/season_goes_around.mp3"));

	// プレイヤーのモデルの生成と初期化
	playerModel_ = std::make_unique<Render3DSkinningModel>(engine_->LoadModel("./Assets/Models/bone", "bone.gltf"),
		engine_->LoadAnimation("./Assets/Models/bone", "bone.gltf"), engine_->LoadSkeleton("./Assets/Models/bone", "bone.gltf"), "Player_Model");

	// 敵のモデルの生成と初期化
	enemyModel_ = std::make_unique<Render3DSkinningModel>(engine_->LoadModel("./Assets/Models/bone", "bone.gltf"),
		engine_->LoadAnimation("./Assets/Models/bone", "bone.gltf"), engine_->LoadSkeleton("./Assets/Models/bone", "bone.gltf"), "Enemy_Model");

	// プレイヤーの生成と初期化
	Character::InitData playerInitData;
	playerInitData.position = Vector3(0.0f, 0.0f, 0.0f);
	playerInitData.hp = 100;
	playerInitData.model_ = playerModel_.get();
	player_ = std::make_unique<Player>(playerInitData);
	player_->Initialize();

	// 敵の生成と初期化
	Character::InitData enemyInitData;
	enemyInitData.position = Vector3(5.0f, 0.0f, 0.0f);
	enemyInitData.hp = 100;
	enemyInitData.model_ = enemyModel_.get();
	enemy_ = std::make_unique<NPC>(enemyInitData, Character::CharacterTag::EnemySide);
	enemy_->Initialize();
}

/// @brief 更新処理
void GameScene::Update()
{
	// プレイヤーの更新
	player_->Update();

	// 敵の更新
	enemy_->Update();
}

/// @brief 描画処理
void GameScene::Draw()
{
	// プレイヤーの描画
	player_->Draw();

	// 敵の描画
	enemy_->Draw();
}