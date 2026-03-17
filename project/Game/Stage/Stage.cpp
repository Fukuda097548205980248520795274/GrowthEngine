#include "Stage.h"
#include <cassert>
#include "IObject/Wall/Wall.h"

/// @brief 初期化
/// @param stageData 
void Stage::Initialize(const StageData* stageData)
{
	// nullptrチェック
	assert(stageData);

	// タイルを取得する
	tiles_ = stageData->tiles_;


	// プレイヤーモデル読み込み
	playerModel_ = std::make_unique<PrimitiveStaticModel>(GrowthEngine::GetInstance()->LoadModel("./Assets/Models/AnimatedCube", "AnimatedCube.gltf"), "Player");

	// 壁モデルプレハブ
	wallModelPrefab_ = 
		std::make_unique<PrefabBaseStaticModel>(GrowthEngine::GetInstance()->LoadModel("./Assets/Models/AnimatedCube", "AnimatedCube.gltf"), 500, "Wall");


	// タイルのデータをめぐる
	for (int32_t i = 0; i < static_cast<int32_t>(tiles_.size()); ++i)
		for (int32_t j = 0; j < static_cast<int32_t>(tiles_[i].size()); ++j)
		{
			switch (tiles_[i][j])
			{
			case StageData::Tile::Player:
				// プレイヤー
				player_ = std::make_unique<Player>(VectorInt3(j, 0, i));
				player_->Initialize(playerModel_.get());
				break;

			case StageData::Tile::Wall:
				// 壁
				std::unique_ptr<Wall> wall = std::make_unique<Wall>(VectorInt3(j, 0, i));
				wall->Initialize(wallModelPrefab_->CreateInstance());
				objects_.push_back(std::move(wall));

				break;
			}
		}
}

/// @brief リセット
void Stage::Reset(const StageData* stageData)
{
	// nullptrチェック
	assert(stageData);

	// タイルを取得する
	tiles_ = stageData->tiles_;

	// プレイヤーを削除
	player_.reset();

	// タイルのデータをめぐる
	for (int32_t i = 0; i < static_cast<int32_t>(tiles_.size()); ++i)
		for (int32_t j = 0; j < static_cast<int32_t>(tiles_[i].size()); ++j)
		{
			switch (tiles_[i][j])
			{
			case StageData::Tile::Player:
				// プレイヤー
				player_ = std::make_unique<Player>(VectorInt3(j, 0, i));
				player_->Initialize(playerModel_.get());
				break;

			case StageData::Tile::Wall:
				// 壁
				std::unique_ptr<Wall> wall = std::make_unique<Wall>(VectorInt3(j, 0, i));
				wall->Initialize(wallModelPrefab_->CreateInstance());
				objects_.push_back(std::move(wall));

				break;
			}
		}
}

/// @brief 更新処理
void Stage::Update()
{
	// プレイヤーが生成されなければ処理しない
	if (player_ == nullptr)
		return;

	// プレイヤーの更新
	player_->Update();

	// オブジェクト更新処理
	for (auto& object : objects_)object->Update();

	// 終了したオブジェクトを消す
	objects_.remove_if([](std::unique_ptr<IObject>& object) {if (object->IsFinished()) { return true; }return false; });
}

/// @brief 描画処理
void Stage::Draw()
{
	// プレイヤー描画処理
	if (player_)player_->Draw();

	// オブジェクト描画処理
	for (auto& object : objects_)object->Draw();
}