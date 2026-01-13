#pragma once
#include <wrl.h>
#include <string>
#include <dxcapi.h>
// dxcapi.hのインクルードは最後に行うこと

namespace Engine
{
	class Log;

	class ShaderCompiler
	{
	public:

		/// @brief 初期化
		/// @param log 
		void Initialize(Log* log);

		/// @brief コンパイルする
		/// @param filePath 
		/// @param profile 
		/// @return 
		Microsoft::WRL::ComPtr<IDxcBlob> Compile(const std::wstring& filePath, const wchar_t* profile);



	private:

		// ログ
		Log* log_ = nullptr;


		// DXCユーティリティ
		Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_ = nullptr;

		// DXCコンパイラ
		Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;

		// DXCインクルードハンドラ
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;
	};
}