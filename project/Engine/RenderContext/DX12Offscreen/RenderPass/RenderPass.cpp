#include "RenderPass.h"
#include <cassert>

#include "RenderContext/DX12Offscreen/RenderTargetPool/RenderTargetPool.h"
#include "RenderContext/DX12Offscreen/DX12Offscreen.h"

/// @brief 描画関数を実行する
/// @param commandList 
/// @param renderTargetPool 
Engine::OffscreenResource* Engine::RenderPass::Execute(ID3D12GraphicsCommandList* commandList, RenderTargetPool* renderTargetPool, DX12Offscreen* offscreen,
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
	
	// テクスチャとして使用するためにバリアを張る
	offscreenResource_->Barrier(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	return offscreenResource_;
}

/// @brief レンダーパスに描画する
/// @param commandList 
/// @param psoFullscreen 
/// @param textureResource 
void Engine::RenderPass::DrawToRenderPass(ID3D12GraphicsCommandList* commandList, PSOFullscreen* psoFullscreen, OffscreenResource* textureResource)
{
	assert(commandList);
	assert(psoFullscreen);
	assert(textureResource);

	// PSOを登録する
	psoFullscreen->Register(commandList, BlendMode::kNone);

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
void Engine::RenderPass::Return(RenderTargetPool* renderTargetPool)
{
	// nullptrチェック
	assert(renderTargetPool);

	if (offscreenResource_)
	{
		renderTargetPool->Return(offscreenResource_);
		offscreenResource_ = nullptr;
	}
}