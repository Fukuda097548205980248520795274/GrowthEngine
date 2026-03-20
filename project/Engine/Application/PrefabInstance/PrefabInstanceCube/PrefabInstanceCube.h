#pragma once
#include "../BasePrefabInstance.h"

class PrefabInstanceCube : public Engine::BasePrefabInstance
{
public:

	/// @brief コンストラクタ
	/// @param drawCall 
	PrefabInstanceCube(std::function<void(const Engine::Prefab3D::Cube::Instance::Param*)> drawCall,
		Engine::Prefab3D::Cube::Base::Param* param);

	/// @brief 描画処理
	void Draw() override;

	/// @brief パラメータ
	Engine::Prefab3D::Cube::Instance::Param param_;

private:

	/// @brief 描画処理
	std::function<void(const Engine::Prefab3D::Cube::Instance::Param*)> drawCall_;
};

