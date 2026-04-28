#include "PostEffectSmoothingData.h"
#include "GrowthEngine.h"
#include "Resource/OffscreenResource/OffscreenResource.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

#include "Parameter/PostEffectParameter/PostEffectParameter.h"

/// @brief 初期化
/// @param device 
/// @param log 
void Engine::PostEffectSmoothingData::Initialize(ID3D12Device* device, Log* log, BasePSOPostEffect* pso)
{
	// nullptrチェック
	assert(device);
	assert(pso);

	// 引数を受け取る
	pso_ = pso;
}

/// @brief リセット
void Engine::PostEffectSmoothingData::Reset()
{
	
}

/// @brief コマンドリストに登録する
/// @param commandList 
void Engine::PostEffectSmoothingData::Register(const PostEffectRenderContext& context)
{
    ID3D12GraphicsCommandList* commandList = context.commandList;
	OffscreenResource* offscreenPixelShaderResource = context.offscreenPixelShaderResource;

	/*------------------------
		コマンドリストに登録
	------------------------*/

	// PSOの設定
	pso_->Register(commandList);

	// テクスチャの設定
	offscreenPixelShaderResource->RegisterGraphics(commandList, 0);

	// 形状の設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ドローコール
	commandList->DrawInstanced(3, 1, 0, 0);

}

/// @brief デバッグ用パラメータ
void Engine::PostEffectSmoothingData::DebugParameter()
{
#ifdef _DEVELOPMENT

#endif
}