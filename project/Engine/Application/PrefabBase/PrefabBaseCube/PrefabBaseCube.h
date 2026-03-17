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

	/// @brief 描画処理
	void Draw() override;

	// パラメータ
	Engine::Prefab::Cube::Base::Param* param_ = nullptr;


private:

	// プレハブプリミティブハンドル
	PrefabPrimitiveHandle handle_ = 0;
};

