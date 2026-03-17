#pragma once
#include "../BasePrefabInstance.h"

class PrefabInstanceCube : public Engine::BasePrefabInstance
{
public:

	/// @brief コンストラクタ
	/// @param drawCall 
	PrefabInstanceCube(std::function<void(const Engine::Prefab::Cube::Instance::Param*)> drawCall,
		Engine::Prefab::Cube::Base::Param* param);

	/// @brief 描画処理
	void Draw() override;

	/// @brief パラメータ
	Engine::Prefab::Cube::Instance::Param param_;

private:

	/// @brief 描画処理
	std::function<void(const Engine::Prefab::Cube::Instance::Param*)> drawCall_;
};

