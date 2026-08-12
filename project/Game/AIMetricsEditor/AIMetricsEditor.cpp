#include "AIMetricsEditor.h"
#include "Entity/Character/NPC/NPC.h"
#include "Entity/Character/Player/Player.h"
#include "BattleDirector/BattleDirector.h"

AIMetricsEditor::AIMetricsEditor()
{
	movementDotHistory_.resize(100, 1.0f);
}

AIMetricsEditor::~AIMetricsEditor()
{
}

/// @brief 初期化
/// @param mainCamera 
void AIMetricsEditor::Initialize(MainCamera3D* mainCamera)
{
	assert(mainCamera);

	// メインカメラを取得
	mainCamera_ = mainCamera;
}

void AIMetricsEditor::Update(float deltaTime)
{
#ifdef DEVELOPMENT	

	// キャラクターのリストを取得
	const auto& characters = Character::GetCharacters();

	// 壁にぶつかった回数を計測するためのタイマーを進行
	wallHitTimer_ += deltaTime;
	
	for (size_t i = 0; i < characters.size(); ++i)
	{
		// NPC同士の最小距離を計算
		for (size_t j = i + 1; j < characters.size(); ++j)
		{
			float dist = (characters[i]->GetPosition() - characters[j]->GetPosition()).Length();

			// NPC間の最小距離を計算
			if (dist < minDistanceBetweenNPCs_)
				minDistanceBetweenNPCs_ = dist;
		}

		// 壁ヒットの回数を計測
		if(!characters[i]->IsPlayer())
			UpdateWallHitMetrics(static_cast<const NPC*>(characters[i]), wallHitTimer_);
	}
	if (characters.size() <= 1) minDistanceBetweenNPCs_ = 0.0f;


	// 狙われていないキャラクターの数を計算
	untargetedCharacterCount_ = 0;
	for (const auto* target : characters) 
	{
		bool isTargeted = false;

		for (const auto* attacker : characters)
		{
			// 攻撃者がターゲットを狙っているかどうかを判定
			if (isTargeted) break;

			// 自分自身はスキップ
			if (target == attacker)continue;

			if (target->GetLockOnTarget() == attacker)
			{
				isTargeted = true;
			}
		}

		// 狙われていないキャラクターの数をカウント
		if (!isTargeted)
		{
			untargetedCharacterCount_++;
		}
	}


	targetEvaluationTimer_ += deltaTime;
	if (targetEvaluationTimer_ >= 3.0f)
	{
		targetEvaluationTimer_ = 0.0f;

		bool hasEvaluatedAnyNPC = false;

		// フィールド上の全NPCの再評価フラグをチェック
		for (auto* character : characters)
		{
			// プレイヤーは対象外
			if (character->IsPlayer()) continue;

			// NPCキャスト
			NPC* npc = static_cast<NPC*>(character);

			// 3秒間に再評価処理が実行されていたかチェック
			if (npc->IsChangedTarget())
			{
				hasEvaluatedAnyNPC = true;
			}
		}

		// 3秒経過しても一度も再評価が行われていなければ VIOLATION
		if (!hasEvaluatedAnyNPC)
		{
			targetEvalViolationCount_++;
		}
	}

#endif
}

void AIMetricsEditor::Draw()
{
#ifdef DEVELOPMENT

	// キャラクター選択UIを描画
	DrawSelectCharacterUI();

	// 計測エディタのUIを描画
	DrawMeasurementEditorUI();

#endif
}

/// @brief 移動のスムーズさを計測する
/// @param npc 
/// @param dt 
/// @param currentTime 
void AIMetricsEditor::UpdateMovementSmoothness(const NPC* npc, float currentTime)
{
	Vector3 currentVelocity = npc->GetVelocity();
	if (currentVelocity.LengthSq() < 0.01f) return; // 停止時はスキップ

	Vector3 currentDir = currentVelocity.Normalize();

	// 履歴に追加
	movementHistory_.push_back({ currentTime, currentDir });

	// 0.4秒以上古い履歴を削除
	while (!movementHistory_.empty() && (currentTime - movementHistory_.front().first > 0.4f)) {
		movementHistory_.pop_front();
	}

	// 0.4秒前（履歴の最古要素）との内積を計算
	if (!movementHistory_.empty())
	{
		Vector3 pastDir = movementHistory_.front().second;
		float dot = Dot(currentDir, pastDir);

		// 内積が0未満＝0.4秒以内に180度近く急ターンした
		if (dot < 0.0f)
		{
			abruptTurnCount_++; // 急旋回カウントなどをインクリメント
		}
	}
}

/// @brief 壁ヒットの回数を計測する
/// @param npc 
/// @param currentTime 
void AIMetricsEditor::UpdateWallHitMetrics(const NPC* npc, float currentTime)
{
	// Character側で壁衝突を検知したフレームで呼び出す
	if (npc->IsWallTouch()) {
		wallHitTimestamps_.push_back(currentTime);
	}

	// 10秒より古い衝突記録を削除
	while (!wallHitTimestamps_.empty() && (currentTime - wallHitTimestamps_.front() > 10.0f)) {
		wallHitTimestamps_.pop_front();
	}

	// 直近10秒の合計衝突回数を更新
	wallHitCountIn10s_ = static_cast<int>(wallHitTimestamps_.size());
}

/// @brief キャラクター選択UIを描画する
void AIMetricsEditor::DrawSelectCharacterUI()
{
#ifdef DEVELOPMENT

	// デルタタイムの取得
	const float kDt = GrowthEngine::GetInstance()->GetDeltaTime();

	// エディタ起動からの累計時間を更新
	editorTotalTime_ += kDt;

	// メトリクスの初期化
	currentAttackingNPCs_ = 0;
	minNextAttackCooldown_ = 999.0f;
	isPlayerInCombo_ = false;
	isBlindSpotAttackAttempted_ = false;
	minDistanceBetweenNPCs_ = 999.0f;
	surroundingCount_ = 0;

	ImGui::Begin("キャラクター選択UI");

	// 全キャラクターのリストを取得
	const auto& characters = Character::GetCharacters();

	// キャラクターが存在しない場合はUIを描画せずに終了
	if (characters.empty())
	{
		ImGui::Text("フィールド上にキャラクターが存在しません。");
		ImGui::End();
		return;
	}

	// 選択中のキャラクターを保持する静的変数
	static int selectedIndex = 0;

	// インデックスの範囲外参照を防止
	if (selectedIndex >= static_cast<int>(characters.size()))
		selectedIndex = 0;



	ImGui::Text("【キャラクター選択】");

	// 選択肢用ラベルの作成
	std::vector<std::string> characterNames;
	for (size_t i = 0; i < characters.size(); ++i)
	{
		const Character* c = characters[i];
		std::string tagStr = "";
		switch (c->GetCharacterTag())
		{
		case CharacterTag::Player:     tagStr = "Player"; break;
		case CharacterTag::EnemyNormal: tagStr = "Enemy"; break;
		case CharacterTag::EnemyBoss:   tagStr = "Boss"; break;
		case CharacterTag::Ally:        tagStr = "Ally"; break;
		default:                        tagStr = "Unknown"; break;
		}

		// キャラクターのインデックス、タグ、ポインタアドレスを含むラベルを作成
		std::string name = "[" + std::to_string(i) + "] " + tagStr + " (" + std::to_string(reinterpret_cast<uintptr_t>(c)) + ")";
		characterNames.push_back(name);
	}

	// コンボボックスで対象キャラクターを選択
	if (ImGui::BeginCombo("キャラクターを選択", characterNames[selectedIndex].c_str()))
	{
		for (int i = 0; i < static_cast<int>(characterNames.size()); ++i)
		{
			const bool isSelected = (selectedIndex == i);
			if (ImGui::Selectable(characterNames[i].c_str(), isSelected))
			{
				selectedIndex = i;

				// キャラクターが切り替わった際に、計測用の変数をリセット
				targetEvalViolationCount_ = 0;
				frequentDetourCount_ = 0;
				lastDetourTime_ = -999.0f;
				isPrevDetouring_ = false;
			}

			// 選択されたアイテムにフォーカスを設定
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	// 選択されたキャラクターを取得
	Character* selectedChar = characters[selectedIndex];

	// スムーズな移動の計測用タイマーを進行
	movementTimer_ += kDt;
	if (movementTimer_ >= 0.4f)
	{
		movementTimer_ = 0.0f;
		
		abruptTurnCount_ = 0; // 急旋回カウントをリセット
	}

	if (selectedChar)
	{
		// プレイヤーの場合、コンボ状態をチェック
		if (selectedChar->IsPlayer())
		{
			Player* player = static_cast<Player*>(selectedChar);

			// プレイヤーのコンボ状態をチェック
			if (player->IsCombo())
				isPlayerInCombo_ = true;


			// カメラの位置と前方ベクトルを取得
			Vector3 cameraPos = mainCamera_->param_->transform.translate;
			Vector3 cameraForward = mainCamera_->GetDirection();

			// カメラの視野角の半分をラジアンに変換
			float halfFovRad = mainCamera_->param_->setting.fov * 0.5f;
			float cosHalfFov = std::cos(halfFovRad);

			// 選択されたキャラクターをターゲットにしているNPCの攻撃状態をチェック
			for (const auto& character : characters)
			{
				if (selectedChar == character) continue; // 自分自身はスキップ

				if (selectedChar != character->GetLockOnTarget()) continue; // 選択されたキャラクターをターゲットにしていないNPCはスキップ

				if (!character->IsInAttackSequence()) continue; // 攻撃中のNPCのみを対象

				Vector3 toNPC = (character->GetPosition() - cameraPos).Normalize();
				float dot = Dot(cameraForward, toNPC);

				// カメラの視野角外から攻撃が試みられた場合のフラグを立てる
				if (dot < cosHalfFov)
				{
					isBlindSpotAttackAttempted_ = true;
					break;
				}
			}
		}
		else
		{
			// NPCの場合、攻撃状態とクールダウンをチェック
			NPC* npc = static_cast<NPC*>(selectedChar);

			// クールダウンの最小値取得
			float cd = npc->GetAttackCooltime();
			if (cd < minNextAttackCooldown_)
				minNextAttackCooldown_ = cd;

			// 移動のスムーズさを計測
			UpdateMovementSmoothness(npc, movementTimer_);


			// NPCの迂回状態をチェック
			bool currentDetouring = npc->IsDetouring();

			// 迂回状態に入った瞬間 (false -> true) を検知
			if (currentDetouring && !isPrevDetouring_)
			{
				// 1秒以内に頻繁に迂回している場合のカウント
				if (editorTotalTime_ - lastDetourTime_ < 1.0f)
				{
					frequentDetourCount_++;
				}

				// 最新の迂回開始時間を記録
				lastDetourTime_ = editorTotalTime_;
			}
			isPrevDetouring_ = currentDetouring;
		}

		// 選択されたキャラクターをターゲットにしているNPCの数をカウント
		for (const auto& character : characters)
		{
			// 自分自身はカウントしない
			if (selectedChar == character) continue;

			// NPCのみを対象
			if (selectedChar != character->GetLockOnTarget())continue;

			// 攻撃中のNPCのみをカウント
			if (!character->IsInAttackSequence()) continue;

			currentAttackingNPCs_++;
		}
	}

	// 選択されたキャラクターの周囲にいるNPCの数を取得
	surroundingCount_ = BattleDirector::GetInstance().GetUsedSlotCount(selectedChar);

	
	// ターゲットへの平均距離 & 3m以内包囲人数の計算
	float totalDist = 0.0f;
	int targetCount = 0;

	if (selectedChar)
	{
		// 包囲状態で3m以内にいるNPCの数
		for (int i = 0; i < surroundingCount_; i++)
		{
			Character* occupant = BattleDirector::GetInstance().GetSlotOccupant(selectedChar, i);
			if (occupant)
			{
				float dist = (occupant->GetWorldPosition() - selectedChar->GetWorldPosition()).Length();

				// 平均距離計算用に距離を加算
				totalDist += dist;
				targetCount++;

				// 3m以内のNPCをカウント
				if (dist <= 3.0f)
				{
					surroundingNPCCount_++;
				}
			}
		}


		maxStuckTime_ = 0.0f;
		maxStaticPositionTime_ = 0.0f;

		if (!selectedChar->IsStance() && !selectedChar->IsInAttackSequence())
		{
			Vector3 currentPos = selectedChar->GetWorldPosition();
			float moveDist = (currentPos - lastPosition_).Length();

			// 静止時間の計測 (自身の移動速度が一定以下の場合)
			if (moveDist < 0.05f * kDt)
			{
				staticTime_ += kDt;
			}
			else
			{
				staticTime_ = 0.0f;
			}

			// スタック時間の計測 (前方0.5m以内に他のキャラクターがいるか)
			bool isStuckByNPC = false;
			Vector3 myForward = selectedChar->GetDirection(); // 自身の向いている方向

			for (const auto* other : characters)
			{
				// 自分自身と死んでいるキャラクターは除外
				if (other == selectedChar || other->IsDead()) continue;

				Vector3 toOther = other->GetPosition() - currentPos;
				toOther.y = 0.0f; // 水平方向のみで判定
				float dist = toOther.Length();

				// 半径0.5m未満にいるか
				if (dist > 0.0f && dist < 0.5f)
				{
					Vector3 toOtherDir = toOther.Normalize();
					float dot = Dot(myForward, toOtherDir);

					// 前方にいるかどうかを判定 (dot > 0.0f なら前方)
					if (dot > 0.0f)
					{
						isStuckByNPC = true;
						break;
					}
				}
			}

			// スタック状態ならタイマーを加算
			if (isStuckByNPC)
			{
				stuckTime_ += kDt;
			}
			else
			{
				// スタック要因が外れた、または自身が0.5m以上移動できたらリセット
				if (moveDist > 0.5f || !isStuckByNPC)
				{
					stuckTime_ = 0.0f;
				}
			}

			lastPosition_ = currentPos;

			// NPCのみのスタック時間と静止時間の最大値を計測
			if (!selectedChar->IsPlayer())
			{
				maxStuckTime_ = std::max(maxStuckTime_, stuckTime_);
				maxStaticPositionTime_ = std::max(maxStaticPositionTime_, staticTime_);
			}
		}
		else if(!selectedChar->IsInAttackSequence())
		{
			Vector3 currentPos = selectedChar->GetWorldPosition();
			float moveDist = (currentPos - lastPosition_).Length();

			// 移動速度が一定以下なら静止時間をカウント
			if (moveDist < 0.05f * kDt)
			{
				staticTime_ += kDt;
			}
			else
			{
				staticTime_ = 0.0f;
			}

			lastPosition_ = currentPos;

			// 1v1交戦時の静止時間の最大値を計測
			staticTime1v1_ = staticTime_;
		}
	}

	// ターゲットへの平均距離の計算
	avgDistanceToTarget_ = (targetCount > 0) ? (totalDist / targetCount) : 0.0f;


	ImGui::End();

#endif
}

/// @brief 計測エディタのUIを描画する
void AIMetricsEditor::DrawMeasurementEditorUI()
{
#ifdef DEVELOPMENT

	ImGui::Begin("AI Evaluation Metrics (Live Auto-Monitor)");

	if (ImGui::BeginTabBar("MetricsTabs"))
	{
		// 戦闘・攻撃
		if (ImGui::BeginTabItem("Combat & Attack"))
		{
			ImGui::Text("=== 同時攻撃NPC数 (目標: 1体のみ) ===");
			if (currentAttackingNPCs_ > 1)
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "今の攻撃人数 : %d (VIOLATION!)", currentAttackingNPCs_);
			else
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "今の攻撃人数 : %d (SAFE)", currentAttackingNPCs_);

			ImGui::Separator();

			ImGui::Text("=== 最短攻撃クールダウン (目標: 1秒以上) ===");
			ImGui::Text("Min Cooldown Remaining: %.2f sec", minNextAttackCooldown_);

			ImGui::Separator();

			ImGui::Text("=== 壁ヒット数 ===");
			ImGui::Text("Wall Hits in Last 10s: %d", wallHitCountIn10s_);

			ImGui::Text("=== コンボ保護 ===");
			ImGui::Text("Player In Combo: %s", isPlayerInCombo_ ? "YES (Protecting)" : "No");

			ImGui::Separator();

			ImGui::Text("=== 画面外(死角)からの不意打ち判定 ===");
			if (isBlindSpotAttackAttempted_)
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "WARNING: Blind Spot Attack Detected!");
			else
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "No Blind Spot Attacks");


			ImGui::Text("=== ターゲット再評価頻度 (3秒毎) ===");
			ImGui::Text("Evaluation Timer: %.1f / 3.0 sec", targetEvaluationTimer_);

			if (targetEvalViolationCount_ > 0)
			{
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
					"Target Eval Violations: %d (VIOLATION!)", targetEvalViolationCount_);
			}
			else
			{
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
					"Target Eval Status: SAFE (0 Violations)");
			}

			ImGui::EndTabItem();
		}

		// 位置・陣形
		if (ImGui::BeginTabItem("Positioning & Formations"))
		{
			ImGui::Text("=== NPC間の最小距離 (目標: 1m以上) ===");
			if (minDistanceBetweenNPCs_ < 1.0f && minDistanceBetweenNPCs_ > 0.0f)
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Min Distance: %.2f m (Too Close!)", minDistanceBetweenNPCs_);
			else
				ImGui::Text("Min Distance: %.2f m", minDistanceBetweenNPCs_);

			ImGui::Separator();

			ImGui::Text("=== 移動軌跡の滑らかさ ===");
			ImGui::Text("abruptTurnCount : %d", abruptTurnCount_);

			ImGui::Separator();

			ImGui::Text("=== 陣形・距離 ===");
			ImGui::Text("Avg Distance to Target: %.2f m", avgDistanceToTarget_);
			ImGui::Text("Surrounding NPCs (<=3m): %d / 8", surroundingNPCCount_);

			ImGui::Separator();

			ImGui::Text("=== ターゲット分散 ===");
			if (untargetedCharacterCount_ > 0)
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Untargeted Free Players: %d", untargetedCharacterCount_);
			else
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Untargeted Players: 0 (Good)");

			ImGui::Separator();

			ImGui::Text("=== スタック & 静止チェック ===");
			if (maxStuckTime_ > 1.0f)
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Max Stuck Time: %.2f sec (STUCK!)", maxStuckTime_);
			else
				ImGui::Text("Max Stuck Time: %.2f sec", maxStuckTime_);

			if (maxStaticPositionTime_ > 5.0f)
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Max Static Time: %.2f sec (Too Long!)", maxStaticPositionTime_);
			else
				ImGui::Text("Max Static Time: %.2f sec", maxStaticPositionTime_);

			ImGui::EndTabItem();
		}

		// AIタスク・1v1状況
		if (ImGui::BeginTabItem("AI Task Management"))
		{
			ImGui::Text("=== 1v1交戦時の静止監視 (目標: 2秒以上静止しない) ===");
			if (staticTime1v1_ > 2.0f)
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "1v1 Static Time: %.2f sec (VIOLATION!)", staticTime1v1_);
			else
				ImGui::Text("1v1 Static Time: %.2f sec", staticTime1v1_);

			ImGui::Text("=== 迂回行動の連続発生 (目標: 1秒以内に連発しない) ===");
			if (frequentDetourCount_ > 0)
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "頻回な迂回 (1秒以内): %d 回 (VIOLATION!)", frequentDetourCount_);
			else
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "頻回な迂回: 0 回 (SAFE)");

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();

#endif
}