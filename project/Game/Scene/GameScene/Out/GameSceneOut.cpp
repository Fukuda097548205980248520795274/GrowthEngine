#include "../GameScene.h"

/// @brief イントロフェーズの初期化処理
void GameScene::OutPhaseInitialize()
{
	// イントロフェーズのタイマーをリセット
	outTimer_ = kOutTime;
}

/// @brief イントロフェーズの更新処理
void GameScene::OutPhaseUpdate()
{
	// デルタタイムを取得する
	const float kDt = engine_->GetDeltaTime() * engine_->GetTimeScale();

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


	// アウトフェーズのタイマーを減算
	outTimer_ -= 1.0f / 60.0f;
	outTimer_ = std::max(outTimer_, 0.0f);

	float t = 1.0f - (outTimer_ / kOutTime);
	auto roadBgm = SoundManager::GetInstance()->GetTutorialRoadBgm();
	auto bossBgm = SoundManager::GetInstance()->GetTutorialBossBgm();
	roadBgm->param_->volume = Lerp(0.2f, 0.0f, t);
	bossBgm->param_->volume = Lerp(0.2f, 0.0f, t);

	// フェードスプライトのアルファ値を更新
	if (fadeSprite_)
	{
		fadeSprite_->param_->material.color.w = 1.0f - (outTimer_ / kOutTime);
	}

	// アウトフェーズが終了したら戦闘フェーズに遷移
	if (outTimer_ <= 0.0f)
	{
		Transition("Title");
	}
}