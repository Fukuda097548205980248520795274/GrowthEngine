#pragma once
#include "Handle/Handle.h"
#include "Data/PrefabData/PrefabData.h"
#include <string>

class GrowthEngine;
class PrefabInstanceStaticModel;

class PrefabBaseStaticModel
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
	void Draw();

	// パラメータ
	Engine::Prefab::StaticModel::Base::Param* param_ = nullptr;


private:

	// エンジン
	const GrowthEngine* engine_ = nullptr;

	// 名前
	std::string name_{};

	// プレハブプリミティブハンドル
	PrefabPrimitiveHandle handle_ = 0;
};

