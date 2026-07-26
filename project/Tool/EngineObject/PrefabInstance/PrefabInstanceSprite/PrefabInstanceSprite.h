#pragma once
#include "../BasePrefabInstance.h"

class PrefabInstanceSprite : public Engine::BasePrefabInstance
{
public:

	/// @brief コンストラクタ
	/// @param drawCall 
	PrefabInstanceSprite(std::function<void(const Engine::Prefab2D::Sprite::Instance::Param*)> drawCall, Engine::Prefab2D::Sprite::Base::Param* param);

	/// @brief 描画処理
	void Draw() override;

	/// @brief パラメータ
	Engine::Prefab2D::Sprite::Instance::Param param_;

private:

	/// @brief 描画処理
	std::function<void(const Engine::Prefab2D::Sprite::Instance::Param*)> drawCall_;
};

