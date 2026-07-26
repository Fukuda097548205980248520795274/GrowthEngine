#pragma once
#include "../BasePrefabBase.h"

class PrefabInstanceTube;

class PrefabBaseTube : public Engine::BasePrefabBase
{
public:

	/// @brief コンストラクタ
	/// @param hTexture 
	/// @param numInstance 
	/// @param name 
	PrefabBaseTube(TextureHandle hTexture, uint32_t numInstance, const std::string& name);

	/// @brief インスタンスを生成する
	[[nodiscard]]
	PrefabInstanceTube* CreateInstance();

	/// @brief 描画処理
	void Draw() override;

	// パラメータ
	Engine::Prefab3D::Tube::Base::Param* param_ = nullptr;


private:

	// 3Dプレハブブハンドル
	Prefab3DHandle handle_ = 0;
};

