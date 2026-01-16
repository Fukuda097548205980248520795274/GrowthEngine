#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <utility>

namespace Engine
{
	class WinApp;
	class DX12Heap;
	class DX12Buffering;
	class Log;

	class ImGuiRender
	{
	public:

		/// @brief デストラクタ
		~ImGuiRender();

		/// @brief 初期化
		/// @param device 
		/// @param winApp 
		/// @param heap 
		/// @param buffering 
		/// @param log 
		void Initialize(ID3D12Device* device, WinApp* winApp, DX12Heap* heap, DX12Buffering* buffering, Log* log);

		/// @brief フレーム開始
		void FrameStart();

		/// @brief 終了処理
		void Finalize();

		/// @brief Dockスペースを作成する
		void CreateDockSpace();

		/// @brief ImGuiスクリーンを描画する
		/// @param resource 
		/// @param gpuHandle 
		/// @param commandList 
		void DrawImGuiScreen(ID3D12Resource* resource, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle, ID3D12GraphicsCommandList* commandList);


	private:


		// SRVハンドル
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> srvHandle_;


		// スクリーン横幅
		float screenWidth_ = 0.0f;
		
		// スクリーン縦幅
		float screenHeight_ = 0.0f;
	};
}