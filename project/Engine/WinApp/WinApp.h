#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>

namespace Engine
{
	class WinApp
	{
	public:

		/// <summary>
		/// ウィンドウプロシージャ
		/// </summary>
		/// <param name="hwnd"></param>
		/// <param name="msg"></param>
		/// <param name="wparam"></param>
		/// <param name="lparam"></param>
		/// <returns></returns>
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


	public:

		/// <summary>
		/// デストラクタ
		/// </summary>
		~WinApp();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(int32_t clientWidth, int32_t clientHeight, const std::string& title);

		/// <summary>
		/// ウィンドウにメッセージを渡して応答する
		/// </summary>
		/// <returns></returns>
		bool ProcessMessage();

		/// <summary>
		/// クライアント領域横幅のGetter
		/// </summary>
		/// <returns></returns>
		int32_t GetClientWidth()const { return clientWidth_; }

		/// <summary>
		/// クライアント領域縦幅のGetter
		/// </summary>
		/// <returns></returns>
		int32_t GetClientHeight()const { return clientHeight_; }

		/// <summary>
		/// クライアント領域横幅のポインタのGetter
		/// </summary>
		/// <returns></returns>
		const int32_t* GetClientWidthP()const { return &clientWidth_; }

		/// <summary>
		/// クライアント領域縦幅のポインタのGetter
		/// </summary>
		/// <returns></returns>
		const int32_t* GetClientHeightP()const { return &clientHeight_; }

		/// <summary>
		/// ウィンドウクラスのGetter
		/// </summary>
		/// <returns></returns>
		WNDCLASS GetWndClass()const { return wc_; }

		/// <summary>
		/// ウィンドウハンドルのGetter
		/// </summary>
		/// <returns></returns>
		HWND GetHwnd()const { return hwnd_; }


	private:

		// ウィンドウクラス
		WNDCLASS wc_{};

		// クライアント領域の横幅
		int32_t clientWidth_ = 0;

		// クライアント領域の縦幅
		int32_t clientHeight_ = 0;

		// ウィンドウハンドル
		HWND hwnd_;
	};
}
