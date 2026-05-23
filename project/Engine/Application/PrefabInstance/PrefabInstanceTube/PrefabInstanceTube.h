#pragma once
#include "../BasePrefabInstance.h"

class PrefabInstanceTube : public Engine::BasePrefabInstance
{
public:

	/// @brief コンストラクタ
	/// @param drawCall 
	PrefabInstanceTube(std::function<void(const Engine::Prefab3D::Tube::Instance::Param*)> drawCall,
		Engine::Prefab3D::Tube::Base::Param* param);

	/// @brief 描画処理
	void Draw() override;

	/// @brief パラメータ
	Engine::Prefab3D::Tube::Instance::Param param_;

private:

	/// @brief 描画処理
	std::function<void(const Engine::Prefab3D::Tube::Instance::Param*)> drawCall_;
};

