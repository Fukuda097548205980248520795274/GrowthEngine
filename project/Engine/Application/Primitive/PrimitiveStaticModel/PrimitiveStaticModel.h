#pragma once
#include "../BasePrimitive.h"

class PrimitiveStaticModel : public Engine::BasePrimitive
{
public:

	/// @brief コンストラクタ
	/// @param hModel
	/// @param name 
	PrimitiveStaticModel(ModelHandle hModel, const std::string& name);

	/// @brief 描画処理
	void Draw() override;

	/// @brief パラメータ
	Engine::Primitive::StaticModel::Param* param_ = nullptr;
};

