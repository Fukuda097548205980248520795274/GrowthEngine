#include "../GameScene.h"
#include "HUD/IntroText/IntroText.h"

/// @brief 戦闘フェーズの初期化処理
void GameScene::BattlePhaseInitialize()
{
	if (!isPause_)
	{
		// イントロテキストの生成
		IntroText::InitData introTextInitData;
		introTextInitData.buttonSprite = startTextSprite_;
		std::unique_ptr<IntroText> introText = std::make_unique<IntroText>();
		introText->Initialize(introTextInitData);
		huds_.push_back(std::move(introText));
	}

	isPause_ = false;
}

/// @brief 戦闘フェーズの更新処理
void GameScene::BattlePhaseUpdate()
{
	// デルタタイムを取得する
	const float kDt = engine_->GetDeltaTime() * engine_->GetTimeScale();

	// ゲームクリア時の処理
	if (isGameClear_)
	{
		phaseManager_->ChangePhase(PhaseType::Finish);
		soundManager_->SeWin();

		// 勝利演出
		IntroText::InitData winTextInitData;
		winTextInitData.buttonSprite = winTextSprite_;
		std::unique_ptr<IntroText> introText = std::make_unique<IntroText>();
		introText->Initialize(winTextInitData);
		huds_.push_back(std::move(introText));
	}

	// プレイヤーが死亡した場合の処理
	if (player_ && player_->IsDead())
	{
		phaseManager_->ChangePhase(PhaseType::Finish);
		soundManager_->SeLose();

		// 敗北演出
		IntroText::InitData loseTextInitData;
		loseTextInitData.buttonSprite = loseTextSprite_;
		std::unique_ptr<IntroText> introText = std::make_unique<IntroText>();
		introText->Initialize(loseTextInitData);
		huds_.push_back(std::move(introText));
	}

	// バトルディレクターの更新
	BattleDirector::GetInstance().Update(kDt);

	// プレイヤーの更新
	if (player_)
	{
		player_->Update();
		playerWeapon_->Update();

		// プレイヤーの体力バーの更新
		if (playerHP_)playerHP_->Update();

		// 太陽光をプレイヤーに追従させる
		sunLight_->param_->position = player_->GetPosition() + Vector3(-5.0f, 10.0f, -5.0f);
	}

	// ナビゲーション矢印
	navigationArrow_->SetPlayer(player_.get());
	if (battleAreas_.size() == 0)navigationArrow_->SetTargetPosition(Vector3(0.0f, 0.0f, 0.0f));
	navigationArrow_->Update();

	// オブジェクトの更新
	objects_.remove_if([](const std::unique_ptr<StageObject>& object) {object->Update();return object->IsFinished();});

	// NPCの更新
	for (auto it = npcs_.begin(); it != npcs_.end(); )
	{
		(*it)->Update();

		// 倒されて終わった場合
		if ((*it)->IsFinished())
		{
			// NPCモデルをプールに返却する
			auto model = (*it)->GetModel();
			for (auto& npcModel : npcModels_)
			{
				if (npcModel.get() == model)
				{
					// NPCモデルの更新を停止する
					npcModel->param_->isUpdate = false;

					npcModelPool_->Release(std::move(npcModel));
					npcModels_.remove(npcModel);

					break;
				}
			}

			// NPCトレイルをプールに返却する
			auto trail = (*it)->GetAttackTrail();
			for (auto& npcTrail : npcTrails_)
			{
				if (npcTrail.get() == trail)
				{
					// 更新を停止する
					trail->param_->isUpdate_ = false;

					npcTrailPool_->Release(std::move(npcTrail));
					npcTrails_.remove(npcTrail);

					break;
				}
			}

			// プールに返却する
			it->get()->PoolRelease();
			npcPool_->Release(std::move(*it));

			// アクティブリストからは除外
			it = npcs_.erase(it);
		}
		else
		{
			++it;
		}
	}

	// 武器の更新
	weapons_.remove_if([](const std::unique_ptr<Weapon>& weapon) {weapon->Update();return weapon->IsFinished();});

	// HUDの更新
	huds_.remove_if([](const std::unique_ptr<HUD>& hud)
		{
			hud->Update();
			return hud->IsFinished();
		}
	);

	// 戦闘エリアの更新
	battleAreas_.remove_if([this](const std::unique_ptr<BattleArea>& battleArea)
		{
			if (battleArea->IsCleared())
			{
				isGameClear_ = battleArea->isGameClear;
				return true;
			}
			return false;
		}
	);

	// チュートリアルの更新
	stickTutorial_->Update();
	dashTutorial_->Update();
	attackTutorial_->Update();
	comboTutorial_->Update();
	grabTutorial_->Update();
	guardTutorial_->Update();
	avoidTutorial_->Update();
	rageTutorial_->Update();

	// エフェクトの更新
	effectManager_->Update();

	// カメラ制御の更新
	UpdateCameraControl(kDt);

	// プレイヤーの動きによるシェイクの更新
	if (player_)
	{
		// 攻撃を当てた時
		if (player_->IsHitAttack())
			cameraShake_->StartShake(0.2f, 0.05f, Vector3(1.0f, 1.0f, 1.0f));

		// ダメージを受けた時
		if (player_->IsHitDamage())
			cameraShake_->StartShake(0.3f, 0.1f, Vector3(1.0f, 1.0f, 1.0f));

		// ガードブレイクしたとき
		if(player_->IsGuardBreaking())
			cameraShake_->StartShake(0.4f, 0.05f, Vector3(1.0f, 1.0f, 1.0f));

		// ガードブレイクされたとき
		if(player_->IsGuardBroke())
			cameraShake_->StartShake(0.4f, 0.1f, Vector3(1.0f, 1.0f, 1.0f));

		// 弾いたとき
		if (player_->IsHitRepel())
			cameraShake_->StartShake(0.1f, 0.025f, Vector3(1.0f, 1.0f, 1.0f));

		// レイジモード開始時
		if (player_->IsSuccessRageModeStart())
			cameraShake_->StartShake(0.1f, 0.15f, Vector3(1.0f, 1.0f, 1.0f));
	}

	// 攻撃ボタンの更新
	if (xButton_)xButton_->Update();
	if (yButton_)yButton_->Update();
	if (aButton_)aButton_->Update();
	if (bButton_)bButton_->Update();
	if (rtTriggerButton_)rtTriggerButton_->Update();

	// カメラシェイクの更新
	cameraShake_->Update(kDt);


	// ポーズ画面の切り替え
	if (engine_->GetKeyTrigger(DIK_ESCAPE) || 
		engine_->GetGamepadButtonTrigger(0, XINPUT_GAMEPAD_START))
	{
		phaseManager_->ChangePhase(PhaseType::Pause);
	}
}