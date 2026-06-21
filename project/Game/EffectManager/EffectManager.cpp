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
	engine_->LoadTexture("./Assets/Textures/smoke_001.png");
	engine_->LoadTexture("./Assets/Textures/impact_000.png");
	engine_->LoadTexture("./Assets/Textures/color_effect.png");
	engine_->LoadTexture("./Assets/Textures/damage_effect.png");
	engine_->LoadTexture("./Assets/Textures/thunder_effect.png");
	engine_->LoadTexture("./Assets/Textures/color_effect_bloom.png");
	engine_->LoadTexture("./Assets/Textures/chip.png");
	engine_->LoadTexture("./Assets/Textures/chip_bloom.png");

	// ガードエフェクトのモデルを生成
	guardEffectModel_ = std::make_unique<PrefabBaseTube>(engine_->LoadTexture("./Assets/Textures/white2x2.png"), 100, "guardEffect");
	guardEffectModel_->param_->blendMode = BlendMode::kNormal;
	guardEffectModel_->param_->material.enableLighting = false;
	guardEffectModel_->param_->material.enableShadow = false;
	guardEffectModel_->param_->material.drawShadowMap = false;

	// 弾きエフェクトのモデルを生成
	deflectEffectModel_ = std::make_unique<PrefabBaseTube>(engine_->LoadTexture("./Assets/Textures/gradation.png"), 100, "deflectEffect");
	deflectEffectModel_->param_->blendMode = BlendMode::kAdd;
	deflectEffectModel_->param_->material.enableLighting = false;
	deflectEffectModel_->param_->material.enableShadow = false;
	deflectEffectModel_->param_->material.drawShadowMap = false;

	// スパークパーティクル000を生成
	spark000_ = std::make_unique<Particle3D>("spark_000", 1000, 10, engine_->LoadModel("./Assets/Models/particle", "particle.obj"));

	// インパクト000を生成
	impact000_ = std::make_unique<Particle3D>("impact_000", 100, 20, engine_->LoadModel("./Assets/Models/particle", "particle.obj"));
	impact001_ = std::make_unique<Particle3D>("impact_001", 100, 20, engine_->LoadModel("./Assets/Models/particle", "particle.obj"));
	impact002_ = std::make_unique<Particle3D>("impact_002", 100, 20, engine_->LoadModel("./Assets/Models/particle", "particle.obj"));
	impact003_ = std::make_unique<Particle3D>("impact_003", 100, 20, engine_->LoadModel("./Assets/Models/particle", "particle.obj"));
	impact004_ = std::make_unique<Particle3D>("impact_004", 100, 20, engine_->LoadModel("./Assets/Models/particle", "particle.obj"));
	impact005_ = std::make_unique<Particle3D>("impact_005", 1000, 20, engine_->LoadModel("./Assets/Models/particle", "particle.obj"));

	// インパクトドロップ000を生成
	impactDrop000_ = std::make_unique<Particle3D>("impactDrop_000", 1000, 20, engine_->LoadModel("./Assets/Models/particle", "particle.obj"));

	// インパクトスモークを生成
	impactSmoke000_ = std::make_unique<Particle3D>("impactSmoke_000", 1000, 20, engine_->LoadModel("./Assets/Models/particle", "particle.obj"));
	impactSmoke001_ = std::make_unique<Particle3D>("impactSmoke_001", 1000, 20, engine_->LoadModel("./Assets/Models/particle", "particle.obj"));

	// 吹っ飛びスモークを生成
	blownSmoke000_ = std::make_unique<Particle3D>("blownSmoke_000", 1000, 20, engine_->LoadModel("./Assets/Models/particle", "particle.obj"));

	// インパクト地面を生成
	impactGround000_ = std::make_unique<Particle3D>("impactGround_000", 100, 20, engine_->LoadModel("./Assets/Models/particleCylinder", "particleCylinder.obj"));
	impactGround001_ = std::make_unique<Particle3D>("impactGround_001", 100, 20, engine_->LoadModel("./Assets/Models/particleCylinder", "particleCylinder.obj"));
	impactGround002_ = std::make_unique<Particle3D>("impactGround_002", 100, 20, engine_->LoadModel("./Assets/Models/particleCylinder", "particleCylinder.obj"));
	impactGround003_ = std::make_unique<Particle3D>("impactGround_003", 100, 20, engine_->LoadModel("./Assets/Models/particleCylinder", "particleCylinder.obj"));
	impactGround004_ = std::make_unique<Particle3D>("impactGround_004", 100, 20, engine_->LoadModel("./Assets/Models/particleCylinder", "particleCylinder.obj"));
	impactGround005_ = std::make_unique<Particle3D>("impactGround_005", 1000, 20, engine_->LoadModel("./Assets/Models/particle", "particle.obj"));
	impactGround006_ = std::make_unique<Particle3D>("impactGround_006", 1000, 20, engine_->LoadModel("./Assets/Models/particle", "particle.obj"));

	// 走りスモークを生成
	dashSmoke000_ = std::make_unique<Particle3D>("dashSmoke_000", 1000, 30, engine_->LoadModel("./Assets/Models/particle", "particle.obj"));
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

	// 弾きエフェクトの更新と終了したエフェクトの削除
	deflectEffects_.remove_if(
		[](std::unique_ptr<DeflectEffect>& effect)
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

	// 弾きエフェクトの描画
	for (auto& effect : deflectEffects_)
		effect->Draw();

	// 吹っ飛びスモーク000を描画
	blownSmoke000_->Draw();

	// 走りスモーク000を描画
	dashSmoke000_->Draw();

	// インパクトドロップ000を描画
	impactDrop000_->Draw();

	// インパクトスモーク001を描画
	impactSmoke000_->Draw();
	impactSmoke001_->Draw();

	// インパクト000を描画
	impact004_->Draw();
	impact005_->Draw();
	impact001_->Draw();
	impact002_->Draw();
	impact003_->Draw();
	impact000_->Draw();

	// インパクト地面000を描画
	impactGround001_->Draw();
	impactGround000_->Draw();
	impactGround002_->Draw();
	impactGround004_->Draw();
	impactGround003_->Draw();
	impactGround005_->Draw();
	impactGround006_->Draw();

	// スパーク000を描画
	spark000_->Draw();

	// ガードエフェクトのモデルを描画
	guardEffectModel_->Draw();

	// 弾きエフェクトのモデルを描画
	deflectEffectModel_->Draw();
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

/// @brief 弾きエフェクトを生成する
/// @param position 
void EffectManager::CreateDeflectEffect(const Vector3& position)
{
	std::unique_ptr <DeflectEffect> deflectEffect = std::make_unique<DeflectEffect>();
	deflectEffect->Initialize(deflectEffectModel_->CreateInstance(), deflectEffectModel_->CreateInstance(), deflectEffectModel_->CreateInstance(), position);

	deflectEffects_.push_back(std::move(deflectEffect));
}

/// @brief スパークを放出する
/// @param position 
void EffectManager::EmitSpark000(const Vector3& position)
{
	Emitter3D emitter("spark_000");
	emitter.param_->position = position;
	emitter.Emit();
}

/// @brief インパクトを放出する
/// @param position 
/// @param rotate 
void EffectManager::Impact000(const Vector3& position, const Vector3& rotate)
{
	impact000_->param_->rotate.axis = Vector3(0.0f, 1.0f, 0.0f);
	impact000_->param_->rotate.start = -rotate.y;
	impact000_->param_->rotate.end = -rotate.y;

	Emitter3D emitter("impact_000");
	emitter.param_->position = position;
	emitter.Emit();
}

/// @brief インパクトを放出する
/// @param position 
void EffectManager::Impact001(const Vector3& position)
{
	Emitter3D emitter("impact_001");
	emitter.param_->position = position;
	emitter.Emit();
}

/// @brief インパクトを放出する
/// @param position 
void EffectManager::Impact002(const Vector3& position)
{
	Emitter3D emitter("impact_002");
	emitter.param_->position = position;
	emitter.Emit();
}

/// @brief インパクトを放出する
/// @param position 
void EffectManager::Impact003(const Vector3& position)
{
	Emitter3D emitter("impact_003");
	emitter.param_->position = position;
	emitter.Emit();
}

/// @brief インパクトを放出する
/// @param position 
void EffectManager::Impact004(const Vector3& position)
{
	Emitter3D emitter("impact_004");
	emitter.param_->position = position;
	emitter.Emit();
}

/// @brief インパクトを放出する
/// @param position 
void EffectManager::Impact005(const Vector3& position)
{
	Emitter3D emitter("impact_005");
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

/// @brief インパクトスモークを放出する
/// @param position 
void EffectManager::ImpactSmoke001(const Vector3& position)
{
	Emitter3D emitter("impactSmoke_001");
	emitter.param_->position = position;
	emitter.Emit();
}

/// @brief 吹っ飛びスモークを放出する
/// @param position 
void EffectManager::BlownSmoke000(const Vector3& position)
{
	Emitter3D emitter("blownSmoke_000");
	emitter.param_->position = position;
	emitter.Emit();
}

/// @brief インパクト地面を放出する
/// @param position 
void EffectManager::ImpactGround000(const Vector3& position)
{
	Emitter3D emitter("impactGround_000");
	emitter.param_->position = position + Vector3(0.0f, -0.0f, 0.0f);
	emitter.Emit();
}

/// @brief インパクト地面を放出する
/// @param position 
void EffectManager::ImpactGround001(const Vector3& position)
{
	Emitter3D emitter("impactGround_001");
	emitter.param_->position = position + Vector3(0.0f, -0.25f, 0.0f);
	emitter.Emit();
}

/// @brief インパクト地面を放出する
/// @param position 
void EffectManager::ImpactGround002(const Vector3& position)
{
	Emitter3D emitter("impactGround_002");
	emitter.param_->position = position + Vector3(0.0f, -0.0f, 0.0f);
	emitter.Emit();
}

/// @brief インパクト地面を放出する
/// @param position 
void EffectManager::ImpactGround003(const Vector3& position)
{
	Emitter3D emitter("impactGround_003");
	emitter.param_->position = position + Vector3(0.0f, -0.0f, 0.0f);
	emitter.Emit();
}

/// @brief インパクト地面を放出する
/// @param position 
void EffectManager::ImpactGround004(const Vector3& position)
{
	Emitter3D emitter("impactGround_004");
	emitter.param_->position = position + Vector3(0.0f, -0.0f, 0.0f);
	emitter.Emit();
}

/// @brief インパクト地面を放出する
/// @param position 
void EffectManager::ImpactGround005(const Vector3& position)
{
	Emitter3D emitter("impactGround_005");
	emitter.param_->position = position + Vector3(0.0f, -0.3f, 0.0f);
	emitter.Emit();
}

/// @brief インパクト地面を放出する
/// @param position 
void EffectManager::ImpactGround006(const Vector3& position)
{
	Emitter3D emitter("impactGround_006");
	emitter.param_->position = position + Vector3(0.0f, -0.4f, 0.0f);
	emitter.Emit();
}

/// @brief 走りスモークを放出する
/// @param position 
void EffectManager::DashSmoke000(const Vector3& position)
{
	Emitter3D emitter("dashSmoke_000");
	emitter.param_->position = position;
	emitter.Emit();
}