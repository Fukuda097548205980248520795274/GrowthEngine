#include "AIMetricsEditor.h"
#include "Entity/Character/NPC/NPC.h"
#include "Entity/Character/Player/Player.h"
#include "BattleDirector/BattleDirector.h"
#include <numbers>

AIMetricsEditor::AIMetricsEditor()
{
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

	// NPCが存在するかどうかを判定するフラグ
	bool hasNPC = false;

	// 攻撃中のNPCが存在するかどうかを判定するフラグ
	bool isAttackingNPCExists = false;

	// 壁にぶつかった回数を計測するためのタイマーを進行
	wallHitTimer_ += deltaTime;

	// NPC間の最小距離を計算
	minDistanceBetweenNPCs_ = 999.0f;
	
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
		if (!characters[i]->IsPlayer())
		{
			UpdateWallHitMetrics(static_cast<const NPC*>(characters[i]), wallHitTimer_);

			// NPCが存在することを示すフラグを立てる
			hasNPC = true;

			// 攻撃中のNPCが存在するかどうかを判定
			if (!isAttackingNPCExists)
				isAttackingNPCExists = characters[i]->IsInAttackSequence();
		}
	}
	if (characters.size() <= 1) minDistanceBetweenNPCs_ = 0.0f;

	// 攻撃中のNPCが存在する場合、次の攻撃までの最短クールダウン時間を計測
	if (hasNPC)
	{
		// 誰も攻撃していなかった状態から、誰かが攻撃を始めた瞬間
		if (isAttackingNPCExists && !wasAttackingNPCExists_)
		{
			// クールタイムが1秒未満だった場合は違反としてカウント
			if (minNextAttackCooldown_ < 1.0f)
			{
				attackCooldownViolationCount_++;
			}
			// 攻撃が始まったのでタイマーをリセット
			minNextAttackCooldown_ = 0.0f;
		}
		else if (!isAttackingNPCExists)
		{
			// 誰も攻撃していない場合はタイマーを進行
			minNextAttackCooldown_ += deltaTime;
		}
	}
	else
	{
		minNextAttackCooldown_ = 0.0f;
	}

	// 現在の攻撃状態を次フレームの比較用に保存
	wasAttackingNPCExists_ = isAttackingNPCExists;


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
	if (!hasNPC)
	{
		targetEvaluationTimer_ = 0.0f;
		targetEvalViolationCount_ = 0;
	}

	if (targetEvaluationTimer_ > 3.0f)
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

			hasNPC = true;

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
	isPlayerInCombo_ = false;
	isBlindSpotAttackAttempted_ = false;
	surroundingCount_ = 0;
	surroundingNPCCount_ = 0;

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

				attackCooldownViolationCount_ = 0;

				prevAttackStates_.clear();
				consecutiveSameDirectionAttacks_ = 0;
				attackDirectionBiasViolationCount_ = 0;
				lastAttackSector_ = -1;
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

			bool isCurrentlyAttacking = character->IsInAttackSequence();
			bool wasAttacking = prevAttackStates_[character];

			// 攻撃開始の瞬間を検知
			if (isCurrentlyAttacking && !wasAttacking)
			{
				// ターゲットから見たNPCへの方向ベクトル
				Vector3 toNPC = (character->GetWorldPosition() - selectedChar->GetWorldPosition());
				toNPC.y = 0.0f; // 水平方向のみで計算
				toNPC = toNPC.Normalize();

				// ターゲットの正面方向と右方向
				Vector3 worldForward = Vector3(0.0f, 0.0f, 1.0f); // ワールドのZ軸
				Vector3 worldRight = Vector3(1.0f, 0.0f, 0.0f); // ワールドのX軸

				// ターゲットを基準としたNPCの角度を計算 (-π 〜 π)
				float angle = std::atan2(Dot(worldRight, toNPC), Dot(worldForward, toNPC));

				// 8方向（45度ごと）のセクターに分割 (0〜7)
				int sector = static_cast<int>(std::round((angle + std::numbers::pi) / (std::numbers::pi / 4.0f))) % 8;

				// 前回と同じセクターからの攻撃かチェック
				if (sector == lastAttackSector_)
				{
					consecutiveSameDirectionAttacks_++;

					// 3回以上連続したら違反としてカウント
					if (consecutiveSameDirectionAttacks_ >= 3)
					{
						attackDirectionBiasViolationCount_++;
					}
				}
				else
				{
					// 違う方向からの攻撃ならリセット
					consecutiveSameDirectionAttacks_ = 1;
					lastAttackSector_ = sector;
				}
			}

			// 状態を更新
			prevAttackStates_[character] = isCurrentlyAttacking;


			// 攻撃中のNPCのみをカウント
			if (!isCurrentlyAttacking) continue;

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
		// 包囲状態で3m以上にいるNPCの数
		for (int i = 0; i < surroundingCount_; i++)
		{
			Character* occupant = BattleDirector::GetInstance().GetSlotOccupant(selectedChar, i);
			if (occupant)
			{
				float dist = (occupant->GetWorldPosition() - selectedChar->GetWorldPosition()).Length();

				// 平均距離計算用に距離を加算
				totalDist += dist;
				targetCount++;

				// 3m以上のNPCをカウント
				if (dist >= 3.0f)
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

	ImGui::Begin("AI評価メトリクス (自動監視)");

	// 表示フィルターのON/OFF設定
	if (ImGui::CollapsingHeader("表示フィルター (ON/OFF)"))
	{
		ImGui::Text("【戦闘・攻撃】");
		ImGui::Checkbox("同時攻撃NPC数", &displaySettings_.showAttackingNPCs); ImGui::SameLine();
		ImGui::Checkbox("最短攻撃クールダウン", &displaySettings_.showAttackCooldown); ImGui::SameLine();
		ImGui::Checkbox("壁衝突回数", &displaySettings_.showWallHits);
		ImGui::Checkbox("コンボ保護", &displaySettings_.showComboProtection); ImGui::SameLine();
		ImGui::Checkbox("死角攻撃", &displaySettings_.showBlindSpotAttack); ImGui::SameLine();
		ImGui::Checkbox("ターゲット評価", &displaySettings_.showTargetEvaluation);

		ImGui::Text("【位置・陣形】");
		ImGui::Checkbox("NPC間の最小距離", &displaySettings_.showMinDistance); ImGui::SameLine();
		ImGui::Checkbox("陣形・距離", &displaySettings_.showFormation); ImGui::SameLine();
		ImGui::Checkbox("スタック & 静止", &displaySettings_.showStuckStatic);
		ImGui::Checkbox("移動のスムーズさ", &displaySettings_.showSmoothness); ImGui::SameLine();
		ImGui::Checkbox("ターゲット分散", &displaySettings_.showTargetDispersion);
		ImGui::Checkbox("攻撃方向の偏り (スロット回転)", &displaySettings_.showAttackDirectionBias);

		ImGui::Text("【AIタスク管理】");
		ImGui::Checkbox("1v1交戦時の静止時間", &displaySettings_.show1v1Static); ImGui::SameLine();
		ImGui::Checkbox("迂回行動の頻度", &displaySettings_.showDetour);

		ImGui::Separator();
	}

	// タブバーの開始
	if (ImGui::BeginTabBar("MetricsTabs"))
	{
		// 戦闘・攻撃
		if (ImGui::BeginTabItem("戦闘・攻撃"))
		{
			if (displaySettings_.showAttackingNPCs)
			{
				ImGui::Text("・ 同時攻撃NPC数 (目標: 1体のみ)");
				if (currentAttackingNPCs_ > 1)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "現在の攻撃人数 : %d人 (違反!)", currentAttackingNPCs_);
				else
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "現在の攻撃人数 : %d人 (正常)", currentAttackingNPCs_);
				ImGui::Separator();
			}

			if (displaySettings_.showAttackCooldown)
			{
				ImGui::Text("・ 攻撃クールダウン (目標: 1秒以上の間隔を空ける)");
				ImGui::Text("現在の非攻撃時間: %.2f秒", minNextAttackCooldown_);

				// 未達成と正常が行き来しないよう、違反回数で評価
				if (attackCooldownViolationCount_ > 0)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "違反回数: %d回 (未達成!)", attackCooldownViolationCount_);
				else
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "違反なし (正常)");
				ImGui::Separator();
			}

			if (displaySettings_.showWallHits)
			{
				ImGui::Text("・ 直近10秒間の壁衝突回数 (目標: 0回)");
				// 色付きの成否表示を追加
				if (wallHitCountIn10s_ > 0)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "衝突回数: %d回 (未達成!)", wallHitCountIn10s_);
				else
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "衝突回数: 0回 (正常)");
				ImGui::Separator();
			}

			if (displaySettings_.showComboProtection)
			{
				ImGui::Text("・ コンボ保護状態");
				ImGui::Text("保護中: %s", isPlayerInCombo_ ? "はい" : "いいえ");
				ImGui::Separator();
			}

			if (displaySettings_.showBlindSpotAttack)
			{
				ImGui::Text("・ 死角からの攻撃判定");
				if (isBlindSpotAttackAttempted_)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "警告: 死角からの攻撃を検知!");
				else
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "死角からの攻撃なし");
				ImGui::Separator();
			}

			if (displaySettings_.showTargetEvaluation)
			{
				ImGui::Text("・ ターゲット再評価頻度 (3秒毎)");
				ImGui::Text("評価タイマー: %.1f / 3.0秒", targetEvaluationTimer_);
				if (targetEvalViolationCount_ > 0)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "違反回数: %d回 (違反!)", targetEvalViolationCount_);
				else
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "違反なし");
				ImGui::Separator();
			}

			ImGui::EndTabItem();
		}

		// 位置・陣形
		if (ImGui::BeginTabItem("位置・陣形"))
		{
			if (displaySettings_.showMinDistance)
			{
				ImGui::Text("・ NPC間の分離行動 (目標: 最小距離1m以上)");

				if (minDistanceBetweenNPCs_ == 0.0f)
				{
					// キャラクターが2体以上いない場合
					ImGui::Text("最小距離: -- m (比較対象なし)");
				}
				else if (minDistanceBetweenNPCs_ < 1.0f)
				{
					// 1m未満の場合は未達成（赤）
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "最小距離: %.2f m (未達成: 密集しすぎています!)", minDistanceBetweenNPCs_);
				}
				else
				{
					// 1m以上の場合は正常（緑）
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "最小距離: %.2f m (正常: 分離できています)", minDistanceBetweenNPCs_);
				}
				ImGui::Separator();
			}

			if (displaySettings_.showFormation)
			{
				ImGui::Text("・ 陣形維持 (目標: 平均距離3m以上 / 適切な待機距離)");

				if (surroundingCount_ > 0)
				{
					// 平均距離の評価
					if (avgDistanceToTarget_ >= 3.0f)
					{
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "平均ターゲット距離: %.2f m (正常)", avgDistanceToTarget_);
					}
					else
					{
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "平均ターゲット距離: %.2f m (未達成: 全体的に近すぎます!)", avgDistanceToTarget_);
					}

					// 包囲人数の評価（3m以上の距離を保っているか）
					if (surroundingNPCCount_ == surroundingCount_)
					{
						// 全員が3m以上離れて陣形を組んでいる（緑）
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "適切な距離(3m以上)の包囲人数: %d / %d人 (正常)", surroundingNPCCount_, surroundingCount_);
					}
					else if (surroundingNPCCount_ > 0)
					{
						// 一部が攻撃等で接近している（黄色で警告寄りの表示）
						ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "適切な距離(3m以上)の包囲人数: %d / %d人 (一部接近中)", surroundingNPCCount_, surroundingCount_);
					}
					else
					{
						// 全員が3m未満にいる（赤）
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "適切な距離(3m以上)の包囲人数: %d / %d人 (未達成: 密集陣形!)", surroundingNPCCount_, surroundingCount_);
					}
				}
				else
				{
					ImGui::Text("現在ターゲットを包囲しているNPCはいません");
				}

				ImGui::Separator();
			}

			if (displaySettings_.showStuckStatic)
			{
				ImGui::Text("・ スタック & 静止チェック");
				if (maxStuckTime_ > 1.0f)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "最大スタック時間: %.2f秒 (スタック中!)", maxStuckTime_);
				else
					ImGui::Text("最大スタック時間: %.2f秒", maxStuckTime_);

				if (maxStaticPositionTime_ > 5.0f)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "最大静止時間: %.2f秒 (静止しすぎ!)", maxStaticPositionTime_);
				else
					ImGui::Text("最大静止時間: %.2f秒", maxStaticPositionTime_);
				ImGui::Separator();
			}

			if (displaySettings_.showSmoothness)
			{
				ImGui::Text("・ 移動のスムーズさ (目標: 急旋回0回)");
				// 色付きの成否表示を追加
				if (abruptTurnCount_ > 0)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "急旋回回数 (0.4秒以内): %d回 (未達成!)", abruptTurnCount_);
				else
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "急旋回なし (正常)");
				ImGui::Separator();
			}

			if (displaySettings_.showTargetDispersion)
			{
				ImGui::Text("・ ターゲット分散");
				if (untargetedCharacterCount_ > 0)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "非ターゲット状態のプレイヤー数: %d人", untargetedCharacterCount_);
				else
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "正常: ターゲット漏れなし");
				ImGui::Separator();
			}

			if (displaySettings_.showAttackDirectionBias)
			{
				ImGui::Text("・ 攻撃方向の偏り (目標: 同一方向から3回以上連続しない)");
				ImGui::Text("現在の連続回数: %d回", consecutiveSameDirectionAttacks_);

				if (attackDirectionBiasViolationCount_ > 0)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "違反回数: %d回 (偏り発生!)", attackDirectionBiasViolationCount_);
				else
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "違反なし (適切に分散)");

				ImGui::Separator();
			}

			ImGui::EndTabItem();
		}

		// AIタスク管理
		if (ImGui::BeginTabItem("AIタスク管理"))
		{
			if (displaySettings_.show1v1Static)
			{
				ImGui::Text("■ 1v1交戦時の静止監視 (目標: 2秒以上静止しない)");
				if (staticTime1v1_ > 2.0f)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "1v1静止時間: %.2f秒 (違反!)", staticTime1v1_);
				else
					ImGui::Text("1v1静止時間: %.2f秒", staticTime1v1_);
				ImGui::Separator();
			}

			if (displaySettings_.showDetour)
			{
				ImGui::Text("■ 迂回行動の連続発生 (目標: 1秒以内に連発しない)");
				if (frequentDetourCount_ > 0)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "違反回数: %d回 (違反!)", frequentDetourCount_);
				else
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "違反なし");
				ImGui::Separator();
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();

#endif
}