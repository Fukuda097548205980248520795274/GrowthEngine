#pragma once
#include "PrimitiveData/PrimitiveStaticModelData/PrimitiveStaticModelData.h"

namespace Engine
{
	class PrimitiveStore
	{
	public:

		/// @brief 読み込み
		/// @param name 
		/// @param typeName 
		void Load(const std::string& name, const std::string& typeName);

		/// @brief 更新処理
		void Update();


	private:


		
	};
}