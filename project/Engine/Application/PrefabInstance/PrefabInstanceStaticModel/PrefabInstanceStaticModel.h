#pragma once
#include <functional>
#include "Data/PrefabData/PrefabData.h"

class PrefabInstanceStaticModel
{
public:

	/// @brief コンストラクタ
	/// @param drawCall 
	PrefabInstanceStaticModel(std::function<void(const Engine::Prefab::StaticModel::Instance::Param*)> drawCall,
		Engine::Prefab::StaticModel::Base::Param* param);

	/// @brief 描画処理
	void Draw();

	/// @brief 削除
	void Delete() { isDelete = true; }

	/// @brief 削除したかどうか
	/// @return 
	bool IsDelete() { return isDelete; }

	/// @brief パラメータ
	Engine::Prefab::StaticModel::Instance::Param param_;

private:

	/// @brief 描画処理
	std::function<void(const Engine::Prefab::StaticModel::Instance::Param*)> drawCall_;

	// 削除フラグ
	bool isDelete = false;
};

