#pragma once
#include <string>
#include "Handle/Handle.h"

class GrowthEngine;

class PrimitiveStaticModel
{
public:

	/// @brief コンストラクタ
	/// @param modelHandle 
	/// @param name 
	PrimitiveStaticModel(ModelHandle modelHandle, const std::string& name);

	/// @brief 描画処理
	void Draw();


private:


	// エンジン
	const GrowthEngine* engine_ = nullptr;


private:

	// 名前
	std::string name_{};

	// モデルハンドル
	ModelHandle modelHandle_ = 0;


private:


};

