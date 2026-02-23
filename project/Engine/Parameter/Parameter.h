#pragma once
#include <json.hpp>
#include <fstream>
#include <string>
#include <variant>

namespace Engine
{
	class Parameter
	{
	public:

		/// @brief コンストラクタ
		/// @param settingTypeName 
		/// @param directory 
		Parameter(const std::string& settingTypeName, const std::string& directory) : settingTypeName_(settingTypeName), directory_(directory) {}

		/// @brief 更新処理
		virtual void Update() = 0;

		/// @brief 登録した調整項目の値に、ファイルの値を反映させる
		/// @param groupName 
		virtual void RegisterGroupDataReflection(const std::string& groupName) = 0;

		/// @brief ファイルを記録する
		/// @param groupName 
		virtual void SaveFile(const std::string& groupName) = 0;


	protected:

		 /// @brief ファイルを作成する
		 /// @param groupName 
		 void CreateRecordFile(const std::string& groupName);


		// 設定の種別名
		std::string settingTypeName_{};

		// ディレクトリパス
		std::string directory_{};


		// JSON
		using json = nlohmann::json;
	};
}