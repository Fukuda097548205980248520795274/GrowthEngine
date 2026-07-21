#include "RenderPassStore.h"
#include <cassert>

#include "RenderContext/DX12Offscreen/RenderTargetPool/RenderTargetPool.h"
#include "RenderContext/DX12Offscreen/DX12Offscreen.h"

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
		{
			data->SetDrawFunc(drawFunc);
			return data->GetHandle();
		}
	}

	// ハンドルを作る
	RenderPassHandle handle = static_cast<RenderPassHandle>(dataTable_.size());

	// 名前を記録する
	nameTable_[name] = handle;

	// レンダーパスを作る
	std::unique_ptr<RenderPassData> renderPass = std::make_unique<RenderPassData>(name, handle, drawFunc);
	dataTable_.push_back(std::move(renderPass));

	return handle;
}

/// @brief レンダーパスに対応する描画関数を実行する
/// @param handle 
/// @param commandList 
/// @param dsvHandle 
/// @return 
Engine::OffscreenResource* Engine::RenderPassStore::RenderPassDraw(RenderPassHandle handle, DX12Offscreen* offscreen, ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
{ 
	// レンダーパスを実行（新しいテクスチャに描き込まれる）
	OffscreenResource* outputResource = dataTable_[handle]->Execute(commandList, renderTargetPool_, offscreen, dsvHandle);
	activeResources_.push_back(outputResource);

	// 新しいテクスチャを最新のテクスチャとして設定する
	offscreen->SetCurrentResource(outputResource);

	return outputResource;
}

/// @brief レンダーパスに対応する描画関数を実行する
/// @param name 
/// @param commandList 
/// @param dsvHandle 
/// @return 
Engine::OffscreenResource* Engine::RenderPassStore::RenderPassDraw(const std::string& name, DX12Offscreen* offscreen, ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
{ 

	// レンダーパスを実行（新しいテクスチャに描き込まれる）
	OffscreenResource* outputResource = dataTable_[nameTable_.at(name)]->Execute(commandList, renderTargetPool_, offscreen, dsvHandle);
	activeResources_.push_back(outputResource);

	// 新しいテクスチャを最新のテクスチャとして設定する
	offscreen->SetCurrentResource(outputResource);

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
	RenderPassData::Param* param = dataTable_[sourceHandle]->GetParam();
	dataTable_[renderTargetHandle]->DrawToRenderPass(commandList, psoFullscreen, dataTable_[sourceHandle]->GetOffscreenResource(), param);
}

/// @brief レンダーパスに描画する
/// @param name 
/// @param offscreen 
/// @param commandList 
/// @param psoFullscreen 
void Engine::RenderPassStore::DrawToRenderPass(const std::string& renderTargetName, const std::string& sourceName,
	ID3D12GraphicsCommandList* commandList,PSOFullscreen* psoFullscreen)
{
	RenderPassData::Param* param = dataTable_[nameTable_.at(sourceName)]->GetParam();
	OffscreenResource* textureResource = dataTable_[nameTable_.at(sourceName)]->GetOffscreenResource();
	dataTable_[nameTable_.at(renderTargetName)]->DrawToRenderPass(commandList, psoFullscreen, textureResource, param);
}

/// @brief 中間リソースを解放する
/// @param resource 
void Engine::RenderPassStore::ReleaseIntermediateResource(OffscreenResource* resource)
{
	// リストの中に該当のリソースがあれば、プールに返却してリストから除外する（二重解放防止）
	auto it = std::find(activeResources_.begin(), activeResources_.end(), resource);
	if (it != activeResources_.end())
	{
		renderTargetPool_->Return(*it);
		activeResources_.erase(it);
	}
}

/// @brief ソースリソースを返却する
/// @param resource 
void Engine::RenderPassStore::ReturnSourceResource(OffscreenResource* resource)
{
	renderTargetPool_->Return(resource); 
}

/// @brief レンダーパスを返却する
void Engine::RenderPassStore::Return()
{
	for (auto& renderPass : activeResources_)
	{
		renderTargetPool_->Return(renderPass);
	}
	activeResources_.clear();
}
