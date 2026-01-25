#pragma once
#include <string>

namespace Engine
{
	class PrimitiveBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		PrimitiveBaseData(const std::string& name) : name_(name){}

		/// @brief 仮想デストラクタ
		virtual ~PrimitiveBaseData() = default;

		/// @brief 更新処理
		virtual void Update() = 0;
		

	protected:

		// 名前
		std::string name_{};

		// 種別名
		std::string typeName_{};
	};
}