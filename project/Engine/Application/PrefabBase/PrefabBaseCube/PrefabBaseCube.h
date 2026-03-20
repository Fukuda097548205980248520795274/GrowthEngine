#pragma once
#include "../BasePrefabBase.h"

class PrefabInstanceCube;

class PrefabBaseCube : public Engine::BasePrefabBase
{
public:

	/// @brief コンストラクタ
	/// @param hTexture 
	/// @param numInstance 
	/// @param name 
	PrefabBaseCube(TextureHandle hTexture, uint32_t numInstance, const std::string& name);

	/// @brief インスタンスを生成する
	[[nodiscard]]
	PrefabInstanceCube* CreateInstance();

	// パラメータ
	Engine::Prefab3D::Cube::Base::Param* param_ = nullptr;


private:

	// 3Dプレハブブハンドル
	Prefab3DHandle handle_ = 0;
};

