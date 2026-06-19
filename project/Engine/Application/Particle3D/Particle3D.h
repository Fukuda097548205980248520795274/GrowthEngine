#pragma once
#include "Handle/Handle.h"
#include <string>
#include "Data/ParticleData/ParticleData.h"

class GrowthEngine;

class Particle3D
{
public:

	/// @brief コンストラクタ
	/// @param name 
	/// @param numInstance 
	/// @param numEmitter 
	/// @param hModel 
	Particle3D(const std::string& name, uint32_t numInstance, uint32_t numEmitter, ModelHandle hModel);

	/// @brief 放出
	/// @param emitterIndex 
	void Emit(int32_t emitterIndex);

	/// @brief 停止
	/// @param emitterIndex 
	void Stop(int32_t emitterIndex);

	/// @brief 描画処理
	void Draw();

	/// @brief ハンドルを取得する
	/// @return 
	Particle3DHandle GetHandle() const { return hParticle_; }

	/// @brief 名前を取得する
	/// @return 
	std::string GetName() const { return name_; }

	/// @brief パラメータ
	Engine::Particle3D::Param* param_ = nullptr;


private:

	/// @brief エンジン
	const GrowthEngine* engine_ = nullptr;

	/// @brief ハンドル
	Particle3DHandle hParticle_ = 0;

	/// @brief 名前
	std::string name_{};

	/// @brief インスタンスの数
	uint32_t numInstance_ = 0;

	/// @brief エミッターの数
	uint32_t numEmitter_ = 0;

	/// @brief モデルハンドル
	ModelHandle hModel_ = 0;
};

