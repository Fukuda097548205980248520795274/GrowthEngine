#pragma once
#include "Handle/Handle.h"
#include <string>
#include "Data/ParticleData/ParticleData.h"

class GrowthEngine;

class Emitter3D
{
public:

	/// @brief コンストラクタ
	/// @param particleName 
	/// @param emitterIndex 
	Emitter3D(const std::string& particleName);

	/// @brief 放出
	void Emit();

	/// @brief 停止
	void Stop();

	/// @brief パラメータ
	Engine::Particle3D::Emitter* param_ = nullptr;


private:

	/// @brief エンジン
	const GrowthEngine* engine_ = nullptr;

	// パーティクル名
	std::string particleName_;

	// エミッターインデックス
	int32_t emitterIndex_ = 0;
};

