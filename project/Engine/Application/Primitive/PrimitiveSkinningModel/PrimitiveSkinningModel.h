#pragma once
#include <string>
#include "Handle/Handle.h"
#include "Data/PrimitiveData/PrimitiveData.h"

class GrowthEngine;

class PrimitiveSkinningModel
{
public:

	/// @brief コンストラクタ
	/// @param hModel 
	/// @param hAnimation 
	/// @param hSkeleton 
	/// @param name 
	PrimitiveSkinningModel(ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton, const std::string& name);

	/// @brief 描画処理
	void Draw();



	/// @brief パラメータ
	Engine::SkinningModel::Param* param_ = nullptr;


private:


	// エンジン
	const GrowthEngine* engine_ = nullptr;


private:

	// 名前
	std::string name_{};

	// 種別名
	std::string type_{};


	// モデルハンドル
	ModelHandle hModel_ = 0;

	/// @brief アニメーションハンドル
	AnimationHandle hAnimation_ = 0;

	/// @brief スケルトンハンドル
	SkeletonHandle hSkeleton_ = 0;


private:


	// プリミティブハンドル
	PrimitiveHandle hPrimitive_ = 0;
};

