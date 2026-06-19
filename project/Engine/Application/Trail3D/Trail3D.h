#pragma once
#include "Store/TrailStore/TrailData/TrailData.h"

class GrowthEngine;

class Trail3D
{
public:

	/// @brief コンストラクタ
	/// @param name 
	/// @param maxLifeTime 
	/// @param hTexture 
	Trail3D(const std::string& name,float maxLifeTime, TextureHandle hTexture);

	/// @brief 描画処理
	void Draw();

	/// @brief パラメータ
	Engine::TrailData::Param* param_ = nullptr;


private:

	/// @brief エンジン
	const GrowthEngine* engine_ = nullptr;

	/// @brief 名前
	std::string name_{};

	/// @brief ハンドル
	TrailHandle hTrail_ = 0;
};

