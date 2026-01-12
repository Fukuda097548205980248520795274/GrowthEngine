#pragma once
#include <string>
#include <fstream>
#include <memory>

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

		/// @brief 初期化
		void Initialize();


	private:

		/// @brief 出力ファイルストリーム
		std::ofstream os;
	};


}