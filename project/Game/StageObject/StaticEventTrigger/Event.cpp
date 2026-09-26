#include "Scene/GameScene/GameScene.h"
#include "HUD/IntroText/IntroText.h"

/// @brief イベントトリガーに触れたときの処理
/// @param eventType 
/// @param param 
/// @param isStartBattleArea 
/// @param isGameClear 
/// @param navMeshGroupId 
/// @param isNavMeshEnabled 
bool GameScene::HandleTriggerEvent(int eventType, const char* param, bool isStartBattleArea, bool isGameClear, int navMeshGroupId, bool isNavMeshEnabled)
{
	// イベントタイプを列挙型に変換する
	StaticEventTrigger::EventType type = static_cast<StaticEventTrigger::EventType>(eventType);

	if (type == StaticEventTrigger::EventType::None)
	{
		return true;
	}
	else if (type == StaticEventTrigger::EventType::ObjectSpawn)
	{
		try
		{
			// param が空文字列の場合は何もしない
			std::string fileName = param;
			if (fileName.empty()) return true;

			// JSONファイルのパスを作成する
			std::string filePath = "./Assets/Parameter/StageData/" + fileName + ".json";

			// ファイルストリームを開く
			std::ifstream ifs(filePath);
			if (!ifs.is_open())return false;

			// ファイルからJSONを読み込んで解析
			json j;
			ifs >> j;
			ifs.close();

			// JSON配列をループして、記述された各種オブジェクトを生成する
			if (j.contains("objects") && j["objects"].is_array())
			{
				if (isStartBattleArea)
				{
					std::unique_ptr<BattleArea> battleArea = std::make_unique<BattleArea>();
					battleArea->isGameClear = isGameClear;

					// 最後のバトルエリアであれば、BGMを切り替える
					if (isGameClear)
					{
						soundManager_->BgmTutorialRoadPlay(false);
						soundManager_->BgmTutorialBossPlay(true);

						// イントロテキストの生成
						IntroText::InitData introTextInitData;
						introTextInitData.buttonSprite = bossTextSprite_;
						std::unique_ptr<IntroText> introText = std::make_unique<IntroText>();
						introText->Initialize(introTextInitData);

						huds_.push_back(std::move(introText));
					}

					for (const auto& objectDataJson : j["objects"])
					{
						PlacementData initData;
						fromJson(objectDataJson, initData);

						// 解析したデータをもとにオブジェクトを生成する
						if (stageEditor_->SpawnObject(initData, battleArea.get()))
							stageEditor_->SetPlacementList(initData);
					}

					battleAreas_.push_back(std::move(battleArea));
				}
				else
				{
					for (const auto& objectDataJson : j["objects"])
					{
						PlacementData initData;
						fromJson(objectDataJson, initData);

						// 解析したデータをもとにオブジェクトを生成する
						if (stageEditor_->SpawnObject(initData))
							stageEditor_->SetPlacementList(initData);
					}
				}

				j.erase("objects");
			}

			// 探索中にバトルエリアが生成された場合は、フェーズを探索からバトルに切り替える
			if (phaseManager_->GetCurrentPhase() == PhaseType::Exploration)
			{
				phaseManager_->ChangePhase(PhaseType::Battle);
			}

			// 生成が終わったイベントトリガーを削除する場合は true を返す
			return true;
		}
		catch (const std::exception& e)
		{
			// JSONの解析に失敗した場合はエラーを出力してイベントトリガーを削除しない
			(void)e;
			return false;
		}
	}
	else if (type == StaticEventTrigger::EventType::PlayCutscene)
	{
		// すでにカットシーンが再生中の場合は何もしない
		if (cutsceneManager_->IsPlaying())
			return false;

		// キャラクターの操作を無効化する
		Character::SetIsCutsceneActive(true);

		// カットシーン用のカメラに切り替える
		cutsceneCamera_->Switch();

		// カットシーン名を取得する
		std::string cutsceneName = param;

		// カットシーン再生が終了したら、コールバックで元のカメラに戻し、プレイヤー操作を解放する
		cutsceneManager_->Play(cutsceneName, [this]()
			{
				// メインカメラに戻す
				mainCamera_->Switch();

				// キャラクターの操作を有効化する
				Character::SetIsCutsceneActive(false);
			}
		);

		return true; // トリガーを削除
	}
	else if (type == StaticEventTrigger::EventType::NavMeshStateChange)
	{
		if (navMesh_ != nullptr)
		{
			navMesh_->SetGroupActive(navMeshGroupId, isNavMeshEnabled);
		}

		return true; // トリガーを削除
	}
	else if (type == StaticEventTrigger::EventType::StickTutorial)
	{
		stickTutorial_->SetEnable(true);
	}
	else if (type == StaticEventTrigger::EventType::DashTutorial)
	{
		dashTutorial_->SetEnable(true);
	}
	else if (type == StaticEventTrigger::EventType::AttackTutorial)
	{
		attackTutorial_->SetEnable(true);
	}
	else if (type == StaticEventTrigger::EventType::ComboTutorial)
	{
		comboTutorial_->SetEnable(true);
	}
	else if (type == StaticEventTrigger::EventType::GrabTutorial)
	{
		if (player_->IsGrabbing())
		{
			attackTutorial_->SetEnable(true);
		}
		else
		{
			grabTutorial_->SetEnable(true);
		}
	}
	else if (type == StaticEventTrigger::EventType::GuardTutorial)
	{
		guardTutorial_->SetEnable(true);
	}
	else if (type == StaticEventTrigger::EventType::AvoidTutorial)
	{
		avoidTutorial_->SetEnable(true);
	}
	else if (type == StaticEventTrigger::EventType::RageModeTutorial)
	{
		rageTutorial_->SetEnable(true);
	}

	// イベントを処理した場合はtrueを返し、処理しなかった場合はfalseを返す
	return false;
}