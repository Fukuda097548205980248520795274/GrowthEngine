#include "Particle3D.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
/// @param numInstance 
/// @param numEmitter 
/// @param hModel 
Particle3D::Particle3D(const std::string& name, uint32_t numInstance, uint32_t numEmitter, ModelHandle hModel) :
	name_(name),numInstance_(numInstance),numEmitter_(numEmitter),hModel_(hModel)
{
	// エンジンのインスタンスを取得
	engine_ = GrowthEngine::GetInstance();

	// パーティクルを読み込む
	hParticle_ = engine_->LoadParticle3D(name_, numInstance_, numEmitter_, hModel_);

	// パラメータを取得
	param_ = engine_->GetParticle3DParam(hParticle_);
}

/// @brief 放出
/// @param emitterIndex 
void Particle3D::Emit(int32_t emitterIndex)
{
	engine_->EmitParticle3D(hParticle_, emitterIndex);
}

/// @brief 停止
/// @param emitterIndex 
void Particle3D::Stop(int32_t emitterIndex)
{
	engine_->StopParticle3D(hParticle_, emitterIndex);
}

/// @brief 描画処理
void Particle3D::Draw()
{
	engine_->DrawParticle3D(hParticle_);
}