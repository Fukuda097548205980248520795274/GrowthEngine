#pragma once
#include "Handle/Handle.h"
#include "Data/PrefabData/PrefabData.h"
#include <string>

namespace Engine
{
	class PrefabPrimitiveBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param numInstance 
		/// @param hPrefab 
		PrefabPrimitiveBaseData(const std::string& name , uint32_t numInstance , PrefabPrimitiveHandle hPrefab) 
			: name_(name), numInstance_(numInstance),hPrefab_(hPrefab) {}

		/// @brief 仮想デストラクタ
		virtual ~PrefabPrimitiveBaseData() = default;

		/// @brief 更新処理
		virtual void Update() = 0;

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief 種類を取得する
		/// @return 
		Prefab::Type GetTypeName()const { return type_; }

		/// @brief ハンドルを取得する
		/// @return 
		PrimitiveHandle GetHandle()const { return hPrefab_; }

		/// @brief パラメータを取得する
		/// @return 
		virtual void* GetParam() = 0;


	protected:

		// 名前
		std::string name_{};

		// 種類
		Prefab::Type type_{};

		// ハンドル
		PrefabPrimitiveHandle hPrefab_ = 0;


		// インスタンス数
		uint32_t numInstance_ = 0;
	};
}