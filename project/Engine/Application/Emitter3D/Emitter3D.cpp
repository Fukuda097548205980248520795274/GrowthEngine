#include "Emitter3D.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param particleName 
/// @param emitterIndex 
Emitter3D::Emitter3D(const std::string& particleName)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// エミッターインデックスを取得する
	emitterIndex_ = engine_->GetEmitter3DIndex(particleName);

	// パラメータを取得する
	param_ = engine_->Get3DEmitter(particleName, emitterIndex_);
	assert(param_);

	// エミッターが確定したので、パーティクル名を記録する
	particleName_ = particleName;
}

/// @brief 放出
void Emitter3D::Emit()
{
	// 放出する
	engine_->EmittParticle3D(particleName_, emitterIndex_);
}

/// @brief 停止
void Emitter3D::Stop()
{
	// 停止する
	engine_->StopParticle3D(particleName_, emitterIndex_);
}