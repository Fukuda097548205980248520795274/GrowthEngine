#include "Scene/GameScene/GameScene.h"
#include "HUD/HP/BossHP/BossHP.h"
#include "HUD/Button/WeaponGetButton/WeaponGetButton.h"

/// @brief キャラクターを生成する
/// @param initData 
/// @return 
Character* GameScene::CreateCharacter(const CharacterInitData& initData, CharacterTag tag,
	const BehaviorTreeConfig& behaviorTreeConfig, const ComboTreeConfig& comboTreeConfig, const std::string& editorName)
{
	Character* character = nullptr;

	if (tag == CharacterTag::Player)
	{
		// すでにプレイヤーが存在する場合は削除する
		if (player_)
		{
			player_.reset();
			player_ = nullptr;
		}

		// すでにプレイヤーの武器が存在する場合は削除する
		if (playerWeapon_)
		{
			playerWeapon_.reset();
			playerWeapon_ = nullptr;
		}

		// すでにプレイヤーの体力バーが存在する場合は削除する
		if (playerHP_)
		{
			playerHP_.reset();
			playerHP_ = nullptr;
		}

		// すでにプレイヤーのレイジゲージが存在する場合は削除する
		if (playerRageGage_)
		{
			playerRageGage_.reset();
			playerRageGage_ = nullptr;
		}

		// すでに武器の耐久力ゲージが存在する場合は削除する
		if (weaponDurabilityGage_)
		{
			weaponDurabilityGage_.reset();
			weaponDurabilityGage_ = nullptr;
		}

		// プレイヤーの武器の生成と初期化
		Weapon::InitData playerWeaponInitData;
		playerWeaponInitData.position = Vector3(0.0f, 0.0f, 0.0f);
		playerWeaponInitData.model = oneHandedWeaponModel_->CreateInstance();
		playerWeaponInitData.durability = 0;
		playerWeaponInitData.attackPower = 1.0f;
		playerWeaponInitData.category = WeaponCategory::OneHanded;
		playerWeaponInitData.isUnbreakable = true;
		playerWeaponInitData.landingCollision = landingCollision_->CreateInstance();
		playerWeapon_ = std::make_unique<Weapon>(playerWeaponInitData);

		// 体力ゲージの生成と初期化
		HP::InitData hpInitData;
		hpInitData.position = Vector2(0.0f, 0.0f);
		hpInitData.hpFrameLeftSprite = hpFrameLeftSprite_->CreateInstance();
		hpInitData.hpFrameMiddleSprite = hpFrameMiddleSprite_->CreateInstance();
		hpInitData.hpFrameRightSprite = hpFrameRightSprite_->CreateInstance();
		hpInitData.hpLeftSprite = hpLeftSprite_->CreateInstance();
		hpInitData.hpMiddleSprite = hpMiddleSprite_->CreateInstance();
		hpInitData.hpRightSprite = hpRightSprite_->CreateInstance();
		hpInitData.delayHpLeftSprite = delayHpLeftSprite_->CreateInstance();
		hpInitData.delayHpMiddleSprite = delayHpMiddleSprite_->CreateInstance();
		hpInitData.delayHpRightSprite = delayHpRightSprite_->CreateInstance();
		hpInitData.hpSeparatorSprite = hpSeparatorSprite_->CreateInstance();
		hpInitData.alpha = 1.0f;
		hpInitData.scale = Vector2(0.5f, 0.5f);
		hpInitData.width = 1200;
		hpInitData.color = Vector3(0.25f, 1.0f, 0.25f);
		playerHP_ = std::make_unique<HP>();
		playerHP_->Initialize(hpInitData);

		// レイジゲージの生成と初期化
		Gage::InitData rageGageInitData;
		rageGageInitData.position = Vector2(0.0f, 0.0f);
		rageGageInitData.hpFrameLeftSprite = hpFrameLeftSprite_->CreateInstance();
		rageGageInitData.hpFrameMiddleSprite = hpFrameMiddleSprite_->CreateInstance();
		rageGageInitData.hpFrameRightSprite = hpFrameRightSprite_->CreateInstance();
		rageGageInitData.hpLeftSprite = hpLeftSprite_->CreateInstance();
		rageGageInitData.hpMiddleSprite = hpMiddleSprite_->CreateInstance();
		rageGageInitData.hpRightSprite = hpRightSprite_->CreateInstance();
		rageGageInitData.hpSeparatorSprite = hpSeparatorSprite_->CreateInstance();
		rageGageInitData.alpha = 1.0f;
		rageGageInitData.scale = Vector2(0.35f, 0.35f);
		rageGageInitData.width = 800;
		rageGageInitData.color = Vector3(0.25f, 0.25f, 1.0f);
		playerRageGage_ = std::make_unique<Gage>();
		playerRageGage_->Initialize(rageGageInitData);

		// 武器耐久ゲージの生成と初期化
		Gage::InitData weaponDurabilityGageInitData;
		weaponDurabilityGageInitData.position = Vector2(0.0f, 0.0f);
		weaponDurabilityGageInitData.hpFrameLeftSprite = hpFrameLeftSprite_->CreateInstance();
		weaponDurabilityGageInitData.hpFrameMiddleSprite = hpFrameMiddleSprite_->CreateInstance();
		weaponDurabilityGageInitData.hpFrameRightSprite = hpFrameRightSprite_->CreateInstance();
		weaponDurabilityGageInitData.hpLeftSprite = hpLeftSprite_->CreateInstance();
		weaponDurabilityGageInitData.hpMiddleSprite = hpMiddleSprite_->CreateInstance();
		weaponDurabilityGageInitData.hpRightSprite = hpRightSprite_->CreateInstance();
		weaponDurabilityGageInitData.hpSeparatorSprite = hpSeparatorSprite_->CreateInstance();
		weaponDurabilityGageInitData.alpha = 0.0f;
		weaponDurabilityGageInitData.scale = Vector2(0.3f, 0.3f);
		weaponDurabilityGageInitData.width = 300;
		weaponDurabilityGageInitData.color = Vector3(1.0f, 1.0f, 1.0f);
		weaponDurabilityGage_ = std::make_unique<Gage>();
		weaponDurabilityGage_->Initialize(weaponDurabilityGageInitData);


		// プレイヤーの生成処理
		CharacterInitData playerInitData = initData;
		playerInitData.hurtboxGroup = playerHurtboxGroup_.get();
		playerInitData.hitboxGroup = playerHitboxGroup_.get();
		playerInitData.landingCollision = landingCollision_->CreateInstance();
		playerInitData.wallTouchCollision = wallTouchCollision_->CreateInstance();
		playerInitData.eventTriggerCollision = eventTriggerCollision_->CreateInstance();
		playerInitData.model_ = playerModel_.get();
		playerInitData.attackTrail = playerTrail_.get();
		playerInitData.hpHUD = playerHP_.get();
		playerInitData.rageGageThresholds = { 20.0f };
		playerInitData.scene = this;
		player_ = std::make_unique<Player>();
		player_->Initialize(playerInitData, playerWeapon_.get());
		player_->InitComboTree(comboTreeConfig, comboTreeEditor_.get());
		player_->SetEditorName(editorName);
		player_->SetRageGageHud(playerRageGage_.get());
		player_->SetWeaponHpGageHud(weaponDurabilityGage_.get(), weaponKnifeSprite_, weaponGunSprite_);

		character = player_.get();


		// カメラ制御の初期化
		InitializeCameraControl();
	}
	else
	{
		CharacterInitData npcInitData = initData;
		npcInitData.scene = this;

		// NPCのモデルの生成と初期化
		std::unique_ptr<Render3DSkinningModel> npcModel = npcModelPool_->Acquire();
		npcModel->param_->isUpdate = true;

		if (tag == CharacterTag::EnemyNormal)
		{
			npcModel->param_->meshOutline[0].enableOutline = true;
			npcModel->param_->meshOutline[0].color = Vector4(0.6f, 0.0f, 0.0f, 1.0f);
		}
		else if (tag == CharacterTag::EnemyBoss)
		{
			npcModel->param_->meshOutline[0].enableOutline = true;
			npcModel->param_->meshOutline[0].color = Vector4(0.6f, 0.0f, 1.0f, 1.0f);
		}
		else if (tag == CharacterTag::Ally)
		{
			npcModel->param_->meshOutline[0].enableOutline = true;
			npcModel->param_->meshOutline[0].color = Vector4(0.1f, 0.1f, 0.1f, 1.0f);
		}
		else if (tag == CharacterTag::Vip)
		{
			npcModel->param_->meshOutline[0].enableOutline = true;
			npcModel->param_->meshOutline[0].color = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
		}

		// NPCのトレイルの生成と初期化
		std::unique_ptr<Trail3D> npcTrail = npcTrailPool_->Acquire();
		npcTrail->param_->isUpdate_ = true;

		// NPCのモデルを初期化データに設定する
		npcInitData.model_ = npcModel.get();
		npcInitData.attackTrail = npcTrail.get();

		// NPCの当たり判定グループの設定
		if (tag == CharacterTag::Ally || tag == CharacterTag::Vip)
		{
			npcInitData.hurtboxGroup = playerHurtboxGroup_.get();
			npcInitData.hitboxGroup = playerHitboxGroup_.get();
		}
		else if (tag == CharacterTag::EnemyNormal || tag == CharacterTag::EnemyBoss)
		{
			npcInitData.hurtboxGroup = enemyHurtboxGroup_.get();
			npcInitData.hitboxGroup = enemyHitboxGroup_.get();
		}

		// 着地判定グループの設定
		npcInitData.landingCollision = landingCollision_->CreateInstance();
		npcInitData.wallTouchCollision = wallTouchCollision_->CreateInstance();

		if (tag == CharacterTag::EnemyBoss)
		{
			BossHP::InitData bossHpInitData;
			bossHpInitData.position = Vector2(0.0f, 0.0f);
			bossHpInitData.hpFrameLeftSprite = hpFrameLeftSprite_->CreateInstance();
			bossHpInitData.hpFrameMiddleSprite = hpFrameMiddleSprite_->CreateInstance();
			bossHpInitData.hpFrameRightSprite = hpFrameRightSprite_->CreateInstance();
			bossHpInitData.hpFrontLeftSprite = hpLeftSprite_->CreateInstance();
			bossHpInitData.hpFrontMiddleSprite = hpMiddleSprite_->CreateInstance();
			bossHpInitData.hpFrontRightSprite = hpRightSprite_->CreateInstance();
			bossHpInitData.hpBackLeftSprite = hpBackLeftSprite_->CreateInstance();
			bossHpInitData.hpBackMiddleSprite = hpBackMiddleSprite_->CreateInstance();
			bossHpInitData.hpBackRightSprite = hpBackRightSprite_->CreateInstance();
			bossHpInitData.delayHpBackLeftSprite = delayHpLeftSprite_->CreateInstance();
			bossHpInitData.delayHpBackMiddleSprite = delayHpMiddleSprite_->CreateInstance();
			bossHpInitData.delayHpBackRightSprite = delayHpRightSprite_->CreateInstance();
			bossHpInitData.delayHpFrontLeftSprite = delayHpFrontLeftSprite_->CreateInstance();
			bossHpInitData.delayHpFrontMiddleSprite = delayHpFrontMiddleSprite_->CreateInstance();
			bossHpInitData.delayHpFrontRightSprite = delayHpFrontRightSprite_->CreateInstance();
			bossHpInitData.hpSeparatorSprite = hpSeparatorSprite_->CreateInstance();
			bossHpInitData.alpha = 1.0f;
			bossHpInitData.scale = Vector2(0.45f, 0.45f);
			bossHpInitData.width = 300;

			std::unique_ptr<BossHP> hp = std::make_unique<BossHP>();
			hp->Initialize(bossHpInitData);
			npcInitData.hpHUD = hp.get();

			huds_.push_back(std::move(hp));
		}
		else
		{
			// 体力ゲージの生成と初期化
			HP::InitData hpInitData;
			hpInitData.position = Vector2(0.0f, 0.0f);
			hpInitData.hpFrameLeftSprite = hpFrameLeftSprite_->CreateInstance();
			hpInitData.hpFrameMiddleSprite = hpFrameMiddleSprite_->CreateInstance();
			hpInitData.hpFrameRightSprite = hpFrameRightSprite_->CreateInstance();
			hpInitData.hpLeftSprite = hpLeftSprite_->CreateInstance();
			hpInitData.hpMiddleSprite = hpMiddleSprite_->CreateInstance();
			hpInitData.hpRightSprite = hpRightSprite_->CreateInstance();
			hpInitData.delayHpLeftSprite = delayHpLeftSprite_->CreateInstance();
			hpInitData.delayHpMiddleSprite = delayHpMiddleSprite_->CreateInstance();
			hpInitData.delayHpRightSprite = delayHpRightSprite_->CreateInstance();
			hpInitData.hpSeparatorSprite = hpSeparatorSprite_->CreateInstance();
			hpInitData.alpha = 1.0f;

			if (tag == CharacterTag::Ally)
			{
				hpInitData.scale = Vector2(0.75f, 0.75f);
				hpInitData.width = 150;
				hpInitData.color = Vector3(0.25f, 1.0f, 0.25f);
			}
			else if (tag == CharacterTag::EnemyNormal)
			{
				hpInitData.scale = Vector2(0.25f, 0.25f);
				hpInitData.width = 200;
				hpInitData.color = Vector3(1.0f, 0.25f, 0.25f);
			}


			std::unique_ptr<HP> hp = std::make_unique<HP>();
			hp->Initialize(hpInitData);
			npcInitData.hpHUD = hp.get();

			huds_.push_back(std::move(hp));
		}

		// NPCの生成処理
		std::unique_ptr<NPC> npc = npcPool_->Acquire();
		npc->Initialize(npcInitData, tag, navMesh_.get());
		npc->InitBehaviorTree(behaviorTreeConfig, behaviorTreeEditor_.get());
		npc->SetEditorName(editorName);
		character = npc.get();

		// NPCのリストに追加する
		npcs_.push_back(std::move(npc));
		npcModels_.push_back(std::move(npcModel));
		npcTrails_.push_back(std::move(npcTrail));
	}

	return character;
}

/// @brief 武器を生成する
/// @param position 
/// @return 
Weapon* GameScene::CreateWeapon(const Weapon::InitData& initData, const BehaviorTreeConfig& behaviorTreeConfig, const ComboTreeConfig& comboTreeConfig)
{
	Weapon* weapon = nullptr;

	WeaponGetButton::InitData buttonInitData;
	buttonInitData.buttonSprite = weaponGetButtonSpritePrefab_->CreateInstance();
	std::unique_ptr<WeaponGetButton> button = std::make_unique<WeaponGetButton>(buttonInitData);

	Weapon::InitData weaponInitData = initData;
	weaponInitData.landingCollision = landingCollision_->CreateInstance();
	weaponInitData.model = oneHandedWeaponModel_->CreateInstance();
	weaponInitData.button = button.get();

	// 武器の生成処理
	std::unique_ptr<Weapon> newWeapon = std::make_unique<Weapon>(weaponInitData);
	weapon = newWeapon.get();

	// 通常状態のステートツリーを設定する
	WeaponStateTreeSet noneStateTreeSet;
	noneStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.noneStateBT.c_str(), nullptr);
	noneStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.noneStateCT.xName_.c_str(), nullptr);
	noneStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.noneStateCT.yName_.c_str(), nullptr);
	noneStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.noneStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("None", noneStateTreeSet);


	// ダッシュ状態のステートツリーを設定する
	WeaponStateTreeSet dashStateTreeSet;
	dashStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.dashStateBT.c_str(), nullptr);
	dashStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.dashStateCT.xName_.c_str(), nullptr);
	dashStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.dashStateCT.yName_.c_str(), nullptr);
	dashStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.dashStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Dash", dashStateTreeSet);

	// 掴まれ状態のステートツリーを設定する
	WeaponStateTreeSet grabbedStateTreeSet;
	grabbedStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.grabbedStateBT.c_str(), nullptr);
	grabbedStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.grabbedStateCT.xName_.c_str(), nullptr);
	grabbedStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.grabbedStateCT.yName_.c_str(), nullptr);
	grabbedStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.grabbedStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Grabbed", grabbedStateTreeSet);

	// 掴み状態のステートツリーを設定する
	WeaponStateTreeSet grabbingStateTreeSet;
	grabbingStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.grabbingStateBT.c_str(), nullptr);
	grabbingStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.grabbingStateCT.xName_.c_str(), nullptr);
	grabbingStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.grabbingStateCT.yName_.c_str(), nullptr);
	grabbingStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.grabbingStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Grabbing", grabbingStateTreeSet);

	// ガード状態のステートツリーを設定する
	WeaponStateTreeSet guardStateTreeSet;
	guardStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.guardStateBT.c_str(), nullptr);
	guardStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.guardStateCT.xName_.c_str(), nullptr);
	guardStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.guardStateCT.yName_.c_str(), nullptr);
	guardStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.guardStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Guard", guardStateTreeSet);

	// 軽ダメージ状態のステートツリーを設定する
	WeaponStateTreeSet lightDamageStateTreeSet;
	lightDamageStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.lightDamageStateBT.c_str(), nullptr);
	lightDamageStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.lightDamageStateCT.xName_.c_str(), nullptr);
	lightDamageStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.lightDamageStateCT.yName_.c_str(), nullptr);
	lightDamageStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.lightDamageStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("LightDamage", lightDamageStateTreeSet);

	// 重ダメージ状態のステートツリーを設定する
	WeaponStateTreeSet heavyDamageStateTreeSet;
	heavyDamageStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.heavyDamageStateBT.c_str(), nullptr);
	heavyDamageStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.heavyDamageStateCT.xName_.c_str(), nullptr);
	heavyDamageStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.heavyDamageStateCT.yName_.c_str(), nullptr);
	heavyDamageStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.heavyDamageStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("HeavyDamage", heavyDamageStateTreeSet);

	// 倒れこみ状態のステートツリーを設定する
	WeaponStateTreeSet downFallingStateTreeSet;
	downFallingStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.downFallingStateBT.c_str(), nullptr);
	downFallingStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.downFallingStateCT.xName_.c_str(), nullptr);
	downFallingStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.downFallingStateCT.yName_.c_str(), nullptr);
	downFallingStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.downFallingStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("DownFalling", downFallingStateTreeSet);

	// ダウン状態のステートツリーを設定する
	WeaponStateTreeSet downLyingStateTreeSet;
	downLyingStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.downLyingStateBT.c_str(), nullptr);
	downLyingStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.downLyingStateCT.xName_.c_str(), nullptr);
	downLyingStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.downLyingStateCT.yName_.c_str(), nullptr);
	downLyingStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.downLyingStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("DownLying", downLyingStateTreeSet);

	// 起き上がり状態のステートツリーを設定する
	WeaponStateTreeSet downGettingUpStateTreeSet;
	downGettingUpStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.downGettingUpStateBT.c_str(), nullptr);
	downGettingUpStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.downGettingUpStateCT.xName_.c_str(), nullptr);
	downGettingUpStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.downGettingUpStateCT.yName_.c_str(), nullptr);
	downGettingUpStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.downGettingUpStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("DownGettingUp", downGettingUpStateTreeSet);

	// ダウン怯み状態のステートツリーを設定する
	WeaponStateTreeSet downStaggerStateTreeSet;
	downStaggerStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.downStaggerStateBT.c_str(), nullptr);
	downStaggerStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.downStaggerStateCT.xName_.c_str(), nullptr);
	downStaggerStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.downStaggerStateCT.yName_.c_str(), nullptr);
	downStaggerStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.downStaggerStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("DownStagger", downStaggerStateTreeSet);

	// 吹き飛び状態のステートツリーを設定する
	WeaponStateTreeSet blownAwayStateTreeSet;
	blownAwayStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.blownAwayStateBT.c_str(), nullptr);
	blownAwayStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.blownAwayStateCT.xName_.c_str(), nullptr);
	blownAwayStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.blownAwayStateCT.yName_.c_str(), nullptr);
	blownAwayStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.blownAwayStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("BlownAway", blownAwayStateTreeSet);

	// 吹き飛び落下状態のステートツリーを設定する
	WeaponStateTreeSet blownFallingStateTreeSet;
	blownFallingStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.blownFallingStateBT.c_str(), nullptr);
	blownFallingStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.blownFallingStateCT.xName_.c_str(), nullptr);
	blownFallingStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.blownFallingStateCT.yName_.c_str(), nullptr);
	blownFallingStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.blownFallingStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("BlownFalling", blownFallingStateTreeSet);

	// 弾き状態のステートツリーを設定する
	WeaponStateTreeSet repelStateTreeSet;
	repelStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.repelStateBT.c_str(), nullptr);
	repelStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.repelStateCT.xName_.c_str(), nullptr);
	repelStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.repelStateCT.yName_.c_str(), nullptr);
	repelStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.repelStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Repel", repelStateTreeSet);

	// 受け流し状態のステートツリーを設定する
	WeaponStateTreeSet deflectStateTreeSet;
	deflectStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.deflectStateBT.c_str(), nullptr);
	deflectStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.deflectStateCT.xName_.c_str(), nullptr);
	deflectStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.deflectStateCT.yName_.c_str(), nullptr);
	deflectStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.deflectStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Deflect", deflectStateTreeSet);

	// 弾かれ状態のステートツリーを設定する
	WeaponStateTreeSet repelledStateTreeSet;
	repelledStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.repelledStateBT.c_str(), nullptr);
	repelledStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.repelledStateCT.xName_.c_str(), nullptr);
	repelledStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.repelledStateCT.yName_.c_str(), nullptr);
	repelledStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.repelledStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Repelled", repelledStateTreeSet);

	// 受け流され状態のステートツリーを設定する
	WeaponStateTreeSet deflectedStateTreeSet;
	deflectedStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.deflectedStateBT.c_str(), nullptr);
	deflectedStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.deflectedStateCT.xName_.c_str(), nullptr);
	deflectedStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.deflectedStateCT.yName_.c_str(), nullptr);
	deflectedStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.deflectedStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Deflected", deflectedStateTreeSet);

	// 回避状態のステートツリーを設定する
	WeaponStateTreeSet avoidStateTreeSet;
	avoidStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.avoidStateBT.c_str(), nullptr);
	avoidStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.avoidStateCT.xName_.c_str(), nullptr);
	avoidStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.avoidStateCT.yName_.c_str(), nullptr);
	avoidStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.avoidStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Avoid", avoidStateTreeSet);

	// 死亡状態のステートツリーを設定する
	WeaponStateTreeSet deadStateTreeSet;
	deadStateTreeSet.behaviorTree = behaviorTreeEditor_->CreateTree(behaviorTreeConfig.deadStateBT.c_str(), nullptr);
	deadStateTreeSet.comboTreeX = comboTreeEditor_->CreateTree(comboTreeConfig.deadStateCT.xName_.c_str(), nullptr);
	deadStateTreeSet.comboTreeY = comboTreeEditor_->CreateTree(comboTreeConfig.deadStateCT.yName_.c_str(), nullptr);
	deadStateTreeSet.comboTreeB = comboTreeEditor_->CreateTree(comboTreeConfig.deadStateCT.bName_.c_str(), nullptr);
	newWeapon->SetStateTreeSet("Dead", deadStateTreeSet);

	// 武器のリストに追加する
	weapons_.push_back(std::move(newWeapon));

	// HUDのリストに追加する
	huds_.push_back(std::move(button));

	return weapon;
}

/// @brief 床オブジェクトを生成する
/// @param position 
/// @param scale 
/// @return 
Floor* GameScene::CreateFloorObject(const Floor::InitData& initData)
{
	// 床
	Floor::InitData floorInitData = initData;
	floorInitData.collision = floorCollision_->CreateInstance();

	std::unique_ptr<Floor> newFloor = std::make_unique<Floor>();
	newFloor->Initialize(floorInitData);
	Floor* floor = newFloor.get();

	// ステージオブジェクトのリストに追加する
	objects_.push_back(std::move(newFloor));

	return floor;
}

/// @brief 壁オブジェクトを生成する
/// @param initData 
/// @return 
Wall* GameScene::CreateWallObject(const Wall::InitData& initData)
{
	// 壁
	Wall::InitData wallInitData = initData;
	wallInitData.collision = wallCollision_->CreateInstance();

	std::unique_ptr<Wall> newWall = std::make_unique<Wall>();
	newWall->Initialize(wallInitData);
	Wall* wall = newWall.get();

	objects_.push_back(std::move(newWall));

	return wall;
}

/// @brief タイマーHUDを生成する
/// @param initData 
/// @return 
Timer* GameScene::CreateTimer(const Timer::InitData& initData)
{
	Timer::InitData timerInitData = initData;
	timerInitData.timerSprite[0] = numbersSprite_->CreateInstance();
	timerInitData.timerSprite[1] = numbersSprite_->CreateInstance();
	timerInitData.timerSprite[2] = numbersSprite_->CreateInstance();
	timerInitData.timerSprite[3] = numbersSprite_->CreateInstance();
	timerInitData.commaSprite = commaSprite_->CreateInstance();

	std::unique_ptr<Timer> newTimer = std::make_unique<Timer>();
	newTimer->Initialize(timerInitData);
	Timer* timer = newTimer.get();

	huds_.push_back(std::move(newTimer));

	return timer;
}

/// @brief 静的イベントトリガーオブジェクトを生成する
/// @param initData 
/// @return 
StaticEventTrigger* GameScene::CreateStaticEventTrigger(const StaticEventTrigger::InitData& initData)
{
	StaticEventTrigger::InitData triggerInitData = initData;
	triggerInitData.collision = eventTriggerAABBCollision_->CreateInstance();
	triggerInitData.onTriggerCallback = [this](int eventType, const char* param, bool isStartBattleArea, bool isGameClear, int navMeshGroupId, bool isNavMeshEnabled) -> bool
		{
			return HandleTriggerEvent(eventType, param, isStartBattleArea, isGameClear, navMeshGroupId, isNavMeshEnabled);
		};

	std::unique_ptr<StaticEventTrigger> newTrigger = std::make_unique<StaticEventTrigger>();
	newTrigger->Initialize(triggerInitData);
	StaticEventTrigger* trigger = newTrigger.get();

	objects_.push_back(std::move(newTrigger));

	return trigger;
}

/// @brief カメラガードオブジェクトを生成する
/// @param initData 
/// @return 
CameraGuard* GameScene::CreateCameraGuard(const CameraGuard::InitData& initData)
{
	CameraGuard::InitData guardInitData = initData;
	guardInitData.collision = cameraGuardCollision_->CreateInstance();

	std::unique_ptr<CameraGuard> newGuard = std::make_unique<CameraGuard>();
	newGuard->Initialize(guardInitData);
	CameraGuard* guard = newGuard.get();

	objects_.push_back(std::move(newGuard));

	return guard;
}