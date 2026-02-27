#pragma once
#include "../Parameter.h"
#include "Math/Vector/Vector2/Vector2.h"
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Vector/Vector4/Vector4.h"

namespace Engine
{
	class PrimitiveParameter : public Parameter
	{
	public:

		/// @brief コンストラクタ
		/// @param settingTypeName 
		/// @param directory 
		PrimitiveParameter(const std::string& settingTypeName, const std::string& directory) : Parameter(settingTypeName, directory){}

		/// @brief 更新処理
		void Update() override;

		/// @brief 登録した調整項目の値に、ファイルの値を反映させる
		/// @param groupName 
		void RegisterGroupDataReflection(const std::string& groupName) override;

		/// @brief ファイルを記録する
		/// @param groupName 
		void SaveFile(const std::string& groupName) override;

		/// @brief 項目に値を入れる
		/// @tparam T 
		/// @param groupName 
		/// @param key 
		/// @param value 
		template<typename T>
		void SetValue(const std::string& groupName, const std::string& key, T* value)
		{
			// グループの参照を取得
			Group& group = data_[groupName];

			// 新しい項目のデータを設定
			Item newItem{};
			newItem = value;

			// 設定した項目を追加
			group[key] = newItem;
		}


	protected:

		// 項目
		using Item = std::variant<bool*, int32_t*, uint32_t*, float*, Vector2*, Vector3*, Vector4*, std::string*>;

		// グループ
		using Group = std::map<std::string, Item>;

		// 1全データ
		using Datas = std::map<std::string, Group>;

		// 全データ
		Datas data_;
	};
}