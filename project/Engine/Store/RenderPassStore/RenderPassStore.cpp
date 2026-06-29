#include "RenderPassStore.h"
#include <cassert>

#include "RenderContext/DX12Offscreen/RenderTargetPool/RenderTargetPool.h"

/// @brief 初期化
/// @param renderTargetPool 
void Engine::RenderPassStore::Initialize(RenderTargetPool* renderTargetPool)
{
	// nullptrチェック
	assert(renderTargetPool);

	// 引数を受け取る
	renderTargetPool_ = renderTargetPool;
}

/// @brief 読み込み
/// @param name 
/// @param drawFunc 
/// @return 
RenderPassHandle Engine::RenderPassStore::Load(const std::string& name, std::function<void()> drawFunc)
{
	// 名前テーブルに存在するか確認する
	for (auto& data : dataTable_)
	{
		if (data->GetName() == name)
			return data->GetHandle();
	}

	// ハンドルを作る
	RenderPassHandle handle = static_cast<RenderPassHandle>(dataTable_.size());

	// 名前を記録する
	nameTable_[name] = handle;

	// レンダーパスを作る
	std::unique_ptr<RenderPass> renderPass = std::make_unique<RenderPass>(name, handle, drawFunc);
	dataTable_.push_back(std::move(renderPass));

	return handle;
}

/// @brief レンダーパスに対応する描画関数を実行する
/// @param handle 
/// @param commandList 
/// @param dsvHandle 
/// @return 
Engine::OffscreenResource* Engine::RenderPassStore::RenderPassDraw(RenderPassHandle handle, DX12Offscreen* offscreen, ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, OffscreenResource* inputResource)
{ 
	OffscreenResource* outputResource = dataTable_[handle]->Execute(commandList, renderTargetPool_, offscreen, dsvHandle, inputResource);
	executeList_.push_back(dataTable_[handle].get());
	return outputResource;
}

/// @brief レンダーパスに対応する描画関数を実行する
/// @param name 
/// @param commandList 
/// @param dsvHandle 
/// @return 
Engine::OffscreenResource* Engine::RenderPassStore::RenderPassDraw(const std::string& name, DX12Offscreen* offscreen, ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, OffscreenResource* inputResource)
{ 
	OffscreenResource* outputResource = dataTable_[nameTable_.at(name)]->Execute(commandList, renderTargetPool_, offscreen, dsvHandle, inputResource); 
	executeList_.push_back(dataTable_[nameTable_.at(name)].get()); 
	return outputResource;
}

/// @brief レンダーパスに描画する
/// @param handle 
/// @param offscreen 
/// @param commandList 
/// @param psoFullscreen 
void Engine::RenderPassStore::DrawToRenderPass(RenderPassHandle renderTargetHandle, RenderPassHandle sourceHandle,
	ID3D12GraphicsCommandList* commandList,PSOFullscreen* psoFullscreen)
{
	dataTable_[renderTargetHandle]->DrawToRenderPass(commandList, psoFullscreen, dataTable_[sourceHandle]->GetOffscreenResource());
}

/// @brief レンダーパスに描画する
/// @param name 
/// @param offscreen 
/// @param commandList 
/// @param psoFullscreen 
void Engine::RenderPassStore::DrawToRenderPass(const std::string& renderTargetName, const std::string& sourceName,
	ID3D12GraphicsCommandList* commandList,PSOFullscreen* psoFullscreen)
{
	dataTable_[nameTable_.at(renderTargetName)]->DrawToRenderPass(commandList, psoFullscreen, dataTable_[nameTable_.at(sourceName)]->GetOffscreenResource());
}

/// @brief レンダーパスを返却する
void Engine::RenderPassStore::Return()
{
	for (auto& renderPass : executeList_)
	{
		renderPass->Return(renderTargetPool_);
	}
	executeList_.clear();
}

/// @brief レンダーパスに対応する描画関数をループで実行する
/// @param name 
/// @param commandList 
/// @param dsvHandle 
/// @param firstInput 
/// @param loopCount 
/// @return 
Engine::OffscreenResource* Engine::RenderPassStore::PingPongDraw(const std::string& name, DX12Offscreen* offscreen, ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, OffscreenResource* firstInput, int32_t loopCount)
{
	OffscreenResource* currentTexture = firstInput;

	for (int32_t i = 0; i < loopCount; ++i)
	{
		// パスを実行して新しい結果をもらう
		OffscreenResource* nextTexture = RenderPassDraw(name, offscreen, commandList, dsvHandle, currentTexture);

		// 使い終わったテクスチャを返却する
		if (currentTexture != firstInput && currentTexture != nullptr)
		{
			renderTargetPool_->Return(currentTexture);
		}

		// 次のループのために持ち替える
		currentTexture = nextTexture;
	}

	// 最終的なエフェクト結果のテクスチャを返す
	return currentTexture;
}