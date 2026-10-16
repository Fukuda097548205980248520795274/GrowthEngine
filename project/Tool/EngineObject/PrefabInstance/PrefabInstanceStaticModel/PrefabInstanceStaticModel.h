#pragma once
#include "../BasePrefabInstance.h"

/// @brief 静的モデルプレハブインスタンス
class PrefabInstanceStaticModel : public Engine::BasePrefabInstance
{
public:

	/// @brief コンストラクタ
	/// @param drawCall 
	PrefabInstanceStaticModel(std::function<void(const Engine::Prefab3D::StaticModel::Instance::Param*)> drawCall,
		Engine::Prefab3D::StaticModel::Base::Param* param);

	/// @brief 描画処理
	void Draw() override;

	/// @brief パラメータ
	Engine::Prefab3D::StaticModel::Instance::Param param_;

private:

	/// @brief 描画処理
	std::function<void(const Engine::Prefab3D::StaticModel::Instance::Param*)> drawCall_;
};

