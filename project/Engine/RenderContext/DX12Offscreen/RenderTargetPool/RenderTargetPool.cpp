#include "RenderTargetPool.h"
#include "RenderContext/DX12Heap/DX12Heap.h"
#include "RenderContext/DX12Buffering/DX12Buffering.h"
#include "Func/ConvertString/ConvertString.h"

/// @brief 初期化
/// @param device 
/// @param heap 
/// @param buffering 
/// @param poolSize 
/// @param commandList 
void Engine::RenderTargetPool::Initialize(ID3D12Device* device, DX12Heap* heap, DX12Buffering* buffering, ID3D12GraphicsCommandList* commandList, int poolSize)
{
	// nullptrチェック
	assert(device);
	assert(heap);
	assert(buffering);
	assert(commandList);

	// 引数を受け取る
	device_ = device;
	heap_ = heap;
	buffering_ = buffering;

	int width = static_cast<int>(buffering_->GetSwapChainDesc().Width);
	int height = static_cast<int>(buffering_->GetSwapChainDesc().Height);

	for (int i = 0; i < poolSize; ++i)
	{
		CreateRenderTarget(width, height, commandList);
	}
}

/// @brief 空いているレンダーターゲットを借りる
/// @return 
Engine::OffscreenResource* Engine::RenderTargetPool::Rent(ID3D12GraphicsCommandList* commandList)
{
	// プールが空の場合は新しいレンダーターゲットを作成
	if (freeQueue_.empty())
	{
		int width = static_cast<int>(buffering_->GetSwapChainDesc().Width);
		int height = static_cast<int>(buffering_->GetSwapChainDesc().Height);

		CreateRenderTarget(width, height, commandList);
	}

	// プールからレンダーターゲットを取得
	OffscreenResource* renderTarget = freeQueue_.front();
	freeQueue_.pop();

	return renderTarget;
}

/// @brief 使い終わったレンダーターゲットを返却する
/// @param renderTarget 
void Engine::RenderTargetPool::Return(OffscreenResource* resource)
{
	// nullptrチェック
	if (!resource)return;

	// プールに返却
	freeQueue_.push(resource);
}

/// @brief フレーム終了時にリソースがすべて返却されているか確認する
void Engine::RenderTargetPool::CheckMemoryLeaks()
{
	// フレームの最後で、貸し出したリソースが全て返ってきているか確認
	assert(freeQueue_.size() == resources_.size() && "返却されていないオフスクリーンリソースがある");
}

/// @brief サイズを作り直す
/// @param device 
/// @param width 
/// @param height 
void Engine::RenderTargetPool::Resize(int width, int height)
{
	for (auto& resource : resources_)
	{
		resource->Resize(device_, width, height);
	}
}

/// @brief レンダーターゲットを作成する
/// @param width 
/// @param height 
void Engine::RenderTargetPool::CreateRenderTarget(int width, int height, ID3D12GraphicsCommandList* commandList)
{
	// レンダーターゲットを作成
	auto renderTarget = std::make_unique<OffscreenResource>();
	renderTarget->Initialize(device_, heap_, width, height, nullptr);
	std::string name = "RenderTarget_" + std::to_string(resources_.size());
	renderTarget->GetResource()->SetName(Engine::ConvertString(name).c_str());
	renderTarget->SetName(name);
	renderTarget->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// プールに追加
	freeQueue_.push(renderTarget.get());
	resources_.push_back(std::move(renderTarget));
}