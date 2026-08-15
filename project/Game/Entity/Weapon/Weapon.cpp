#include "Weapon.h"
#include "Entity/Character/Character.h"
#include "StageEditor/StageData/StageData.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"

#include "Entity/Character/Player/Player.h"
#include "Entity/Character/NPC/NPC.h"

#include <numbers>

// 武器インスタンスの共有リスト
std::vector<Weapon*> Weapon::weapons_{};

/// @brief コンストラクタ
/// @param initData 
Weapon::Weapon(const InitData& initData) : Entity()
{
	// インスタンスリストに登録する
	weapons_.push_back(this);

	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 位置
	worldTransform_->translate_ = initData.position;

	// 耐久力
	durability_ = initData.durability;

	// 攻撃力
	attackPower_ = initData.attackPower;

	// カテゴリ
	category_ = initData.category;

	// 壊れない武器かどうか
	isUnbreakable_ = initData.isUnbreakable;

	// モデル
	if (initData.model)
	{
		model_ = initData.model;

		// ワールドトランスフォームの親をモデルに設定する
		model_->param_.parent = worldTransform_.get();
	}

	// ワールドトランスフォームを更新する
	worldTransform_->Update();

	// 着地判定
	if (initData.landingCollision)
	{
		landingCollision_ = initData.landingCollision;

		// コリジョンの位置を武器の位置に設定する
		landingCollision_->param_->start = GetWorldPosition();
		landingCollision_->param_->radius = 0.05f;
		landingCollision_->param_->diff = Vector3(0.0f, 0.0f, 0.0f);
	}
}

/// @brief デストラクタ
Weapon::~Weapon()
{
	// 当たり判定を削除する
	if (landingCollision_)landingCollision_->Delete();
	landingCollision_ = nullptr;

	// モデルを削除する
	if (model_)model_->isDelete_ = true;
	model_ = nullptr;

	if (owner_)
	{
		owner_->ReleaseWeapon();
		owner_ = nullptr;
	}

	// インスタンスリストから自分を除外する
	auto it = std::find(weapons_.begin(), weapons_.end(), this);
	if (it != weapons_.end())
	{
		weapons_.erase(it);
	}
}

/// @brief 更新処理
void Weapon::Update()
{
	// 更新が無効なら何もしない
	if (!updateEnabled_)return;

	// 有効でないときは更新しない
	if (!isActive_)return;

	// デルタタイムを取得する
	float dt = engine_->GetDeltaTime() * engine_->GetTimeScale();

	if (!isBreak_)
	{
		// 着地しているかどうか
		LandingCheck();

		// 持ち主がいるときは持ち主の位置に追従する
		if (IsEquipped())
		{
			// カテゴリに応じてトランスフォームを調整する
			switch (category_)
			{
				// 片手武器
			case WeaponCategory::OneHanded:

				// 90度回転させる
				worldTransform_->rotate_.z = std::numbers::pi_v<float> / 2.0f;

				// ワールドトランスフォームの親を持ち主の右手に設定する
				worldTransform_->SetParent(owner_->GetBoneMatrix(JointType::HandR));

				break;


				// 両手武器
			case WeaponCategory::TwoHanded:

				break;
			}
		}

		// 落下の更新
		FallUpdate(dt);

		// 基底クラスの更新
		Entity::Update();

		// 耐久力が0以下で壊れない武器でないときは壊れる
		if (durability_ <= 0 && !isUnbreakable_)
		{
			isBreak_ = true;

			// 当たり判定を削除する
			if (landingCollision_)landingCollision_->Delete();
			landingCollision_ = nullptr;

			return;
		}

		// 着地判定の位置を更新する
		if (landingCollision_)
		{
			landingCollision_->param_->diff = GetWorldPosition() - landingCollision_->param_->start;
			landingCollision_->param_->start = GetWorldPosition();
		}
	}
}

/// @brief 描画処理
void Weapon::Draw()
{
	// 有効でないときは描画しない
	if (!isActive_)return;

	// 壊れている武器は描画しない
	if (isBreak_) return;

	// モデルがあるときは描画する
	if (model_)model_->Draw();
}

/// @brief 武器に入るダメージ
/// @param damage 
void Weapon::TakeDamage(int damage)
{
	// 壊れない武器なら、耐久力を減らさない
	if (isUnbreakable_) return;

	// 耐久力を減らす
	durability_ -= damage;
}

/// @brief 所持者を設定する
/// @param owner 
void Weapon::SetOwner(Character* owner)
{
	// 所持者がnullptrのときは、所持者を解除する
	if (!owner)
	{
		owner_ = nullptr;
		return;
	}

	// 所持者を設定する
	owner_ = owner;

	// 所持者がPlayerかNPCかで、状態ツリーの所有者を設定する
	if (owner_->IsPlayer())
	{
		if (stateTrees_.empty()) return;
		for (auto& [stateName, treeSet] : stateTrees_)
		{
			if (treeSet.comboTreeX) treeSet.comboTreeX->SetOwner(static_cast<Player*>(owner_));
			if (treeSet.comboTreeY) treeSet.comboTreeY->SetOwner(static_cast<Player*>(owner_));
			if (treeSet.comboTreeB) treeSet.comboTreeB->SetOwner(static_cast<Player*>(owner_));
		}
	}
	else
	{
		if (stateTrees_.empty()) return;
		for (auto& [stateName, treeSet] : stateTrees_)
		{
			if (treeSet.behaviorTree) treeSet.behaviorTree->SetOwner(static_cast<NPC*>(owner_));
		}
	}
}

/// @brief 武器を吹き飛ばす
/// @param velocity 
void Weapon::BlowAway(const Vector3& velocity)
{
	// 親を外す
	if (worldTransform_)
	{
		worldTransform_->SetParent(nullptr);
	}

	// 吹き飛び速度を設定
	velocity_ = velocity;
	isGrounded_ = false;
}

/// @brief 武器の状態ツリーセットを取得する
/// @param stateName 
/// @return 
WeaponStateTreeSet* Weapon::GetStateTreeSet(const std::string& stateName)
{
	auto it = stateTrees_.find(stateName);
	if (it != stateTrees_.end()) return &it->second;
	return nullptr;
}

/// @brief 落下の更新
/// @param deltaTime 
void Weapon::FallUpdate(float deltaTime)
{
	if (!isGrounded_)
	{
		// 重力による落下処理（Y軸のみ加速）
		velocity_.y += kGravity * deltaTime;
		if (velocity_.y < kMaxFallSpeed) velocity_.y = kMaxFallSpeed;

		// X, Y, Z 全て方向の速度を位置に反映する
		worldTransform_->translate_ += velocity_ * deltaTime;

		// 速度の減衰率を計算する
		float drag = std::pow(0.5f, deltaTime);
		velocity_.x *= drag;
		velocity_.z *= drag;
	}
}

/// @brief 着地判定の更新
void Weapon::LandingCheck()
{
	// 着地フラグをリセットする
	isGrounded_ = false;

	// 所持されているときは着地しているとみなす
	if (IsEquipped())
	{
		isGrounded_ = true;

		// 所持されている間は速度をリセット
		velocity_ = Vector3(0.0f, 0.0f, 0.0f);
		return;
	}

	// 着地判定の衝突判定がないときは着地していないとみなす
	if (!landingCollision_) return;

	// 着地判定の衝突があるかどうか
	if (landingCollision_->isCollision_)
	{
		for (auto& hitOpponent : landingCollision_->hitOpponents_)
		{
			// 着地判定の相手がAABBでない場合は無視する
			if (hitOpponent->GetType() != Engine::Collision3D::Type::AABB) 
				continue;

			// 着地したときの位置を計算する
			auto floorCollision = static_cast<Collision3DInstanceAABB*>(hitOpponent);
			worldTransform_->translate_.y = floorCollision->param_->center.y + floorCollision->param_->radius.y;
		}

		// 着地したので接地フラグを立てる
		isGrounded_ = true;

		// 着地したので吹き飛び・落下速度を完全にリセットする
		velocity_ = Vector3(0.0f, 0.0f, 0.0f);
	}
}

/// @brief デバッグUIを描画する
/// @param placementData 
/// @param placementList 
/// @param history 
/// @param isDirty 
void Weapon::DrawDebugUI(PlacementData* placementData, std::vector<PlacementData>& placementList, StageEditorHistory* history, bool* isDirty)
{
#ifdef DEVELOPMENT

	// 基底クラスのデバッグUIを描画する
	Entity::DrawDebugUI(placementData, placementList, history, isDirty);

	ImGui::Separator();

	// 耐久力のドラッグ
	if (ImGui::IsItemActivated()) { history->SaveHistory(placementList); *isDirty = true; }
	ImGui::DragInt("耐久力", &durability_, 1, 0, 1000000);
	if (ImGui::IsItemDeactivatedAfterEdit())placementData->durability = durability_;

	// 攻撃力のドラッグ
	if (ImGui::IsItemActivated()) { history->SaveHistory(placementList); *isDirty = true; }
	ImGui::DragFloat("攻撃力", &attackPower_, 0.01f, 0.0f, 1000000.0f);
	if (ImGui::IsItemDeactivatedAfterEdit())placementData->attackPower = attackPower_;

	// 壊れない武器かどうかのチェックボックス
	if (ImGui::IsItemActivated()) { history->SaveHistory(placementList); *isDirty = true; }
	ImGui::Checkbox("壊れないかどうか", &isUnbreakable_);
	if (ImGui::IsItemDeactivatedAfterEdit())placementData->isUnbreakable = isUnbreakable_;

#endif
}