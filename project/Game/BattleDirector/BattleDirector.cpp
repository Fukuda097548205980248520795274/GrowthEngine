#include "BattleDirector.h"
#include "Entity/Character/Character.h"
#include "Entity/Character/Player/Player.h"

/// @brief インスタンスを取得する
/// @return 
BattleDirector& BattleDirector::GetInstance()
{
	static BattleDirector instance;
	return instance;
}

/// @brief 攻撃トークンを要求する
/// @param npc 
/// @return 
bool BattleDirector::RequestAttackToken(Character* npc)
{
	// NPCが現在狙っているターゲットを取得
	Character* target = npc->GetLockOnTarget();

	// ターゲットがいない場合は、攻撃トークンの要求を許可する
	if (!target)return true;

	// ターゲットのクールダウン時間を確認する
	if (targetTokenCooldowns_[target] > 0.0f)
		return false;

	// 現在の攻撃トークン保持者を取得
	auto it = npcToTargetMap_.find(npc);
	if (it != npcToTargetMap_.end())
	{
		// すでに攻撃トークンを保持している場合は、同じターゲットに対する要求かどうかを確認する
		if (it->second == target)return true;

		// 別のターゲットに対する要求の場合は、現在の攻撃トークンを返却する
		ReleaseAttackToken(npc);
	}

	// 現在の攻撃トークン保持者を取得
	auto& holders = targetTokenHolders_[target];

	// 攻撃トークンの最大数に達していない場合は、攻撃トークンを許可する
	if (holders.size() < maxAttackTokens_)
	{
		// 攻撃トークンを保持しているキャラクターのセットにNPCを追加する
		holders.insert(npc);

		// NPCとそのターゲットのマッピングを更新する
		npcToTargetMap_[npc] = target;
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
	Character* target = it->second;


	// 攻撃トークンのクールタイムを計算する
	float bestInterval = 1.5f;
	float finalInterval = bestInterval / globalTension_;

	// ターゲットのクールタイムを設定する
	targetTokenCooldowns_[target] = finalInterval;


	// 攻撃クールタイムを計算する
	float bestCooltime = 5.0f;
	float finalCooltime = bestCooltime / npc->GetAggressiveness();

	// NPCの攻撃クールタイムを設定する
	npc->SetAttackCooltime(finalCooltime);


	// NPCとそのターゲットのマッピングを削除する
	auto targetIt = targetTokenHolders_.find(target);
	if (targetIt != targetTokenHolders_.end())
	{
		targetIt->second.erase(npc);

		// NPCとそのターゲットのマッピングを削除する
		if (targetIt->second.empty())
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

	for (auto& other : Character::GetCharacters())
	{
		// 自分以外のキャラクターを確認する
		if (other == npc)
			continue;

		// 他のキャラクターが同じターゲットを狙っている場合は、スコアを計算する
		if (other->GetLockOnTarget() == target)
		{
			// 他のキャラクターのクールタイムが残っていない場合は、スコアを比較する
			if (other->GetAttackCooltime() <= 0.0f)
			{
				float otherScore = CalculateUtilityScore(other, target);
				if (otherScore > myScore)
					return false;
			}
		}
	}

	return true;
}

/// @brief 攻撃トークンのユーティリティスコアを計算する
/// @param attacker 
/// @param target 
/// @return 
float BattleDirector::CalculateUtilityScore(Character* attacker, Character* target)
{
	// 基礎スコアを設定する
	float score = 1000.0f;

	// 距離に基づくスコアの減算
	Vector3 toTarget = target->GetWorldPosition() - attacker->GetWorldPosition();
	toTarget.y = 0.0f;
	float distanceSq = toTarget.LengthSq();
	score -= distanceSq * 2.0f; // 重みをかけて距離の二乗を減算する

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