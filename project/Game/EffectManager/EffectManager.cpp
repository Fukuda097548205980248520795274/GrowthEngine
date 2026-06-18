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
	// テクスチャを読み込む
	engine_->LoadTexture("./Assets/Textures/smoke_000.png");

	// ガードエフェクトのモデルを生成
	guardEffectModel_ = std::make_unique<PrefabBaseTube>(engine_->LoadTexture("./Assets/Textures/white2x2.png"), 100, "guardEffect");
	guardEffectModel_->param_->blendMode = BlendMode::kNormal;

	// スパークパーティクル000を生成
	spark000_ = std::make_unique<Particle3D>("spark_000", 1000, 10, engine_->LoadModel("./Assets/Models/particle", "particle.obj"));

	// インパクトドロップ000を生成
	impactDrop000_ = std::make_unique<Particle3D>("impactDrop_000", 1000, 20, engine_->LoadModel("./Assets/Models/particle", "particle.obj"));

	// インパクトスモーク000を生成
	impactSmoke000_ = std::make_unique<Particle3D>("impactSmoke_000", 1000, 20, engine_->LoadModel("./Assets/Models/particle", "particle.obj"));
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

	// インパクトドロップ000を描画
	impactDrop000_->Draw();

	// インパクトスモーク000を描画
	impactSmoke000_->Draw();

	// スパーク000を描画
	spark000_->Draw();

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

/// @brief スパークを放出する
/// @param position 
void EffectManager::EmitSpark000(const Vector3& position)
{
	Emitter3D emitter("spark_000");
	emitter.param_->position = position;
	emitter.Emit();
}

/// @brief インパクトドロップを放出する
/// @param position 
void EffectManager::ImpactDrop000(const Vector3& position)
{
	Emitter3D emitter("impactDrop_000");
	emitter.param_->position = position;
	emitter.Emit();
}

/// @brief インパクトスモークを放出する
/// @param position 
void EffectManager::ImpactSmoke000(const Vector3& position)
{
	Emitter3D emitter("impactSmoke_000");
	emitter.param_->position = position;
	emitter.Emit();
}