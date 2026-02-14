#pragma once
#include <string>
#include "Handle/Handle.h"

class GrowthEngine;

class PrimitiveStaticModel
{
public:

	/// @brief コンストラクタ
	/// @param hModel
	/// @param name 
	PrimitiveStaticModel(ModelHandle hModel, const std::string& name);

	/// @brief 描画処理
	void Draw();


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


private:


	// プリミティブハンドル
	PrimitiveHandle hPrimitive_ = 0;
};

