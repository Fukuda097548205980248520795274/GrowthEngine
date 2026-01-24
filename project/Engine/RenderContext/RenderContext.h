#pragma once
#include "DX12Debug/DX12Debug.h"
#include "DX12Core/DX12Core.h"
#include "DX12Command/DX12Command.h"
#include "DX12Heap/DX12Heap.h"
#include "DX12Buffering/DX12Buffering.h"
#include "DX12Fence/DX12Fence.h"
#include "ShaderCompiler/ShaderCompiler.h"
#include "DX12Offscreen/DX12Offscreen.h"
#include "ImGuiRender/ImGuiRender.h"
#include <memory>

#include "Store/TextureStore/TextureStore.h"
#include "Store/ModelStore/ModelStore.h"

namespace Engine
{
	class Log;

	class RenderContext
	{
	public:

		/// @brief 初期化
		/// @param log 
		void Initialize(WinApp* winApp, Log* log);

		/// @brief 描画前処理
		void PreDraw();

		/// @brief 描画後処理
		void PostDraw();

		/// @brief テクスチャを読み込む
		/// @param filePath 
		/// @param log 
		TextureHandle LoadTexture(const std::string& filePath, Log* log) { return textureStore_->Load(filePath, heap_.get(), core_->GetDevice(), command_->GetCommandList(), log); }

		/// @brief モデルを読み込む
		/// @param directory 
		/// @param fileName 
		/// @param log 
		/// @return 
		ModelHandle LoadModel(const std::string& directory, const std::string& fileName, Log* log) { return modelStore_->Load(directory, fileName, core_->GetDevice(), log); }


	private:

#ifdef _DEBUG
		// DX12Debug
		std::unique_ptr<DX12Debug> debug_ = nullptr;
#endif

		// DX12Core
		std::unique_ptr<DX12Core> core_ = nullptr;

		// DX12Command
		std::unique_ptr<DX12Command> command_ = nullptr;

		// DX12Heap
		std::unique_ptr<DX12Heap> heap_ = nullptr;

		// DX12Buffering
		std::unique_ptr<DX12Buffering> buffering_ = nullptr;

		// DX12Fence
		std::unique_ptr<DX12Fence> fence_ = nullptr;

		// シェーダコンパイラ
		std::unique_ptr<ShaderCompiler> shaderCompiler_ = nullptr;

		// DX12Offscreen
		std::unique_ptr<DX12Offscreen> offscreen_ = nullptr;


	private:

		// テクスチャストア
		std::unique_ptr<TextureStore> textureStore_ = nullptr;

		// モデルストア
		std::unique_ptr<ModelStore> modelStore_ = nullptr;


	private:

		/// @brief ビューポート
		D3D12_VIEWPORT viewport_{};

		/// @brief シザー矩形
		D3D12_RECT scissorRect_{};


	private:

		// コマンドリスト
		ID3D12GraphicsCommandList* commandList_ = nullptr;

		// コマンドアロケータ
		ID3D12CommandAllocator* commandAllocator_ = nullptr;


	private:

		// ImGui用SRVハンドル
#ifdef _DEVELOPMENT
		std::unique_ptr<ImGuiRender> imguiRender_ = nullptr;
#endif
	};
}