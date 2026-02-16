#pragma once
#include <string>
#include "Handle/Handle.h"
#include "Data/PrimitiveData/PrimitiveData.h"

class GrowthEngine;

class PrimitiveAnimationModel
{
public:

	/// @brief コンストラクタ
	/// @param hModel 
	/// @param hAnimation 
	/// @param name 
	PrimitiveAnimationModel(ModelHandle hModel,AnimationHandle hAnimation, const std::string& name);

	/// @brief 描画処理
	void Draw();



	/// @brief パラメータ
	Engine::Primitive::AnimationModel::Param* param_ = nullptr;


private:


	// エンジン
	const GrowthEngine* engine_ = nullptr;


private:

	// 名前
	std::string name_{};

	// 種類
	Engine::Primitive::Type type_;


	// モデルハンドル
	ModelHandle hModel_ = 0;

	/// @brief アニメーションハンドル
	AnimationHandle hAnimation_ = 0;


private:


	// プリミティブハンドル
	PrimitiveHandle hPrimitive_ = 0;
};

