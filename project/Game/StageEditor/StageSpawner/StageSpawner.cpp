#include "StageSpawner.h"
#include "Scene/GameScene/GameScene.h"
#include "../StageEditorUI/StageEditorUIHelper/StageEditorUIHelper.h"

/// @brief 初期化
void StageSpawner::Initialize()
{
	// モーションマネージャのインスタンスを取得
	motionManager_ = MotionManager::GetInstance();
}

/// @brief 実体を生成する
/// @param data 
/// @param weaponData 
void StageSpawner::SpawnActualEntity(PlacementData& data, bool isRuntime)
{
	if (data.instancePtr != nullptr)
	{
		DeleteActualEntity(data);
	}

	TemplateData tData;
	if (!StageEditorUIHelper::LoadPrefab(data.templateName, tData))
		return;

	data.category = tData.category;
	data.subType = tData.subType;

	// キャラクター
	if (data.category == EditCategory::Character)
	{
		// int から CharacterTag にキャスト
		CharacterTag tag = static_cast<CharacterTag>(data.subType);

		// 初期化データの作成
		CharacterInitData initData;
		initData.position = data.position;
		initData.aggressiveness = tData.aggressiveness;
		initData.hp = tData.hp;
		initData.rotateY = data.rotate_.y;
		initData.guardGage_ = tData.guardGage;
		initData.guardRecoveryTime = tData.guardRecoveryTime;
		initData.model_ = nullptr; // モデルは後で設定する
		initData.weapon = nullptr; // 武器は後で設定する
		initData.hStandMotion = tData.standMotion.handle;
		initData.hStanceMotion = tData.stanceMotion.handle;
		initData.hWalkMotion = tData.walkMotion.handle;
		initData.hDashMotion = tData.dashMotion.handle;
		initData.hAvoidFrontMotion = tData.avoidFrontMotion.handle;
		initData.hAvoidBackMotion = tData.avoidBackMotion.handle;
		initData.hAvoidLeftMotion = tData.avoidLeftMotion.handle;
		initData.hAvoidRightMotion = tData.avoidRightMotion.handle;
		initData.hGuardMotion = tData.guardMotion.handle;


		// 武器の生成
		Weapon* newWeapon = nullptr;
		if (isRuntime && strlen(tData.equipWeaponPrefabName) > 0)
		{
			TemplateData weaponData;

			// 武器プレハブのデータを読み込む
			if (StageEditorUIHelper::LoadPrefab(tData.equipWeaponPrefabName, weaponData))
			{
				if (weaponData.category == EditCategory::Weapon)
				{
					Weapon::InitData weaponInitData;
					weaponInitData.position = data.position;
					weaponInitData.durability = weaponData.durability;
					weaponInitData.attackPower = weaponData.attackPower;
					weaponInitData.isUnbreakable = weaponData.isUnbreakable;
					weaponInitData.category = static_cast<WeaponCategory>(weaponData.subType);
					weaponInitData.model = nullptr;

					// 武器の実体を生成
					newWeapon = scene_->CreateWeapon(weaponInitData, weaponData.behaviorTrees, weaponData.comboTrees);
				}
			}
		}

		// キャラクターの初期化データに武器を渡す
		initData.weapon = newWeapon;

		// タグに応じて、NPCの初期化データを設定する
		Character* newCharacter = scene_->CreateCharacter(initData, tag, tData.behaviorTrees, tData.comboTrees, data.name);

		// 武器の所有者を設定
		if (newWeapon)
			newCharacter->GrabWeapon(newWeapon);

		// ポインタを保存しておく
		data.instancePtr = newCharacter;

		// 自動生成された武器をマップに追加
		if (newWeapon)
			autoSpawnedWeaponsMap_[newCharacter] = newWeapon;
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
		else if (tag == StageObject::StageObjectTag::Wall)
		{
			// 壁の生成処理
			Wall::InitData initData;
			initData.position = data.position;
			initData.scale = data.scale;
			initData.rotateY = data.rotate_.y;
			Wall* newWall = scene_->CreateWallObject(initData);
			data.instancePtr = newWall;
		}
		else if (tag == StageObject::StageObjectTag::StaticEventTrigger)
		{
			// 静的イベントトリガーの生成処理
			StaticEventTrigger::InitData initData;
			initData.position = data.position;
			initData.scale = data.scale;
			initData.eventType = tData.eventType;
			initData.isStartBattleArea = tData.isBattleAreaStart;
			initData.isGameClear = tData.isGameClear;
			initData.navMeshGroupId = tData.targetNavMeshGroupId;
			initData.isNavMeshEnabled = tData.targetNavMeshState;

			// イベントトリガーの種類に応じて、ステージデータファイル名またはカットシーン名を設定
			if(tData.eventType == static_cast<int32_t>(StaticEventTrigger::EventType::ObjectSpawn))
				strcpy_s(initData.eventStageDataFileName, sizeof(initData.eventStageDataFileName), tData.eventStageDataFileName);
			else if(tData.eventType == static_cast<int32_t>(StaticEventTrigger::EventType::PlayCutscene))
				strcpy_s(initData.eventStageDataFileName, sizeof(initData.eventStageDataFileName), tData.eventCutsceneName);

			StaticEventTrigger* newTrigger = scene_->CreateStaticEventTrigger(initData);
			data.instancePtr = newTrigger;
		}
		else if (tag == StageObject::StageObjectTag::CameraGuard)
		{
			// カメラガードの生成処理
			CameraGuard::InitData initData;
			initData.position = data.position;
			initData.scale = data.scale;
			CameraGuard* newCameraGuard = scene_->CreateCameraGuard(initData);
			data.instancePtr = newCameraGuard;
		}
	}
	else if (data.category == EditCategory::Weapon)
	{
		// 武器
		Weapon::InitData initData;
		initData.position = data.position;
		initData.durability = tData.durability;
		initData.attackPower = tData.attackPower;
		initData.isUnbreakable = tData.isUnbreakable;
		initData.category = static_cast<WeaponCategory>(tData.subType);
		initData.model = nullptr; // モデルは後で設定する

		Weapon* newWeapon = scene_->CreateWeapon(initData, tData.behaviorTrees, tData.comboTrees);
		data.instancePtr = newWeapon;
	}
}

/// @brief 実体を生成する（戦闘エリアの情報も渡す）
/// @param data 
/// @param battleAreas 
/// @param weaponData 
void StageSpawner::SpawnActualEntity(PlacementData& data, BattleArea* battleAreas, bool isRuntime)
{
	if (data.instancePtr != nullptr)
	{
		DeleteActualEntity(data);
	}

	TemplateData tData;
	if (!StageEditorUIHelper::LoadPrefab(data.templateName, tData))
		return;

	data.category = tData.category;
	data.subType = tData.subType;

	// キャラクター
	if (data.category == EditCategory::Character)
	{
		// int から CharacterTag にキャスト
		CharacterTag tag = static_cast<CharacterTag>(data.subType);

		// NPCの初期化データの作成
		CharacterInitData initData;
		initData.position = data.position;
		initData.aggressiveness = tData.aggressiveness;
		initData.hp = tData.hp;
		initData.rotateY = data.rotate_.y;
		initData.guardGage_ = tData.guardGage;
		initData.guardRecoveryTime = tData.guardRecoveryTime;
		initData.model_ = nullptr; // モデルは後で設定する
		initData.weapon = nullptr; // 武器は後で設定する
		initData.hStandMotion = tData.standMotion.handle;
		initData.hStanceMotion = tData.stanceMotion.handle;
		initData.hWalkMotion = tData.walkMotion.handle;
		initData.hDashMotion = tData.dashMotion.handle;
		initData.hAvoidFrontMotion = tData.avoidFrontMotion.handle;
		initData.hAvoidBackMotion = tData.avoidBackMotion.handle;
		initData.hAvoidLeftMotion = tData.avoidLeftMotion.handle;
		initData.hAvoidRightMotion = tData.avoidRightMotion.handle;
		initData.hGuardMotion = tData.guardMotion.handle;

		// 武器の生成
		Weapon* newWeapon = nullptr;
		if (isRuntime && strlen(tData.equipWeaponPrefabName) > 0)
		{
			TemplateData weaponData;
			if (!StageEditorUIHelper::LoadPrefab(tData.equipWeaponPrefabName, weaponData))
			{
				if (weaponData.category == EditCategory::Weapon)
				{
					Weapon::InitData weaponInitData;
					weaponInitData.position = data.position;
					weaponInitData.durability = weaponData.durability;
					weaponInitData.attackPower = weaponData.attackPower;
					weaponInitData.isUnbreakable = weaponData.isUnbreakable;
					weaponInitData.category = static_cast<WeaponCategory>(weaponData.subType);
					weaponInitData.model = nullptr;

					// 武器の実体を生成
					newWeapon = scene_->CreateWeapon(weaponInitData, weaponData.behaviorTrees, weaponData.comboTrees);
				}
			}
		}

		// キャラクターの初期化データに武器を渡す
		initData.weapon = newWeapon;

		// タグに応じて、NPCの初期化データを設定する
		Character* newCharacter = scene_->CreateCharacter(initData, tag, tData.behaviorTrees, tData.comboTrees, data.name);

		// 武器の所有者を設定
		if (newWeapon)
			newCharacter->GrabWeapon(newWeapon);

		// ポインタを保存しておく
		data.instancePtr = newCharacter;

		// 自動生成された武器をマップに追加
		if (newWeapon)
			autoSpawnedWeaponsMap_[newCharacter] = newWeapon;

		// 敵キャラクターの場合、戦闘エリアの敵リストに追加する
		if (newCharacter->IsEnemySide())
		{
			battleAreas->enemies.push_back(static_cast<NPC*>(newCharacter));
		}
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

			battleAreas->stageObjects.push_back(newFloor);
		}
		else if (tag == StageObject::StageObjectTag::Wall)
		{
			// 壁の生成処理
			Wall::InitData initData;
			initData.position = data.position;
			initData.scale = data.scale;
			initData.rotateY = data.rotate_.y;
			Wall* newWall = scene_->CreateWallObject(initData);
			data.instancePtr = newWall;

			battleAreas->stageObjects.push_back(newWall);
		}
		else if (tag == StageObject::StageObjectTag::StaticEventTrigger)
		{
			// 静的イベントトリガーの生成処理
			StaticEventTrigger::InitData initData;
			initData.position = data.position;
			initData.scale = data.scale;
			initData.eventType = tData.eventType;
			initData.isStartBattleArea = tData.isBattleAreaStart;
			initData.isGameClear = tData.isGameClear;
			initData.navMeshGroupId = tData.targetNavMeshGroupId;
			initData.isNavMeshEnabled = tData.targetNavMeshState;

			// イベントトリガーの種類に応じて、ステージデータファイル名またはカットシーン名を設定
			if (tData.eventType == static_cast<int32_t>(StaticEventTrigger::EventType::ObjectSpawn))
				strcpy_s(initData.eventStageDataFileName, sizeof(initData.eventStageDataFileName), tData.eventStageDataFileName);
			else if (tData.eventType == static_cast<int32_t>(StaticEventTrigger::EventType::PlayCutscene))
				strcpy_s(initData.eventStageDataFileName, sizeof(initData.eventStageDataFileName), tData.eventCutsceneName);

			StaticEventTrigger* newTrigger = scene_->CreateStaticEventTrigger(initData);
			data.instancePtr = newTrigger;

			battleAreas->stageObjects.push_back(newTrigger);
		}
		else if (tag == StageObject::StageObjectTag::CameraGuard)
		{
			// カメラガードの生成処理
			CameraGuard::InitData initData;
			initData.position = data.position;
			initData.scale = data.scale;
			CameraGuard* newCameraGuard = scene_->CreateCameraGuard(initData);
			data.instancePtr = newCameraGuard;

			battleAreas->stageObjects.push_back(newCameraGuard);
		}
	}
	else if (data.category == EditCategory::Weapon)
	{
		// 武器
		Weapon::InitData initData;
		initData.position = data.position;
		initData.durability = tData.durability;
		initData.attackPower = tData.attackPower;
		initData.isUnbreakable = tData.isUnbreakable;
		initData.category = static_cast<WeaponCategory>(tData.subType);
		initData.model = nullptr; // モデルは後で設定する

		Weapon* newWeapon = scene_->CreateWeapon(initData, tData.behaviorTrees, tData.comboTrees);
		data.instancePtr = newWeapon;
	}
}

/// @brief 実体を削除する
/// @param data 
void StageSpawner::DeleteActualEntity(PlacementData& data)
{
	// 既に実体がない場合は何もしない
	if (data.instancePtr == nullptr) return;

	if (data.category == EditCategory::Character)
	{
		Character* character = static_cast<Character*>(data.instancePtr);

		// 自動生成された武器がある場合は削除する
		auto it = autoSpawnedWeaponsMap_.find(character);
		if (it != autoSpawnedWeaponsMap_.end())
		{
			Weapon* weapon = static_cast<Weapon*>(it->second);
			if (weapon)
				weapon->Delete();
			autoSpawnedWeaponsMap_.erase(it);
		}

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

	// ポインタをクリア
	data.instancePtr = nullptr;
}

/// @brief 自動生成された武器をすべて削除する
void StageSpawner::DeleteAllAutoSpawnedWeapons()
{
	for (const auto& pair : autoSpawnedWeaponsMap_)
	{
		Weapon* weapon = static_cast<Weapon*>(pair.second);
		if (weapon)
			weapon->Delete();
	}
	autoSpawnedWeaponsMap_.clear();
}