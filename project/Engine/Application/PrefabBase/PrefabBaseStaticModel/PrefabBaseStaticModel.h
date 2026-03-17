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

	/// @brief 描画処理
	void Draw() override;

	// パラメータ
	Engine::Prefab::StaticModel::Base::Param* param_ = nullptr;


private:

	// プレハブプリミティブハンドル
	PrefabPrimitiveHandle handle_ = 0;
};

