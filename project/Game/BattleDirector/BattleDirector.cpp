#include "BattleDirector.h"
#include "Entity/Character/Character.h"
#include "Entity/Character/Player/Player.h"
#include "Entity/Character/NPC/NPC.h"
#include "NavMesh/NavMesh.h"
#include <numbers>

/// @brief インスタンスを取得する
/// @return 
BattleDirector& BattleDirector::GetInstance()
{
	static BattleDirector instance;
	return instance;
}

/// @brief 攻撃トークンを要求する
/// @param npc 
/// @param type 
/// @return 
bool BattleDirector::RequestAttackToken(Character* npc, ActionTokenType type)
{
	// NPCが現在狙っているターゲットを取得
	Character* target = npc->GetLockOnTarget();

	// ターゲットがいない場合は、攻撃トークンの要求を許可する
	if (!target)return true;

	// NPCが最も攻撃に適していない場合は、攻撃トークンの要求を拒否する
	if(!IsBestAttacker(npc))return false;

	// ターゲットのクールタイムが残っている場合は、攻撃トークンの要求を拒否する
	if (type == ActionTokenType::Attack && targetTokenCooldowns_[target] > 0.0f)
		return false;

	// 現在の攻撃トークン保持者を取得
	auto it = npcToTargetMap_.find(npc);
	if (it != npcToTargetMap_.end())
	{
		// すでに攻撃トークンを保持している場合は、ターゲットとタイプが一致するかを確認する
		if (it->second.target == target && it->second.type == type)return true;

		// 別のターゲットに対する要求の場合は、現在の攻撃トークンを返却する
		ReleaseAttackToken(npc);
	}

	// 現在の攻撃トークン保持者を取得
	auto& holders = targetTokenHolders_[target][type];

	// 攻撃トークンの最大数に達していない場合は、攻撃トークンの要求を許可する
	if (holders.size() < maxTokens_[type])
	{
		// 攻撃トークンを保持しているキャラクターのセットにNPCを追加する
		holders.insert(npc);

		// NPCとそのターゲットのマッピングを更新する
		npcToTargetMap_[npc] = { target, type };
		return true;
	}

	// 攻撃トークンの最大数に達している場合は、攻撃トークンの要求を拒否する
	return false;
}

/// @brief 攻撃トークンを返却する
/// @param npc 
void BattleDirector::ReleaseAttackToken(Character* npc)
{
	// NPCが現在狙っているターゲットを取得
	auto it = npcToTargetMap_.find(npc);
	if (it == npcToTargetMap_.end())return;

	// 現在の攻撃トークン保持者を取得
	Character* target = it->second.target;
	ActionTokenType type = it->second.type;

	float aggressiveness = std::max(0.1f, npc->GetAggressiveness());

	// 攻撃トークンのクールタイムを計算する
	float bestInterval = 1.5f;
	float finalInterval = bestInterval / (std::max(0.01f, globalTension_) * aggressiveness);

	// ターゲットのクールタイムを設定する
	targetTokenCooldowns_[target] = finalInterval;


	// 攻撃クールタイムを計算する
	float bestCooltime = 5.0f;
	float finalCooltime = bestCooltime /aggressiveness;

	// NPCの攻撃クールタイムを設定する
	npc->SetAttackCooltime(finalCooltime);


	// NPCとそのターゲットのマッピングを削除する
	auto targetIt = targetTokenHolders_.find(target);
	if (targetIt != targetTokenHolders_.end())
	{
		targetIt->second[type].erase(npc);

		// NPCとそのターゲットのマッピングを削除する
		if (targetIt->second[type].empty())
			targetTokenHolders_.erase(targetIt);
	}

	// NPCとそのターゲットのマッピングを削除する
	npcToTargetMap_.erase(it);
}

/// @brief クリア処理
void BattleDirector::Clear()
{
	targetTokenHolders_.clear();
	npcToTargetMap_.clear();
	targetSlots_.clear();
	npcCurrentSlots_.clear();
}

/// @brief 更新処理
/// @param dt 
void BattleDirector::Update(float dt)
{
	auto& characters = Character::GetCharacters();
	for (auto& character : characters)
	{
		// プレイヤーキャラクターを見つけたら、戦況のテンションを更新する
		if (character->IsPlayer())
		{
			UpdateTension(static_cast<Player*>(character));
			break;
		}
	}

	for (auto it = targetTokenCooldowns_.begin(); it != targetTokenCooldowns_.end(); )
	{
		// クールダウン時間を減算する
		if (it->second > 0.0f)
		{
			it->second -= dt;
			++it;
		} 
		else
		{
			// クールダウンが終了したターゲットを削除する
			it = targetTokenCooldowns_.erase(it);
		}
	}

	// 戦闘スロットを最適化する
	OptimizeSlots();
}

/// @brief 指定されたNPCが最も攻撃に適しているかどうかを判定する
/// @param npc 
/// @return 
bool BattleDirector::IsBestAttacker(Character* npc)
{
	// NPCが現在狙っているターゲットを取得
	Character* target = npc->GetLockOnTarget();
	if (!target)return false;

	// クールタイムが残っている場合は、最も近い攻撃者ではないと判断する
	if (npc->GetAttackCooltime() > 0.0f)
		return false;

	// スコアを計算する
	float myScore = CalculateUtilityScore(npc, target);

	// スコアが0以下の場合は、最も近い攻撃者ではないと判断する
	if (myScore <= 0.0f)return false;

	for (auto& other : Character::GetCharacters())
	{
		// 自分以外のキャラクターを確認する
		if (other == npc)
			continue;

		// 他のキャラクターが同じターゲットを狙っている場合は、スコアを計算する
		if (other->GetLockOnTarget() == target)
		{
			// 他のキャラクターの攻撃クールタイムが0以下の場合は、スコアを比較する
			if (other->GetAttackCooltime() <= 0.0f)
			{
				float otherScore = CalculateUtilityScore(other, target);
				if (otherScore > myScore)return false;
			}
		}
	}

	return true;
}

/// @brief NPCにスロットを割り当てる
/// @param npc 
/// @param target 
void BattleDirector::AssignSlot(Character* npc, Character* target)
{
	if (!npc || !target) return;

	// 既にスロットを持っている場合は何もしない
	if (npcCurrentSlots_.find(npc) != npcCurrentSlots_.end()) return;

	// ターゲット用のスロット配列が未作成なら初期化する
	if (targetSlots_.find(target) == targetSlots_.end())
	{
		std::vector<CombatSlot> slots(kMaxSlots);
		for (int i = 0; i < kMaxSlots; ++i)
		{
			// 45度（2π / 8）ずつずらして設定
			slots[i].angleOffset = (2.0f * std::numbers::pi_v<float> / kMaxSlots) * i;
			slots[i].distance = npc->GetSlotDistance();
		}

		// ターゲットに対するスロットを初期化して保存
		targetSlots_[target] = slots;
	}

	auto& slots = targetSlots_[target];
	int bestSlotIndex = -1;
	float minDistanceSq = std::numeric_limits<float>::max();

	Vector3 npcPos = npc->GetWorldPosition();
	Vector3 targetPos = target->GetWorldPosition();

	// 空いているスロットの中で、現在のNPCの座標に最も近いスロットを探す
	for (int i = 0; i < kMaxSlots; ++i)
	{
		if (!slots[i].isOccupied)
		{
			// スロットの角度オフセットを取得
			float finalAngle = slots[i].angleOffset;

			// ターゲットの回転を考慮してスロットのワールド座標を計算する
			float sx = targetPos.x + std::sin(finalAngle) * slots[i].distance;
			float sz = targetPos.z + std::cos(finalAngle) * slots[i].distance;
			Vector3 slotPos = { sx, npcPos.y, sz };

			// NavMeshがある場合は、スロットの座標をNavMesh上の最も近い点に修正する
			if (const NavMesh* navMesh = npc->GetNavMesh())
			{
				if (auto nearest = navMesh->GetNearestPoint(slotPos, 5.0f))
				{
					slotPos = nearest.value();
				}
			}

			Vector3 diff = slotPos - npcPos;
			float distSq = diff.LengthSq();

			// 最も近いスロットを更新
			if (distSq < minDistanceSq)
			{
				minDistanceSq = distSq;
				bestSlotIndex = i;
			}
		}
	}

	// スロットの割り当て
	if (bestSlotIndex != -1)
	{
		slots[bestSlotIndex].isOccupied = true;
		slots[bestSlotIndex].occupant = npc;
		npcCurrentSlots_[npc] = bestSlotIndex;
	}
}

/// @brief NPCのスロットを解放する
/// @param npc 
void BattleDirector::ReleaseSlot(Character* npc)
{
	auto it = npcCurrentSlots_.find(npc);
	if (it == npcCurrentSlots_.end()) return;

	int slotIndex = it->second;

	// ターゲットを特定してスロットを空ける（O(N)検索になりますが、Nが小さいので許容）
	for (auto& pair : targetSlots_)
	{
		auto& slots = pair.second;
		if (slotIndex < slots.size() && slots[slotIndex].occupant == npc)
		{
			slots[slotIndex].isOccupied = false;
			slots[slotIndex].occupant = nullptr;
			break;
		}
	}

	npcCurrentSlots_.erase(it);
}

/// @brief NPCのスロットのワールド座標を取得する
/// @param npc 
/// @param target 
/// @return 
std::optional<Vector3> BattleDirector::GetSlotWorldPosition(Character* npc, Character* target)
{
	// NPCがスロットを持っていない場合は、std::nulloptを返す
	auto it = npcCurrentSlots_.find(npc);
	if (it == npcCurrentSlots_.end()) return std::nullopt;

	// ターゲットのスロット情報を取得
	int slotIndex = it->second;
	auto targetIt = targetSlots_.find(target);
	if (targetIt == targetSlots_.end()) return std::nullopt;


	// 間合いを取ろうとしていないときのみ、自分とターゲットの攻撃者数を比較して、スロットの座標を返すかどうかを判断する
	if (!npc->IsTakingDistance())
	{
		// お互いに狙い合っているかを確認
		bool isMutualTargeting = (target->GetLockOnTarget() == npc);

		// 自分とターゲットの攻撃者数を取得
		int myAttackersCount = GetTargetingCount(npc);
		int targetAttackersCount = GetTargetingCount(target);

		// 自分がターゲットよりも多く狙われている場合は、スロットの座標を返さない
		if (myAttackersCount > targetAttackersCount)
		{
			return std::nullopt;
		}

		// お互いに狙い合っていて、かつ自分とターゲットがそれぞれ1人ずつしか狙われていない場合は、スロットの座標を返さない
		if (isMutualTargeting && myAttackersCount == 1 && targetAttackersCount == 1)
		{
			return std::nullopt;
		}
	}


	// スロットのワールド座標を計算する
	const auto& slot = targetIt->second[slotIndex];
	Vector3 targetPos = target->GetWorldPosition();

	// ターゲットの回転を考慮してスロットのワールド座標を計算する
	float radianOffset = slot.angleOffset + target->GetPersonalSlotDegreeOffset() * (std::numbers::pi_v<float> / 180.0f);

	Vector3 slotWorldPos;
	slotWorldPos.x = targetPos.x + std::sin(radianOffset) * slot.distance;
	slotWorldPos.y = targetPos.y; // 高さはターゲットまたは地形に合わせる
	slotWorldPos.z = targetPos.z + std::cos(radianOffset) * slot.distance;

	// NavMeshがある場合は、スロットの座標をNavMesh上の最も近い点に修正する
	if (const NavMesh* navMesh = npc->GetNavMesh())
	{
		if (auto nearest = navMesh->GetNearestPoint(slotWorldPos, 5.0f))
		{
			slotWorldPos = nearest.value();
		}
	}

	return slotWorldPos;
}

/// @brief ターゲットされているNPCの数を取得する
/// @param target 
/// @return 
int BattleDirector::GetTargetingCount(Character* target)
{
	// 全キャラクターのリストを取得
	auto characters = Character::GetCharacters();
	
	// ターゲットされているキャラクターの数をカウントする
	int targetedCount = 0;

	// 自分がプレイヤー側かどうかを判定する
	const bool kIsSelfPlayerSide = target->IsPlayerSide();

	for (Character* other : characters)
	{
		// 自分自身、死亡しているキャラクター、または同じ側のキャラクターはスキップする
		if (other == target || other->IsDead() || other->IsPlayerSide() == kIsSelfPlayerSide) continue;

		// 他のキャラクターがターゲットを狙っているかどうかを確認する
		if (other->GetLockOnTarget() == target)
		{
			targetedCount++;
		}
	}

	return targetedCount;
}

/// @brief 戦闘スロットを最適化する
void BattleDirector::OptimizeSlots()
{
	// ターゲットごとにスロットを最適化する
	for (auto& [target, slots] : targetSlots_)
	{
		if (!target || slots.empty()) continue;

		// ターゲットが死んでいる場合はスキップ
		if (target->IsDead()) continue;

		// 実際に占有されているスロットへのポインタ（またはインデックス）を収集
		std::vector<CombatSlot*> activeSlots;
		for (auto& slot : slots)
		{
			if (slot.isOccupied && slot.occupant)
			{
				activeSlots.push_back(&slot);
			}
		}

		int numAttackers = static_cast<int32_t>(activeSlots.size());
		if (numAttackers == 0) continue;

		// 360度を実際の攻撃者の数で分割し、均等な角度を計算する
		float angleStep = (2.0f * std::numbers::pi_v<float>) / numAttackers;

		// 各攻撃者のスロットの目標角度を更新する
		for (int i = 0; i < numAttackers; ++i)
		{
			Character* attacker = activeSlots[i]->occupant;

			// スロットの目標角度を更新する
			activeSlots[i]->angleOffset = i * angleStep;
		}
	}
}

/// @brief 攻撃トークンのユーティリティスコアを計算する
/// @param attacker 
/// @param target 
/// @return 
float BattleDirector::CalculateUtilityScore(Character* attacker, Character* target)
{
	// 攻撃者がダウンしている場合はスコアを0にする
	if (attacker->IsDownLying())
		return 0.0f;

	// ターゲットがプレイヤーで、コンボ中はスコアを0にする
	if (target->IsPlayer())
	{
		Player* player = static_cast<Player*>(target);
		if(player->IsCombo())
			return 0.0f;
	}

	// 基礎スコアを設定する
	float score = 1000.0f;

	// 距離に基づくスコアの減算
	Vector3 toTarget = target->GetWorldPosition() - attacker->GetWorldPosition();
	toTarget.y = 0.0f;
	float distanceSq = toTarget.LengthSq();
	score -= distanceSq * 2.0f; // 重みをかけて距離の二乗を減算する

	// プレイヤーがターゲットの場合、カメラの前方にいるかどうかを考慮してスコアを調整する
	if (target->IsPlayer())
	{
		// プレイヤーから攻撃者へのベクトルを求める
		Vector3 toAttacker = attacker->GetWorldPosition() - target->GetWorldPosition();
		toAttacker.y = 0.0f;

		if (toAttacker.LengthSq() > 0.0f)
		{
			toAttacker = toAttacker.Normalize();

			// カメラの前方向と攻撃者へのベクトルの内積を計算する
			float dot = (cameraForward_.x * toAttacker.x) + (cameraForward_.z * toAttacker.z);

			// カメラ前方にいる場合はスコアを大きく加算
			if (dot > 0.0f)
			{
				score += dot * 500.0f;
			}
			else
			{
				// カメラ後方にいる場合はスコアを減算して優先度を下げる
				score += dot * 300.0f;
			}
		}
	}

	// スコアが1.0未満にならないようにする
	score = std::max(1.0f, score);

	// 攻撃者の攻撃力に基づくスコアの加算
	float finalAggressiveness = attacker->GetAggressiveness() * globalTension_;
	score *= finalAggressiveness;

	return score;
}

/// @brief 戦況のテンションを更新する
/// @param player 
void BattleDirector::UpdateTension(Player* player)
{
	// プレイヤーでない場合は更新しない
	if (!player)return;

	// 通常のテンション値を設定する
	float newTension = 1.0f;

	// プレイヤーの体力比率を計算する
	float hpRatio = static_cast<float>(player->GetHp()) / static_cast<float>(player->GetMaxHp());

	// 体力が35%未満の場合
	if (hpRatio <= 0.35f)
	{
		newTension *= lowHpTensionMultiplier_;

	}
	else if (hpRatio >= 0.65f)
	{
		// 体力が65%以上の場合
		newTension *= highHpTensionMultiplier_;
	}
	else
	{
		// 体力が中程度の場合
		newTension *= mediumHpTensionMultiplier_;
	}

	// 補間で徐々にテンションを変化させる
	float dt = GrowthEngine::GetInstance()->GetDeltaTime();
	globalTension_ = Lerp(globalTension_, newTension, dt * 2.0f);
}