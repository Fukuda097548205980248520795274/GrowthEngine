#pragma once
#include <string>
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"
#include "Handle/Handle.h"
#include "Data/PrimitiveData/PrimitiveData.h"

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

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief 種類を取得する
		/// @return 
		Primitive::Type GetType()const { return type_; }

		/// @brief ハンドルを取得する
		/// @return 
		PrimitiveHandle GetHandle()const { return hPrimitive_; }

		/// @brief パラメータを取得する
		/// @return 
		virtual void* GetParam() = 0;
		

	protected:

		// 名前
		std::string name_{};

		// 種別名
		Primitive::Type type_;

		// ハンドル
		PrimitiveHandle hPrimitive_ = 0;
	};
}