#pragma once
#include <string>
#include <fstream>

namespace Engine
{
	class Log
	{
	public:

		/// @brief コンストラクタ
		Log();

		/// @brief ロギング
		/// @param log 
		void Logging(const std::string& log);


	private:

		/// @brief ファイルを生成する
		std::ofstream CreateLogFile();

		/// @brief 出力ファイルストリーム
		std::ofstream os;
	};


}