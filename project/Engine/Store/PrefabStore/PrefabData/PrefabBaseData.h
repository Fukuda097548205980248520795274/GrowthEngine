#pragma once
#include "Handle/Handle.h"
#include <string>

namespace Engine
{
	class PrefabBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		PrefabBaseData(const std::string& name) : name_(name) {}

		/// @brief 仮想デストラクタ
		virtual ~PrefabBaseData() = default;

		/// @brief 更新処理
		virtual void Update() = 0;

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief 種別名を取得する
		/// @return 
		std::string GetTypeName()const { return typeName_; }

		/// @brief ハンドルを取得する
		/// @return 
		PrimitiveHandle GetHandle()const { return hPrefab_; }

		/// @brief パラメータを取得する
		/// @return 
		virtual void* GetParam() = 0;


	protected:

		// 名前
		std::string name_{};

		// 種別名
		std::string typeName_{};

		// ハンドル
		PrefabHandle hPrefab_ = 0;


		// インスタンス数
		uint32_t numInstance_ = 0;
	};
}