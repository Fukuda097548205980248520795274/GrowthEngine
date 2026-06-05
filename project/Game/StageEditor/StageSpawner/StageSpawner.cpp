#include "StageSpawner.h"
#include "Scene/GameScene/GameScene.h"

/// @brief 初期化
void StageSpawner::Initialize()
{
	// モーションマネージャのインスタンスを取得
	motionManager_ = MotionManager::GetInstance();
}

/// @brief 実体を生成する
/// @param data 
void StageSpawner::SpawnActualEntity(PlacementData& data)
{
	if (data.instancePtr != nullptr)
	{
		DeleteActualEntity(data);
	}

	// キャラクター
	if (data.category == EditCategory::Character)
	{
		// int から CharacterTag にキャスト
		Character::CharacterTag tag = static_cast<Character::CharacterTag>(data.subType);

		// NPCの初期化データの作成
		Character::InitData initData;
		initData.position = data.position;
		initData.hp = data.hp;
		initData.rotateY = data.rotateY;
		initData.model_ = nullptr; // モデルは後で設定する
		initData.weapon = nullptr; // 武器は後で設定する
		initData.behaviorTreeName = data.behaviorScriptName;
		initData.hStandMotion = data.standMotion.handle;
		initData.hStanceMotion = data.stanceMotion.handle;
		initData.hWalkMotion = data.walkMotion.handle;
		initData.hDashMotion = data.dashMotion.handle;
		initData.hAvoidFrontMotion = data.avoidFrontMotion.handle;
		initData.hAvoidBackMotion = data.avoidBackMotion.handle;
		initData.hAvoidLeftMotion = data.avoidLeftMotion.handle;
		initData.hAvoidRightMotion = data.avoidRightMotion.handle;

		// タグに応じて、NPCの初期化データを設定する
		Character* newCharacter = scene_->CreateCharacter(initData, tag);

		// ポインタを保存しておく
		data.instancePtr = newCharacter;
	}
	else if (data.category == EditCategory::Object)
	{
		// オブジェクト

		StageObject::StageObjectTag tag = static_cast<StageObject::StageObjectTag>(data.subType);

		if (tag == StageObject::StageObjectTag::Floor)
		{
			// 床の生成処理
			Floor::InitData initData;
			initData.position = data.position;
			initData.scale = data.scale;

			Floor* newFloor = scene_->CreateFloorObject(initData);
			data.instancePtr = newFloor;
		}
	}
	else if (data.category == EditCategory::Weapon)
	{
		// 武器
		Weapon::InitData initData;
		initData.position = data.position;
		initData.durability = data.durability;
		initData.attackPower = data.attackPower;
		initData.category = static_cast<WeaponCategory>(data.subType);
		initData.model = nullptr; // モデルは後で設定する

		Weapon* newWeapon = scene_->CreateWeapon(initData);
		data.instancePtr = newWeapon;
	}
}

/// @brief 実体を削除する
/// @param data 
void StageSpawner::DeleteActualEntity(PlacementData& data)
{
	if (data.category == EditCategory::Character)
	{
		Character* character = static_cast<Character*>(data.instancePtr);
		character->Delete();
	}
	else if (data.category == EditCategory::Object)
	{
		StageObject* stageObject = static_cast<StageObject*>(data.instancePtr);
		stageObject->Delete();
	}
	else if (data.category == EditCategory::Weapon)
	{
		Weapon* weapon = static_cast<Weapon*>(data.instancePtr);
		weapon->Delete();
	}
}