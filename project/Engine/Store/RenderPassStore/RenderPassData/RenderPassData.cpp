#include "RenderPassData.h"
#include <cassert>

#include "RenderContext/DX12Offscreen/RenderTargetPool/RenderTargetPool.h"
#include "RenderContext/DX12Offscreen/DX12Offscreen.h"

/// @brief 描画関数を実行する
/// @param commandList 
/// @param renderTargetPool 
Engine::OffscreenResource* Engine::RenderPassData::Execute(ID3D12GraphicsCommandList* commandList, RenderTargetPool* renderTargetPool, DX12Offscreen* offscreen,
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, OffscreenResource* inputResource)
{
	// nullptrチェック
	assert(commandList);
	assert(renderTargetPool);

	// 前のパスから渡されたレンダーターゲットを保持する
	inputResource_ = inputResource;

	// レンダーターゲットを借りる
	offscreenResource_ = renderTargetPool->Rent(commandList);
	
	// 書き込み先（Destination）には新しく借りたテクスチャをセット
	offscreen->SetDestinationResource(offscreenResource_);

	// 読み込み元（Source）には、前のパスから回ってきた入力テクスチャをセット
	offscreen->SetSourceResource(inputResource_);

	offscreenResource_->Barrier(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	offscreenResource_->ClearRenderTarget(commandList, dsvHandle);

	// 描画処理
	drawFunc_();

	// 最初に持っていたオフスクリーンリソースを記録する
	OffscreenResource* offscreenResource = offscreenResource_;

	OffscreenResource* destinationResource = offscreen->GetDestinationResource();
	if (offscreenResource_ != destinationResource)
	{
		// 描画先が変わってしまった場合は、描画先を戻す
		offscreenResource_ = destinationResource;
	}
	
	// テクスチャとして使用するためにバリアを張る
	offscreenResource_->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	return offscreenResource;
}

/// @brief レンダーパスに描画する
/// @param commandList 
/// @param psoFullscreen 
/// @param textureResource 
void Engine::RenderPassData::DrawToRenderPass(ID3D12GraphicsCommandList* commandList, PSOFullscreen* psoFullscreen, OffscreenResource* textureResource, Param* param)
{
	assert(commandList);
	assert(psoFullscreen);
	assert(textureResource);

	// PSOを登録する
	psoFullscreen->Register(commandList, param->blendMode);

	// レンダーターゲットを設定する
	commandList->SetGraphicsRootDescriptorTable(0, textureResource->GetSrvGpuHandle());

	// 三角形の描画に必要な情報を設定する
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 3つの頂点を描画する
	commandList->DrawInstanced(3, 1, 0, 0);
}

/// @brief レンダーパスを返却する
/// @param commandList 
/// @param renderTargetPool 
void Engine::RenderPassData::Return(RenderTargetPool* renderTargetPool)
{
	// nullptrチェック
	assert(renderTargetPool);

	if (offscreenResource_)
	{
		renderTargetPool->Return(offscreenResource_);
		offscreenResource_ = nullptr;
	}
}