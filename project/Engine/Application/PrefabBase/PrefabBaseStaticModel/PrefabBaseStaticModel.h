#pragma once
#include "../BasePrefabBase.h"

class PrefabInstanceStaticModel;

class PrefabBaseStaticModel : public Engine::BasePrefabBase
{
public:

	/// @brief コンストラクタ
	/// @param hModel モデルハンドル
	/// @param numInstance インスタンス数
	/// @param name 名前
	PrefabBaseStaticModel(ModelHandle hModel, uint32_t numInstance, const std::string& name);

	/// @brief インスタンスを生成する
	[[nodiscard]]
	PrefabInstanceStaticModel* CreateInstance();

	// パラメータ
	Engine::Prefab3D::StaticModel::Base::Param* param_ = nullptr;


private:

	// プレハブプリミティブハンドル
	Prefab3DHandle handle_ = 0;
};

