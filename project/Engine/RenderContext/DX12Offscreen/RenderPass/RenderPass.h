#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <string>
#include <functional>
#include "Handle/Handle.h"

namespace Engine
{
	class RenderTargetPool;
	class OffscreenResource;
	class DX12Offscreen;

	class RenderPass
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param handle 
		/// @param drawFunc 
		RenderPass(const std::string& name, RenderPassHandle handle, std::function<void()> drawFunc) : name_(name), handle_(handle), drawFunc_(drawFunc) {}

		/// @brief 描画関数を実行する
		/// @param commandList 
		/// @param renderTargetPool 
		/// @param dsvHandle 
		OffscreenResource* Execute(ID3D12GraphicsCommandList* commandList, RenderTargetPool* renderTargetPool,DX12Offscreen* offscreen,
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, OffscreenResource* inputResource = nullptr);

		/// @brief レンダーパスを返却する
		/// @param commandList 
		/// @param renderTargetPool 
		void Return(RenderTargetPool* renderTargetPool);

		/// @brief 名前を取得する
		/// @return 
		std::string GetName() const { return name_; }

		/// @brief レンダーパスハンドルを取得する
		/// @return 
		RenderPassHandle GetHandle() const { return handle_; }

		/// @brief レンダーターゲットを取得する
		/// @return 
		OffscreenResource* GetOffscreenResource() const { return offscreenResource_; }

		/// @brief 前のパスから渡されたレンダーターゲットを取得する
		/// @return 
		OffscreenResource* GetInputResource() const { return inputResource_; }

	private:

		/// @brief レンダーパス名
		std::string name_{};

		/// @brief レンダーパスハンドル
		RenderPassHandle handle_ = 0;

		/// @brief レンダーターゲット
		OffscreenResource* offscreenResource_ = nullptr;

		/// @brief 前のパスから渡されたレンダーターゲット
		OffscreenResource* inputResource_ = nullptr;

		/// @brief 描画関数
		std::function<void()> drawFunc_;
	};
}