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
		/// @param folderName 
		Parameter(const std::string& folderName);

		/// @brief ファイルがあるかどうか
		/// @param fileName 
		/// @return 
		bool IsFileFound(const std::string& fileName);

		/// @brief 登録した調整項目の値に、ファイルの値を反映させる
		/// @param fileName 
		virtual void RegisterGroupDataReflection(const std::string& fileName) = 0;

		/// @brief ファイルを記録する
		/// @param fileName 
		virtual void SaveFile(const std::string& fileName) = 0;


	protected:

		 /// @brief ファイルを作成する
		 /// @param fileName 
		 void CreateRecordFile(const std::string& fileName);


		// 設定の種別名
		 std::string folderName_{};

		// ディレクトリパス
		std::string directory_ = "./Assets/Parameter/";


		// JSON
		using json = nlohmann::json;
	};
}