#pragma once
#include <string>
#include "Handle/Handle.h"
#include "Data/PrimitiveData/PrimitiveData.h"

class GrowthEngine;

namespace Engine
{
	class BasePrimitive
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		BasePrimitive(const std::string& name) : name_(name){}

		/// @brief 描画処理
		void Draw();


	protected:


		// エンジン
		const GrowthEngine* engine_ = nullptr;

		// 名前
		std::string name_{};

		// 種類
		Engine::Primitive::Type type_;

		// プリミティブハンドル
		PrimitiveHandle hPrimitive_ = 0;
	};
}