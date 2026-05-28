#include "EffectManager.h"

/// @brief インスタンス
std::unique_ptr<EffectManager> EffectManager::instance_ = nullptr;

/// @brief インスタンスを取得する
/// @return 
EffectManager* EffectManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_.reset(new EffectManager());
		instance_->Initialize();
	}

	return instance_.get();
}


/// @brief 初期化処理
void EffectManager::Initialize()
{
	// ガードエフェクトのモデルを生成
	guardEffectModel_ = std::make_unique<PrefabBaseTube>(engine_->LoadTexture("./Assets/Textures/white2x2.png"), 100, "guardEffect");
	guardEffectModel_->param_->blendMode = BlendMode::kNormal;
}

/// @brief 更新処理
void EffectManager::Update()
{
	// ガードエフェクトの更新と終了したエフェクトの削除
	guardEffects_.remove_if(
		[](std::unique_ptr<GuardEffect>& effect)
		{
			effect->Update();
			return effect->IsFinished();
		}
	);
}

/// @brief 描画処理
void EffectManager::Draw()
{
	// ガードエフェクトの描画
	for (auto& effect : guardEffects_)
		effect->Draw();

	// ガードエフェクトのモデルを描画
	guardEffectModel_->Draw();
}

/// @brief ガードエフェクトを生成する
/// @param position 
/// @param rotate 
void  EffectManager::CreateGuardEffect(const Vector3& position, const Vector3& rotate)
{
	std::unique_ptr<GuardEffect> guardEffect = std::make_unique<GuardEffect>();
	guardEffect->Initialize(guardEffectModel_->CreateInstance(), position, rotate);

	guardEffects_.push_back(std::move(guardEffect));
}